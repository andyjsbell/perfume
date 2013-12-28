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
    if (doc.type && (doc.type == Q.type) && Q.name) {
        if (doc.users.indexOf(Q.name) != -1) {
            return true;
        }
    }
    return false;
}.toString();

// uri: _design/svc/_show/detail/{docid}
jdata.shows = new Object();
jdata.shows.detail = function(doc, req){
    if (doc) return "user: " + doc._id + "\n";
    return "invalid user";
}.toString();

// uri: _design/svc/_view/user
jdata.views = new Object();
jdata.views.user = new Object();
jdata.views.user.map = function(doc) {
    if (doc)
        emit(doc._id, doc.name);
}.toString();
jdata.views.user.reduce = function(keys, values) {
    var msg = "Have " + values.length + " users";
    return msg;
}.toString();

// uri: _design/svc/_update/setpass?passwd=xxxxxx
jdata.updates = new Object();
jdata.updates.setpass = function(doc, req) {
    //req.query/body/form/userCtx
    if (!doc || !req.query.passwd)
        return [null, "nothing"];
    doc.passwd = req.query.passwd;
    return [doc, "ok"];
}.toString();

var jtxt = JSON.stringify(jdata, null, "\t");
var fs = require('fs');
fs.writeFile("/tmp/yeau_design.json", jtxt);


///
///==============================================
///

// accounts
var users = [ // --------------------------------- projs
            ["user1@gmail.com", "pass1", "Nick1", [0, 1]],
            ["user2@163.com", "pass2", "Nick2",   [1, 2]],
            ];
// projects
var projs = [ // ---------------------------------  users ----- bills
            ["32ce8be9-ca4a-4515-b6f0-e5d8ecbbe41f", [1],       [0]],
            ["417887be-425e-43e9-b5b6-7ff6cdb5d917", [0, 1],    [1]],
            ["526cf7cc-6d1b-11e3-8245-0f764373fdf5", [0, 1],    [2]],
            ];
// bills
var bills = [
            ["c39bb870-6d1b-11e3-a0ac-bf37982f503c", 0],
            ["c8e1ea8e-6d1b-11e3-997f-37eb48d16142", 1],
            ["cfcba89e-6d1b-11e3-a0f6-8be99dff6263", 2],
            ];


// for accounts
for (k in users) {
    user = users[k];
    var jdata = new Object();
    jdata = {
        _id:user[0], type:"acco", name:user[0], passwd:user[1], nick:user[2],
        desc:"account for " + user[0], 
        projs:[]
    };
    for (i in user[3]) {
        proj = projs[user[3][i]];
        jdata.projs.push(proj[0]);
    }
    var jtxt = JSON.stringify(jdata, null, "\t");
    require('fs').writeFile("/tmp/yeau_acco_"+k+".json", jtxt);
}

// for projs
for (k in projs) {
    proj = projs[k];
    var jdata = new Object();
    jdata = {
        _id:proj[0], type:"proj", name:"proj"+k, desc:"project for "+k,
        users:[],
        bills:[]
    };
    for (i in proj[1]) {
        user = users[proj[1][i]];
        jdata.users.push(user[0]);
    }
    for (j in proj[2]) {
        bill = bills[proj[2][j]];
        jdata.bills.push(bill[0]);
    }
    var jtxt = JSON.stringify(jdata, null, "\t");
    require('fs').writeFile("/tmp/yeau_proj_"+k+".json", jtxt);
}

// for bills
for (k in bills) {
    bill = bills[k];
    var jdata = new Object();
    jdata = {
        _id:bill[0], type:"bill", name:"bill"+k, desc:"bill for "+k, 
        cash: 100,
        proj:""
    };
    jdata.proj = projs[bill[1]][0];
    var jtxt = JSON.stringify(jdata, null, "\t");
    require('fs').writeFile("/tmp/yeau_bill_"+k+".json", jtxt);
}

