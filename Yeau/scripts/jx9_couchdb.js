/**
 * jx9 rsync script
 * uskee.org
 */


import 'scripts/jx9_misc.jx9';

function required($var, $msg="required") {
    if (is_null($var)) exit($msg);
}
function checkint($var) {
    if ($var <= 0) exit("$var <= 0");
}
function checkdb($db, $msg="fail") {
    if (!db_exists($db)) {
        if(!db_create($db)) {
            log_error(db_errlog());
            exit($msg);
        }
    }
}
function authenx($name, $passwd) {
    if(!db_exists("user")) return;
    $authcb = function($rec) {
        return (($rec.name == $name) && ($rec.passwd == $passwd));
    };
    if(!db_fetch_all("users", $authcb)) {
        exit("fail to authenx user/passwd");
    }
}
function neon_http($conf) {
    // get config
    $proto = {scheme:"http", hostname:$conf.hostname, port:$conf.port};
    $session = ne_session_create($proto);
    if($session <= 0) return "no session";

    $head = {session:$session, method:$conf.method, path:$conf.path};
    //ne_set_useragent({session:$session, product:"Jx9Agent/1.0"});
    $request = ne_request_create($head);
    if($request <= 0) return;

    // add header
    $req_head = {"Host": "$conf.hostname:$conf.port",
                "Accept": "application/json",
                "User-Agent": "Jx9Agent/1.0"};
    if ($conf.authen) 
        $req_head["Authorization"] = $conf.authen;

    foreach ($req_head as $key, $val) {
        ne_add_request_header({request:$request, name:$key, value:$val});
    }

    // add content and content-length
    if ($conf.body) {
        $ctype = {request:$request, name:"Content-Type", value:"application/json"};
        ne_add_request_header($ctype);
        $ctype = {request:$request, name:"Content-Length", value:strlen($conf.body)};
        ne_add_request_header($ctype);
        $cbody = {request:$request, buffer:"$conf.body", count:strlen($conf.body)};
        ne_set_request_body_buffer($cbody);
    }

    $resp_body = {request:$request};
    $ret = ne_request_dispatch($resp_body);
    if ($ret == 0 && $resp_body["Content"]) {
        $resp["Content"] = json_decode($resp_body["Content"]);
        $resp["Content-Length"] = $resp_body["Content-Length"];
        //print $resp["Content"];
    }else {
        log_error("ne_request_dispatch: $ret"); 
    }

    // get status
    $resp_status = {request:$request};
    $ret = ne_get_status($resp_status);
    if ($ret == 0) {
        $resp["code"] = $resp_status.code;
        $resp["klass"] = $resp_status.klass;
        $resp["reason"] = $resp_status.reason;
    }else {
        log_error("ne_get_status: $ret, $resp_status");
    }

    ne_request_destroy({request:$request});
    ne_close_connection({session:$session});
    ne_session_destroy({session:$session});
    return $resp;
}
function update_local_doc($db, $doc) {
    if (!$db || !$doc) return FALSE;
    checkdb($db);
    while(($rec=db_fetch($db)) != NULL) {
        if ($doc._id == $rec._id) {
            print "find rec: $key and remove it";
            db_drop_record($db, $rec.__id);
        }
    }
    db_store($db, $doc);
    return TRUE;
}
function find_doc($docs, $id) {
    foreach($docs as $doc) 
        if ($doc._id == $id) return $doc;
    return NULL;
}
function merge_doc($ldoc, $rdoc) { // return doc only have validate updates, or return NULL.
    if ($ldoc._id != $rdoc._id) return NULL;
    if ($ldoc._rev == $rdoc._rev) return NULL;

    $lrev = (int)$ldoc._rev;
    $rrev = (int)$rdoc._rev;
    if ($lrev > $rrev) return $rdoc; // local doc is invalid

    $rdoc2 = $get_rdoc_cb($lrev); // get the basic revision of ldoc from remote
    if ($ldoc._rev == $rdoc2._rev) return $rdoc;
    return $ldoc;
}
function get_conflicts($ldoc, $rdoc) {
    // local have conflict with remote
    // init with remote latest: when conflicts happen, use remote latest with priority
    $doc = {name:FALSE, desc:FALSE, users: FALSE, bills: FALSE};
    foreach ($doc as $key, $val) {
        if ($ldoc[$key] != $rdoc[$key])  $doc[$key] = TRUE; // have conflict
    }
    return $doc;
}
function run_rsync($http, $db) {
    $rdocs = [];
    $http["method"] = "GET"};
    $http["path"] = "/db_yeau/_changes?filter=svc/myprojs&uid=$user&include_docs=true";
    $http["body"] = NULL;
    $resp = neon_http($http);
    if ($resp.klass == 2) {
        foreach ($resp.Content.results as $ret) {
            if ($ret.doc)   array_push($rdocs, $ret.doc);
        }
        foreach ($rdocs as $doc) update_local_doc($db, $doc);
    }
    return $rdocs;
}
function run_post_doc($http, $doc) {
    $http["method"] = "POST";
    $http["path"] = "/db_yeau/$doc._id";
    $http["body"] = "$doc";
    return neon_http($http);
}
function run_get_doc($http, $did, int $rev_seq) {
    $http["method"] = "GET";
    $http["path"] = "/db_yeau/$did?revs_info=true";
    $http["body"] = NULL;
    $resp = $neon_http($http);
    $revs = $resp.Content._revs_info;
    for ($revs as $item) {
        $seq = (int) $item.rev;
        if ($seq == $rev_seq) {$rev = $item.rev; break;}
    }
    if ($rev) {
        $http.method = "GET";
        $http["path"] = "/db_yeau/$did?rev=$rev";
        $http["body"] = NULL;
        $resp = $neon_http($http);
    }else {
        $resp = NULL;
    }
    return $resp.Content;
}
function run_local_update($http, $db) {
    $ldocs = [];
    while(($rec=db_fetch($db)) != NULL) {
        $doc = {};
        foreach($rec as $k, $v) 
            if ($k != "__id") $doc[$k] = $v;
        if (!$doc._rev) run_post_doc($http, $doc);
        array_push($ldocs, $doc);
    }
    return $ldocs;
}
function run_db_sync($dbl, $dbr) {
    $ldocs = db_fetch_all($dbl);
    $rdocs = db_fetch_all($dbr);
    if ($ldocs == $rdocs) return;

    foreach($ldocs as $ldoc) {
        $rdoc = find_doc($rdocs, $ldoc._id);
        if ($rdoc) {
            $ndoc = merge_doc($ldoc, $rdoc);
            if ($ndoc != $ldoc)
                update_local_doc($dbl, $ndoc);
        }
    }
}
function run_getter($db, $key) {
    while(($rec=db_fetch($db))) {
        if (rec_contain($key, $rec)) {
            rec_getter($val, $rec);
            break;
        }
    }
    return $val;
}
function run_putter($db, $key, $val, $scm=NULL) {
    //drop old record searched at first, not recursively.
    while(($rec=db_fetch($db))) {
        if (rec_contain($key, $rec)) {
            db_drop_record($db, $rec.__id);
            $new_rec = rec_putter($val, $rec, $scm);
            break;
        }
    }

    if (!$new_rec) {
        $new_rec = rec_putter($key, $scm);
        $new_rec = rec_putter($val, $rec);
    }
    if (is_null($new_rec.id))
        return $jx9_err.e_fail;

    // store new record
    if(!db_store($db, $new_rec)) {
        db_rollback();
        return $jx9_err.e_db_store;
    }
    db_commit();
    return $jx9_err.s_ok;
}

function main() {
    $dbr = "db_yeau_rsync";
    $dbl = "db_yeau_local";

    $tag = $eau_jx9_arg.tag;
    $user = $eau_jx9_arg.user;
    $passwd = $eau_jx9_arg.passwd;
    $host = $eau_jx9_arg.host;
    $port = (int)$eau_jx9_arg.port;

    $tag = "rsync";
    $user = "user1@gmail.com";
    $passwd = "";
    $host = "127.0.0.1";
    $port = 5984;
    $http = {hostname:$host,port:$port};

    required($tag, "no tag");
    required($user, "no user");
    //required($passwd, "no passwd");
    authenx($user, $passwd);

    switch($tag) {
    case "rsync":{ 
        $rdocs = run_rsync($http, $dbr);
        $ldocs = run_local($dbl);
        $updocs = run_conflicts($ldocs, $rdocs);
        break;
    }
    case "getter":
        run_getter($db);
        break;
    case "putter":
        run_putter($db);
        break;
    }
}

main();
