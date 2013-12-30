var jdata = new Object();

jdata._id = "_design/svc";
jdata.language = "javascript";

jdata.validate_doc_update = function(newDoc, oldDoc, userCtx, secObj) {
    //throw({ unauthorized: "Error message here." });
    //throw({ forbidden: "Error message here." });
    function require(field, msg) {
        msg = msg || "It must have a " + field;
        if (!newDoc[field]) throw({forbidden : msg});
    };

    require("_id");
    require("type");
    require("name");
    if (newDoc.type == "acco") {
        require("passwd");
        if (newDoc.name != newDoc._id) 
            throw({forbidden : "account's name must equal to _id"});
    }else if (newDoc.type == "proj") {
        require("creator"); // uid
        //require("users");
    }else if (newDoc.type == "bill") {
        require("creator"); // uid
        require("cash");
        require("proj");    // pid
        //require("stats");
    }else {
        throw({forbidden : "only support type of [acco/proj/bill]"});
    }
    //require("desc");
    //require("logo");
    return;
}.toString();

jdata.filters = new Object();
// uri: _changes?filter=svc/myprojs&uid=xx
jdata.filters.myprojs = function(doc, req) {
    var Q = req.query;
    if (!Q || !Q.uid) return false;
    if (doc.type != "proj") return false;
    if (Q.uid == doc.creator) return true;

    user = doc.users[Q.uid];
    if (!user) return false;
    switch(user.stat) {
        case "wait":        break;
        case "approve":     return true;
        case "refuse":      break;
        case "desperate":   break;
    }
    return false;
}.toString();
// uri: _changes?filter=svc/mybills&uid=xx&pid=xx
jdata.filters.mybills = function(doc, req) {
    var Q = req.query;
    if (!Q || !Q.uid || !Q.pid) return false;
    if (doc.type != "bill") return false;
    if (Q.pid != doc.proj) return false;
    if (Q.uid == doc.creator) return true;

    stat = doc.stats[Q.uid];
    if (!stat) return false;
    switch(user.stat) {
        case "draft":       break;
        case "wait":        break;
        case "approve":     return true;
        case "refuse":      break;
        case "desperate":   break;
    }
    return false;
}.toString();

// uri: _design/svc/_show/detail/{docid}
jdata.shows = new Object();
jdata.shows.detail = function(doc, req){
    if (doc) return doc;
    return "invalid docid";
}.toString();

/*
// uri: _design/svc/_view/test
jdata.views = new Object();
jdata.views.test = new Object();
jdata.views.test.map = function(doc) {
    if (doc)
        emit(doc._id, doc.name);
}.toString();
jdata.views.test.reduce = function(keys, values) {
    var msg = "Have " + values.length + " values\n";
    return msg;
}.toString();
*/

// uri: _design/svc/_update/setpass/{docid}?passwd=xxxxxx
jdata.updates = new Object();
jdata.updates.setpass = function(doc, req) {
    //req.query/body/form/userCtx
    if (!doc || doc.type != "acco") return [null, "invalid type"];
    var Q = req.query;
    if (!Q || !Q.passwd) return [null, "invalid Q"];
    doc.passwd = Q.passwd;
    return [doc, "ok"];
}.toString();
// uri: _design/svc/_update/domyproj/{docid}?proj=xx&action=xx
jdata.updates.addproj = function(doc, req) {
    //req.query/body/form/userCtx
    if (!doc || doc.type != "acco") return [null, "invalid type"];
    var Q = req.query;
    if (!Q || !Q.action || !Q.proj) return [null, "invalid Q"];
    if (action == "add") {
        doc.projs.push(Q.proj);
    }else if (action == "del"){
        var index = doc.projs.indexOf(Q.proj);
        if (index != -1)
            doc.projs.splice(index, 1);
    }else {
        return [null, "fail"];
    }
    return [doc, "ok"];
}.toString();

var jtxt = JSON.stringify(jdata, null, "\t");
var fs = require('fs');
fs.writeFile("/tmp/yeau_design.json", jtxt);


///
///==============================================
///
// aim1: query projs of some user: creator/owner/member/viewer/observer

/// accounts: uid, type, name, pass, nick, desc, projs{<pid:[]>}
var uids = ["user1@gmail.com", "user2@163.com"];
var pids = ["proj0", "proj1", "proj2"];
var projs = [
    //_id<pid>, name, desc, creator<uid>, users[*], bills[*], misc
    [0, "proj0", "..",      0,              0,      0,          []],  
    [1, "proj1", "..",      1,              1,      1,          []],  
    [2, "proj2", "..",      2,              2,      2,          []],  
];

var uroles = ["owner", "member", "viewer", "observer"];
var ustats = ["wait", "approve", "refuse", "desperate"];
var todo = ["null", "no", "yes"];
var users = [
    //_id<uid>, role, stat, todo: {uid:xx, uid:xx}
    [0,         0,      1,  [[uid:null]]                ],
];

var bids = ["bill0", "bill1", "bill2", "bill3", "bill4", "bill5"];
var bstats = ["draft", "wait", "approve", "refuse", "desperate"]; 
var todo = ["null", "no", "yes"];
var bills = [
    //_id<bid>, name, desc, cash, creator<uid>, stat, todo:{uid:xx, uid:xx}
    [0,     "bill0", "..",  100,    0,            2,      [[uid:null]]        ],
];


