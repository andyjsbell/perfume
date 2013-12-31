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
function update_data($db, $key, $data) {
    if (!$db || !$key || !$data) return FALSE;

    checkdb($db);
    while(($rec=db_fetch($db)) != NULL) {
        if ($key == $rec._id) {
            print "find rec: $key and remove it";
            db_drop_record($db, $rec.__id);
        }
    }
    db_store($db, $data);
    //db_commit();
    return TRUE;
}
function run_rsync($db, $user, $passwd) {
    $http = {hostname:"127.0.0.1",port:5984,method:"GET"};

    // step1: get remote results
    $http["path"] = "/db_yeau/_changes?filter=svc/myprojs&uid=$user&include_docs=true";
    //$http["authen"] = "Basic $user:$passwd";
    $resp = neon_http($http);
    if ($resp.klass == 2 && !is_null($resp.Content.results)) {
        $rdata = $resp.Content.results;
    }

    // step2: get local results
    $ldata = db_fetch_all($db);

    if (!$rdata) {
        $mdata = $ldata;
        $key = $ldata._id;
        // TODO: sync local into remote
    }else if (!$ldata) {
        // sync remote into local
        foreach($rdata as $ret) {
            update_data($db, $ret.id, $ret.doc);
        }
    }else {
        // merge $ldata and $rdata
        $lcnt = count($ldata);
        print "ldata:$lcnt\n$ldata";
        print "rdata:\n$rdata";

        // sync merged data into local and remote
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
