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
    };

    require("_id");
    require("name");
    //require("desc");
    // default: project creator is not its owner
    require("creator"); 
    if (newDoc._id.indexOf(newDoc.creator+":") == -1) {
        throw({forbidden : "invalid project id"});
    }

    if (oldDoc && (oldDoc._id !== newDoc._id || oldDoc.creator !== newDoc.creator)) {
        throw({forbidden : "invalid project id or creator"});
    }

    if (newDoc.users) {
        for (k in newDoc.users) {
            user = newDoc.users[k];
            required(user, "role");
            required(user, "stat");
            required(user, "todo");
        }
    }

    if (newDoc.bills) {
        for (k in newDoc.bills) {
            bill = newDoc.bills[k];
            required(bill, "name");
            //required(bill, "desc");
            required(bill, "cash");
            required(bill, "creator");
            required(bill, "stat");
            required(bill, "todo");
        }
    }
}.toString();

jdata.filters = new Object();
/**
 * uri: _changes?filter=svc/myprojs&uid=xx[&role=xx]
 * @return all projs created by uid
 * @return all projs by uid with role[xx]
 */
jdata.filters.myprojs = function(doc, req) {
    if (doc._id.indexOf("_design/") == 0) return false;
    var Q = req.query;
    if (!Q || !Q.uid) return false;
    if (!Q.role) {
        if (Q.uid == doc.creator) return true;
    }else {
        user = doc.users[Q.uid];
        if (Q.role == user.role && user.stat == "approve")  return true;
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
 * uri: _design/svc/_update/setuser/{docid} '{uid, nuid, role}'
 */
jdata.updates.setuser = function(doc, req) {
    if (doc._id.indexOf("_design/") == 0) return [null, "unsupport _design doc"];
    eval('Q='+req.body);
    if (!Q || !Q.uid || !Q.nuid || !Q.role) return [null, "invalid body"];

    // Only owner/creator of the project can add new user
    if (Q.uid != doc.creator) {
        user = doc.users[Q.uid];
        if (!user || user.stat != "approve" || user.role != "owner")    return [null, "no privilege"];
    }
    
    // Check whether this nuid exist or not
    nuser = doc.users[Q.nuid];
    if (nuser) {
        // Only project creator have privilege to update other owners
        if (nuser.role == "owner") {
            if (Q.uid == doc.creator) {
                nuser.stat = "approve";
                nuser.role = Q.role;
            }
        }else {
            nuser.stat = "approve";
            nuser.role = Q.role;
        }
    }else {
        nuser = {stat:"approve", role:Q.role, todo:{}};
    }

    function updateowner(uid_, do_) {
        for (k in doc.bills) {
            bill_ = doc.bills[k];
            if (bill_.stat != "wait") continue;
            if (do_ == "delete" && bill_.todo[uid_])
                delete bill_.todo[uid_];
            else if (do_== "new" && !bill_[k].todo[uid_])
                bill_.todo[uid_] = "ing";
        }
    }

    // TODO: update bills' todo
    doc.users[Q.nuid] = nuser;
    if (nuser.role == "owner") {
        updateowner(Q.nuid, "new");
    }else {
        updateowner(Q.nuid, "delete");
    }

    return [doc, "ok"];
}.toString();
/**
 * uri: _design/svc/_update/deluser/{docid} '{uid, ouid}'
 */
jdata.updates.deluser = function(doc, req) {
    if (doc._id.indexOf("_design/") == 0) return [null, "unsupport _design doc"];
    eval('Q='+req.body);
    if (!Q || !Q.uid || !Q.ouid) return [null, "fail"];

    ouser = doc.users[Q.ouid];
    if (!ouser) return [null, "fail"];

    // creator: can del all, owner: can del all except owners, 
    if (Q.uid != doc.creator) {
        if (ouser.role == "owner") return [null, "fail"];
        user = doc.users[Q.uid];
        if (!user || user.stat != "approve" || user.role != "owner")    return [null, "fail"];
    }

    // TODO: update bill's todo if ouid is owner
    if (ouser.role == "owner") {
        for (k in doc.bills) {
            bill = doc.bills[k];
            if (bill.stat == "wait" && bill.todo[ouid]) {
                delete bill.todo[Q.ouid];
            }
        }
    }
    delete doc.users[Q.ouid];
    return [doc, "ok"];
}.toString();

/**
 * uri: _design/svc/_update/addbill/{docid} '{uid, name, cash, desc}'
 */
jdata.updates.addbill = function(doc, req) {
    if (doc._id.indexOf("_design/") == 0) return [null, "unsupport _design doc"];
    eval('Q='+req.body);
    if (!Q || !Q.uid || !Q.name || !Q.cash) return [null, "fail"];
    
    // Only member/owner have authority.
    user = doc.users[Q.uid];
    if (!user || user.stat != "approve") return false;
    if (user.role != "owner" && user.role != "member") return false;

    // To create new bill, default 'draft'
    var bill = new Object();
    bill = {name:Q.name, cash:Q.cash, creator:Q.uid,
            desc:Q.desc, stat:"draft",
            todo:{}
    };

    var d = new Date();
    bid = d.getTime().toString();
    doc.bills[bid] = bill; // TODO: which format of bid is suitable?
    return [doc, "ok"];
}.toString();
/**
 * uri: _design/svc/_update/setbill/{docid} '{uid, bid, name, desc, cash}'
 */
jdata.updates.setbill = function(doc, req) {
    if (doc._id.indexOf("_design/") == 0) return [null, "unsupport _design doc"];
    eval('Q='+req.body);
    if (!Q || !Q.uid || !Q.bid) return [null, "invalid body"];

    // check bill stat
    bill = doc.bills[Q.bid];
    if (!bill || bill.stat == "approve" || bill.stat == "refuse") 
        return [null, "fail"];

    // must be bill creator
    if (Q.uid != bill.creator) return [null, "only bill creator can modify bill!"];

    // update bill data
    if (Q.name) bill.name = Q.name;
    if (Q.desc) bill.desc = Q.desc;
    if (Q.cash) bill.cash = Q.cash;

    // clear bill stat
    if (Q.name || Q.cash || Q.desc) {
        bill.stat = "draft";
        bill.todo = {};
        return [doc, "ok"];
    }
    return [null, "no update"];
}.toString();
/**
 * uri: _design/svc/_update/dobill/{docid} '{uid, bid, action}'
 */
jdata.updates.dobill = function(doc, req) {
    if (doc._id.indexOf("_design/") == 0) return [null, "unsupport _design doc"];
    eval('Q='+req.body);
    if (!Q || !Q.uid || !Q.bid || !Q.action) return [null, "invalid body"];
    if ("ing,no,yes".indexOf(Q.action) == -1) return [null, "invalid action"];

    // check bill stat, cannot process ultimate states: 'approve/refuse'
    bill = doc.bills[Q.bid];
    if (!bill) return [null, "invalid bid"];
    if(bill.stat == "approve" || bill.stat == "refuse") return [null, "bill done!"];

    // for bill's creator, 'desperate' is only valid for creator
    if (Q.uid == bill.creator) {
        if (Q.action == "no") {
            bill.stat = "desperate";
            return [doc, "ok"];
        }else if (bill.stat == "desperate") {
            bill.stat = "draft";
            return [doc, "ok"];
        }
        // It needs all owners' approve(maybe no creator), so init with "ing" state
        bill.todo = {};
        for (k in doc.users) {
            user = doc.users[k];
            if (user.stat != "approve" || user.role != "owner") 
                continue;
            bill.todo[user.name] = "ing";
        }
        if (bill.todo[Q.uid])   bill.todo[Q.uid] = "yes";
        bill.stat = "wait";
    }

    // Bill must be 'wait' for processing by other owners
    if(bill.stat != "wait") return [null, "fail"];

    // Only owner can verify the bill
    user = doc.users[Q.uid];
    if (!user || user.stat != "approve" || user.role != "owner") 
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
}.toString();


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
    [0, "proj0", "..",      0,          [0,1],      [0],          []],  
    [1, "proj1", "..",      1,          [2],      [1],          []],  
    [2, "proj2", "..",      1,          [2,3],      [2],          []],  
];

var g_uroles = ["owner", "member", "viewer", "observer"];
var g_ustats = ["wait", "approve", "refuse", "desperate"];
var g_todo = ["ing", "no", "yes"];
var g_users = [
//name<uid>, role, stat, todo: {uid:xx, uid:xx}
    [0,     0,      1,  [[0,0]]                ],
    [1,     0,      1,  [[0,0]]                ],

    [1,     0,      1,  [[0,0]]                ],

    [0,     0,      1,  [[0,0]]                ],
];

var g_bids = ["bill0", "bill1", "bill2", "bill3", "bill4", "bill5"];
var g_bstats = ["draft", "wait", "approve", "refuse", "desperate"]; 
var g_todo = ["null", "no", "yes"];
var g_bills = [
//_id<bid>, name, desc, cash, creator<uid>, stat, todo:{uid:xx, uid:xx}
    [0, "bill0", "..",  100,    1,          2,    [[0,0]]        ],
    [1, "bill1", "..",  100,    1,          2,    [[0,0]]        ],
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

