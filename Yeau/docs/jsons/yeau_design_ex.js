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
    function required(subDoc, field, msg) {
        msg = msg || "It must have a " + field;
        if (!subDoc[field]) throw({forbidden : msg});
    }

    require("_id");
    require("name");
    require("desc");
    require("creator");
    if (newDoc.users) {
        for (k in newDoc.users) {
            user = newDoc.users[k];
            required(user, "role");
            required(user, "stat");
        }
    }
    if (newDoc.bills) {
        for (k in newDoc.bills) {
            bill = newDoc.bills[k];
            required(bill, "name");
            required(bill, "cash");
            required(bill, "creator");
            required(bill, "stat");
            required(bill, "todo");
        }
    }
    return;
}.toString();

jdata.filters = new Object();
/**
 * uri: _changes?filter=svc/myprojs&uid=xx[&role=xx]
 * @return all projs created by uid
 * @return all projs by uid with role[xx]
 */
jdata.filters.myprojs = function(doc, req) {
    var Q = req.query;
    if (!Q || !Q.uid) return false;
    if (!Q.role) {
        if (Q.uid == doc.creator) return true;
    }else {
        user = doc.users[uid];
        if (Q.role == user.role && user.stat == "apporve") 
            return true;
    }
    return false;
}.toString();

/**
 * uri: _design/svc/_show/detail/{docid}
 */
/*
jdata.shows = new Object();
jdata.shows.detail = function(doc, req){
    if (doc) return doc;
    return "invalid docid";
}.toString();
*/

/**
 * uri: _design/svc/_view/test
 */
/*
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

jdata.updates = new Object();
/**
 * uri: _design/svc/_update/addbill/{docid} '{uid, name, cash, desc}'
 */
jdata.updates.addbill = function(doc, req) {
    Q = req.body;
    if (!Q || !Q.uid || !Q.name || !Q.cash) return [null, "fail"];
    
    // check priviledge
    user = doc.users[Q.uid];
    if (!user || user.stat != "apporve") return false;
    if (user.role != "owner" && user.role != "member") return false;

    // create new bill
    var bill = new Object();
    bill = {name:Q.name, cash:Q.cash, creator:Q.uid,
            desc:Q.desc, stat:"draft",
            todo:{}
    };

    // need all owners' approve
    bill.todo[doc.creator] = "ing";
    for (k in doc.users) {
        user = doc.users[k];
        if (user.stat != "apporved" || user.role != "owner") 
            continue;
        bill.todo[user.name] = "ing";
    }

    var d = new Date();
    bid = d.getTime().toString();
    doc.bills[bid] = bill;
    return [doc, "ok"];
}.toString();
/**
 * uri: _design/svc/_update/setbill/{docid} '{uid, bid, name, desc, cash}'
 */
jdata.updates.setbill = function(doc, req) {
    Q = req.body;
    if (!Q || !Q.uid || !Q.bid) return [null, "fail"];

    // check bill stat
    bill = doc.bills[Q.bid];
    if (!bill || bill.stat == "apporve" || bill.stat == "refuse") 
        return [null, "fail"];

    // must be bill creator
    if (bill.creator != bill.uid) return [null, "fail"];

    // update bill data
    if (Q.name) bill.name = Q.name;
    if (Q.desc) bill.desc = Q.desc;
    if (Q.cash) bill.cash = Q.cash;

    // clear bill stat
    if (Q.name || Q.cash || Q.desc) {
        for (k in bill.todo) bill.todo[k] = "ing";
        bill.stat = "draft";
    }
    return [doc, "ok"];
}
/**
 * uri: _design/svc/_update/dobill/{docid} '{uid, bid, action}'
 */
