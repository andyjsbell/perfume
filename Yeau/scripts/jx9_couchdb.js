/**
 * jx9 couchdb script
 * uskee.org
 */

function log($msg, $level="info") {
    print "$msg";
}
function required($var, $msg="required") {
    if (is_null($var)) {log($msg, "error"); return FALSE;}
    return TRUE;
}
function checkdb($db) {
    if (!db_exists($db) && !db_create($db)) {
        log(db_errlog());
        return FALSE;
    }
    return TRUE;
}
function neon_http($conf) {
    // get config
    $proto = {scheme:"http", hostname:$conf.hostname, port:$conf.port};
    $session = ne_session_create($proto);
    if($session <= 0) {log("no session"); return NULL;}

    $head = {session:$session, method:$conf.method, path:$conf.path};
    //ne_set_useragent({session:$session, product:"Jx9Agent/1.0"});
    $request = ne_request_create($head);
    if($request <= 0) {log("fail to ne_request_create"); return NULL;}

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
        log("ne_request_dispatch: $ret"); 
    }

    // get status
    $resp_status = {request:$request};
    $ret = ne_get_status($resp_status);
    if ($ret == 0) {
        $resp["code"] = $resp_status.code;
        $resp["klass"] = $resp_status.klass;
        $resp["reason"] = $resp_status.reason;
    }else {
        log("ne_get_status: $ret, $resp_status");
    }

    ne_request_destroy({request:$request});
    ne_close_connection({session:$session});
    ne_session_destroy({session:$session});
    return $resp;
}
function update_doc($db, $doc) {
    if (!$db || !$doc) return FALSE;
    checkdb($db);
    while(($rec=db_fetch($db)) != NULL) {
        if ($doc._id == $rec._id) {
            log("find rec: $key and remove it", "debug");
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
                update_doc($dbl, $ndoc);
        }
    }
}
function register($db, $name, $pass) {
    if (!$db || !$name || !$pass) return FALSE;
    if(!checkdb($db)) return FALSE;
    $authcb = function($rec) {return ($rec.name == $name);};
    if(db_fetch_all($db, $authcb)) {
        log("user: $name has been existed", "error");
        return FALSE;
    }
    $rec = {name:$name, pass:$pass};
    return db_store($db, $rec);
}
function authenx($db, $name, $pass) {
    if (!$db || !$name || !$pass) return FALSE;
    if(!db_exists($db)) return FALSE;
    $authcb = function($rec) {return (($rec.name == $name) && ($rec.pass == $pass));};
    if(!db_fetch_all($db, $authcb)) {
        log("fail to authenx user/passwd", "error");
        return FALSE;
    }
    return TRUE;
}
function signin($db, $name, $pass) {
    return authenx($db, $name, $pass)
}
function run_msync($db, $out) {
    if (!$db) return FALSE;
    if(!db_exists($db)) return FALSE;
    $rall = db_fetch_all($db);
    if (!$rall) {
        log("no available projects", "error");
        return FALSE;
    }
    $out = $rall;
    return TRUE;
}
function run_lsync($db, $projs) {
    if (!$db || !$projs) return FALSE;
    db_drop_collection($db);
    checkdb($db);
    foreach($projs as $proj) {
        db_store($db, $proj);
    }
    db_commit($db);
    return TRUE;
}
function run_rsync($db, $http, $user) {
    $rdocs = [];
    $http["method"] = "GET";
    $http["path"] = "/db_yeau/_changes?filter=svc/myprojs&uid=$user&include_docs=true";
    $http["body"] = NULL;
    $resp = neon_http($http);
    if ($resp.klass != 2)
        return FALSE;

    foreach ($resp.Content.results as $ret) {
        if ($ret.doc)   array_push($rdocs, $ret.doc);
    }
    foreach ($rdocs as $doc) update_doc($db, $doc);
    return TRUE;
}
function run_merge($dbl, $dbr, $http, $user) {
    return TRUE;
}

function main($jx9_arg, $jx9_out) {
    $dbu = "db_users";
    $dbl = "db_yeau_local";
    $dbr = "db_yeau_remote";

    $func = $jx9_arg.func;
    $user = $jx9_arg.user;
    $pass = $jx9_arg.pass;
    $host = $jx9_arg.host;
    $port = (int)$jx9_arg.port;

    $func = "rsync";
    if(!required($func, "no func")) return -1;

    // for testing
    $user = "user1@gmail.com";
    $pass = "";
    $host = "127.0.0.1";
    $port = 5984;

    $bret = FALSE;
    switch($func) {
    case "register":
        $bret = register($dbu, $user, $pass);
        break;
    case "signin":
        $bret = signin($dbu, $user, $pass);
        break;
    case "msync": // local -> memory
        $bret = run_msync($dbl, $jx9_out);
        break;
    case "lsync": // memory -> local
        $bret = run_lsync($dbl, $jx9_arg.projs);
        break;
    case "rsync": // (1) local _rev or latest _rev, (2) server -> remote(_rev) -> local, (3) remote -> local
        if(!required($host, "no host")) return -1;
        if(!required($port, "no port")) return -1;
        $http = {hostname:$host,port:$port};
        $bret = run_rsync($dbr, $http, $user);
        break;
    case "merge": // (1) (local+remote+server) -> server, (2) drop local/remote
        if(!required($host, "no host")) return -1;
        if(!required($port, "no port")) return -1;
        $http = {hostname:$host,port:$port};
        $bret = run_merge($dbl, $dbr, $http, $user);
        break;
    }

    if ($bret)
        return 0;
    return -1;
}


if (!$eau_jx9_arg) return -1;
$eau_jx9_out = {};
return main($eau_jx9_arg, $eau_jx9_out);

