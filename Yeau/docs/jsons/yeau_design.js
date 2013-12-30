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
    require("name");
    require("type");
    if (newDoc.type == "acco") {
        require("passwd");
        if (newDoc.name != newDoc._id) 
            throw({forbidden : "account's name must equal to _id"});
    }else if (newDoc.type == "proj") {
    }else if (newDoc.type == "bill") {
    }else {
        throw({forbidden : "only support type of [acco/proj/bill]"});
    }
    //require("desc");
    //require("logo");
    //require("projs");
    return;
}.toString();

// uri: _changes?filter=svc/owned&name=""&type="acco/proj/bill"
jdata.filters = new Object();
jdata.filters.owned = function(doc, req) {
    var Q = req.query;
    if (!Q || !Q.type || !Q.name) return false;
    if (doc.type == Q.type) {
        if (doc.users.indexOf(Q.name) != -1) {
            return true;
        }
    }
    return false;
}.toString();

// uri: _design/svc/_show/detail/{docid}
jdata.shows = new Object();
jdata.shows.detail = function(doc, req){
    if (doc) return doc;
    return "invalid docid";
}.toString();

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
var users = [ 
    // uid,              pass,    nick,      projs[<pid>] 
    ["user1@gmail.com", "pass1", "Nick1",   [0,1,2] ],
    ["user2@163.com",   "pass2", "Nick2",   [0,2]   ],
];

/// projects: pid, type, name, desc, creator, 
///             users{<uid:{urole, pstat:xx, todo:{<uid:xx>}}>}, 
///             bills{<bid:[]>}
var uroles = "creator" + ["owner", "member", "viewer", "observer"];
var pstats = ["wait", "approve", "refuse", "desperated"] + ["yes", "no"];
var projs = [ 
    // pid,  creator<uid>, users{uid:{role:xx, stat:xx, todo:{uid:xx}}, bills{bid:[]},
    ["proj0",0,     
        [[0, "owner", "approve"]],
        [0,1],
    ],
    ["proj1",1,
        [[1, "owner", "approve"], [0,"member", "approve"]],
        [5],
    ],
    ["proj2",1,
        [[1, "owner", "approve"], [0,"member", "approve"]],
        [2,3,4],
    ],
];

/// bills: bid, type, name, desc, creator, cash, pid, stats{stat:xx, todo:{uid:xx}}
var bstats = ["draft", "wait", "approve", "refuse", "desperated"] + ["yes", "no"];
var bills = [ 
    // bid,  creator<uid>, pid, stats{stat:xx, todo:{uid:xx}}
    ["bill0",0,     0, {stat:"approve", todo:{uid:null}} ],
    ["bill1",0,     1, {stat:"approve", todo:{uid:null}} ],
    ["bill2",1,     2, {stat:"approve", todo:{uid:null}} ],
    ["bill3",1,     2, {stat:"approve", todo:{uid:null}} ],
    ["bill4",0,     2, {stat:"approve", todo:{uid:null}} ],
    ["bill5",0,     1, {stat:"apporve", todo:{uid:null}} ] ,
];


// for accounts
for (k in users) {
    user = users[k];
    var jdata = new Object();
    jdata = {
        _id:user[0], type:"acco", name:user[0], pass:user[1], nick:user[2],
        desc:"account for " + user[0], 
        projs:{} // 'pid':[]
    };
    for (i in user[3]) {
        uproj = projs[user[3][i]];
        pcreator = users[uproj[1]][0];
        pid = pcreator+":"+uproj[0];
        jdata.projs[pid] = [];
    }
    jtxt = JSON.stringify(jdata, null, "\t");
    require('fs').writeFile("/tmp/yeau_acco_"+k+".json", jtxt);
}

// for projects: id => user:proj
for (k in projs) {
    proj = projs[k];
    creator = users[proj[1]][0];
    var jdata = new Object();
    jdata = {
        _id:creator+":"+proj[0], type:"proj", name:"proj"+k, desc:"project for "+k, 
        creator:creator,
        users:{}, // 'uid:[role,stat]'
        bills:{}, // 'bid:[]'
    };
    for (i in proj[2]) {
        pusers = proj[2][i];
        uid = users[pusers[0]][0];
        jdata.users[uid] = {role:pusers[1], stat:pusers[2]};
        jdata.users[uid]["todo"] = {uid:null};
    }
    for (j in proj[3]) {
        pbill = proj[3][j];
        bill = bills[pbill];
        bcreator = users[bill[1]][0];
        bid = bcreator + ":" + bill[0];
        jdata.bills[bid] = [];
    }
    jtxt = JSON.stringify(jdata, null, "\t");
    require('fs').writeFile("/tmp/yeau_proj_"+k+".json", jtxt);
}

// for bills
for (k in bills) {
    bill = bills[k];
    creator = users[bill[1]][0];
    projc = users[projs[bill[2]][1]][0];
    projn = projs[bill[2]][0];
    var jdata = new Object();
    jdata = {
        _id:creator+":"+bill[0], type:"bill", name:"bill"+k, desc:"bill for "+k, 
        creator:creator,
        cash: 100,
        proj: projc+":"+projn,
        stats: {stat:"approve"},
    };
    jdata.stats["todo"] = {uid:null};
    jtxt = JSON.stringify(jdata, null, "\t");
    require('fs').writeFile("/tmp/yeau_bill_"+k+".json", jtxt);
}

