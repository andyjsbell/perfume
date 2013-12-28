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
    require("_id"); // email
    require("passwd");
    //require("name");
    //require("desc");
    //require("logo");
    //require("projs");
    return;
}.toString();

// uri: _changes?filter=svc/today&id=xxx
jdata.filters = new Object();
jdata.filters.today = function(doc, req) {
    if (doc._id == req.query.id) {
        return true;
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
fs.writeFile("/tmp/design_user.json", jtxt);