jdata.updates.dobill = function(doc, req) {
    Q = req.body;
    if (!Q || !Q.uid || !Q.bid || !Q.action) return [null, "fail"];
    if (Q.action != "yes" || Q.action != "no" || Q.action != "ing") 
        return [null, "fail"];

    // check bill stat
    bill = doc.bills[Q.bid];
    if (!bill) return [null, "fail"];
    if(bill.stat == "approve" || bill.stat == "refuse") return [null, "fail"];

    if (Q.uid == bill.creator) {
        if (Q.action == "no") {
            bill.stat = "desperate";
            return [doc, "ok"];
        }else if (bill.stat == "desperate") {
            bill.stat = "draft";
            return [doc, "ok"];
        }else if (bill.todo[Q.uid]) {
            bill.todo[Q.uid] = "yes";
            bill.stat = "wait";
        }
    }

    // must be 'wait' for process by other owners
    if(bill.stat != "wait") return [null, "fail"];

    // check user priviledge: owner
    user = doc.users[Q.uid];
    if (!user || user.stat != "apporve" || user.role != "owner") 
        return [null, "fail"];

    bill.todo[Q.uid] = Q.action;
    var stat = "approve";
    for (k in bill.todo) {
        if (bill.todo[k] == "no") {
            stat = "refuse";
        }else if (bill.todo[k] == "ing") {
            stat = "ing";
        }
        if (stat == "refuse") break;
    }
    bill.stat = stat;
    return [doc, "ok"];
} 


var jtxt = JSON.stringify(jdata, null, "\t");
require('fs').writeFile("/tmp/yeau_design.json", jtxt);


///
///==============================================
///
// aim1: query projs of some user: creator/owner/member/viewer/observer

var g_uids = ["user1@gmail.com", "user2@163.com"];
var g_pids = ["proj0", "proj1", "proj2"];
var g_projs = [
//_id<pid>, name, desc, creator<uid>, users[*], bills[*], misc
    [0, "proj0", "..",      0,          [0],      [0],          []],  
    [1, "proj1", "..",      1,          [1],      [1],          []],  
    [2, "proj2", "..",      1,          [2],      [2],          []],  
];

var g_uroles = ["owner", "member", "viewer", "observer"];
var g_ustats = ["wait", "approve", "refuse", "desperate"];
var g_todo = ["ing", "no", "yes"];
var g_users = [
//name<uid>, role, stat, todo: {uid:xx, uid:xx}
    [0,     0,      1,  [[0,0]]                ],
    [1,     0,      1,  [[0,0]]                ],
    [1,     0,      1,  [[0,0]]                ],
];

var g_bids = ["bill0", "bill1", "bill2", "bill3", "bill4", "bill5"];
var g_bstats = ["draft", "wait", "approve", "refuse", "desperate"]; 
var g_todo = ["null", "no", "yes"];
var g_bills = [
//_id<bid>, name, desc, cash, creator<uid>, stat, todo:{uid:xx, uid:xx}
    [0, "bill0", "..",  100,    0,          2,    [[0,0]]        ],
    [1, "bill1", "..",  100,    0,          2,    [[0,0]]        ],
    [2, "bill2", "..",  100,    0,          2,    [[0,0]]        ],
];


for (k in g_projs) {
    proj = g_projs[k];
    p_uid = g_uids[proj[3]];
    p_id = p_uid+":"+g_pids[proj[0]];

    p_users = new Object();
    for (i in proj[4]) {
        user = g_users[proj[4][i]];
        uid = g_uids[user[0]],
        p_users[uid] = {
            role: g_uroles[user[1]],
            stat: g_ustats[user[2]],
            todo: {uid:"ing"}
        };
    }

    p_bills = new Object();
    for (j in proj[5]) {
        bill = g_bills[proj[5][j]];
        creator = g_uids[bill[4]],
        bid = creator+":"+g_bids[bill[0]];
        p_bills[bid] = {
            name: bill[1],
            desc: bill[2],
            cash: bill[3],
            creator: g_uids[bill[4]],
            stat: g_bstats[bill[5]],
            todo: {uid:"ing"}
        };
    };

    var jdata = new Object();
    jdata = {
        _id:p_id, name:proj[1], desc:proj[2], 
        creator:p_uid,
        users: p_users,
        bills: p_bills,
    };
    jtxt = JSON.stringify(jdata, null, "\t");
    require('fs').writeFile("/tmp/yeau_data_"+k+".json", jtxt);
}

