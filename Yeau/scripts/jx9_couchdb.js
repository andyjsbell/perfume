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
function merge_doc($ldoc, $rdoc, $get_rdoc_cb) {
    if ($ldoc._id != $rdoc._id) return NULL;
    if (!$ldoc._rev || !$rdoc._rev) return NULL;
    $lrev = (int)$ldoc._rev;
    $rrev = (int)$rdoc._rev;
    if ($lrev > $rrev) return NULL; // local doc is invalid

    $doc = {};
    $rdoc2 = $get_rdoc_cb($lrev);
    if ($ldoc._rev != $rdoc2._rev) { // have update or conflict
        if ($rdoc._rev == $rdoc2._rev) { // have update
            foreach($ldoc as $k, $v) 
                if ($k != "__id" && $k != "_rev") $doc[$k] = $v;
        }else { // have conflict
            $doc = {_id:$rdoc._id, name:$rdoc.name, desc:$rdoc.desc, creator:$rdoc.creator};
            if ($rdoc2.name == $rdoc.name) $doc[name] = $ldoc.name;
            if ($rdoc2.desc == $rdoc.desc) $doc[desc] = $ldoc.desc;
            
            // update users
            $doc[users] = {};
            foreach ($rdoc2.users as $uid, $uinfo) {
                $rinfo = $rdoc.users[$uid];
                $linfo = $ldoc.users[$uid];
                if (!$rinfo || !$linfo) { continue; }

                if ($rinfo.role == $uinfo.role) $info.role = $linfo.role;
                if ($rinfo.stat == $uinfo.stat) 
                    $info.stat = $linfo.stat;

                foreach($uinfo.todo as $tid, $tinfo) {
                    if ($rinfo.todo[$tid] == $uinfo.todo[$tid]) {
                        if ($linfo.todo[$tid])
                            $info.todo[$tid] = $linfo.todo[$tid];
                    }
                }
            }
        }
    }else {
        return NULL; // no update between local and remote;
    }
    return $doc;
}
function run_rsync($db, $host, $port, $user, $passwd) {
    // step1
    $http = {hostname:$host,port:$port,method:"GET"};
    $http["path"] = "/db_yeau/_changes?filter=svc/myprojs&uid=$user&include_docs=true";
    //$http["authen"] = "Basic $user:$passwd";
    $resp = neon_http($http);
    if ($resp.klass == 2) {
        $rdocs = [];
        foreach ($resp.Content.results as $ret) {
            if ($ret.doc)   array_push($rdocs, $ret.doc);
        }
    }else { return; }

    // step2: get local docs, 
    $ldocs = db_fetch_all($db);

    // push them into remote server
    $sync2remote = function($doc) {
        uplink $http;
        $http.method = "POST";
        $http["path"] = "/db_yeau/$doc._id";
        $http["body"] = "$doc";
        $resp = neon_http($http);
        print $resp;
    };
    $getrevdoc = function($did, int $rev_seq) {
        uplink $http;
        $http.method = "GET";
        $http["path"] = "/db_yeau/$did?revs_info=true";
        $resp = $neon_http($http);
        $revs = $resp.Content._revs_info;
        for ($revs as $item) {
            $seq = (int) $item.rev;
            if ($seq == $rev_seq) {$rev = $item.rev; break;}
        }
        $resp = NULL;
        if ($rev) {
            $http.method = "GET";
            $http["path"] = "/db_yeau/$did?rev=$rev";
            $resp = $neon_http($http);
        }
        return $resp.Content;
    }

    if($rdocs && $ldocs) { // merge $ldata and $rdata
        $odocs = [];
        foreach($ldocs as $doc) {
            if (!$doc._rev) { $sync2remote($doc);}
            else {array_push($odocs, $doc)};
        }
        $ldocs = $odocs;

        // compared to remote, if update, sync into remote.
        foreach($ldocs as $doc) {
            if(($rdoc=find_doc($rdocs, $doc._id)) == NULL) continue;
            if(($ndoc=merge_doc($doc, $rdoc, $getrevdoc)) == NULL) continue;
            $sync2remote($ndoc);
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
    $tag = $eau_jx9_arg.tag;
    $user = $eau_jx9_arg.user;
    $passwd = $eau_jx9_arg.passwd;
    $tag = "rsync";
    $user = "user1@gmail.com";
    $passwd = "";
    $db = "db_yeau";

    required($tag, "no tag");
    required($user, "no user");
    //required($passwd, "no passwd");
    authenx($user, $passwd);

    switch($tag) {
    case "rsync":
        run_rsync($db, $user, $passwd);
        break;
    case "getter":
        run_getter($db);
        break;
    case "putter":
        run_putter($db);
        break;
    }
}

main();
