var jdata = new Object();

jdata._id = "_design/user";
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

jdata.filters = new Object();
jdata.filters.today = function(doc, req) {
    if (doc.date == "today") {
        return true;
    }
    return true;
}.toString();

jdata.shows = new Object();
jdata.shows.all = function(doc, req){
    if (doc) return "user: " + doc._id + "\n";
    return "invalid user";
}.toString();

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

jdata.updates = new Object();
jdata.updates.setpass = function(doc, req) {
    if (!doc)
        return [null, "no doc"];
    //req.query/body/form/userCtx
    if (!req.query.passwd)
        return [null, "no passwd"];
    doc.passwd = req.query.passwd;
    return [doc, "ok"];
}.toString();

var jtxt = JSON.stringify(jdata, null, "\t");
var fs = require('fs');
fs.writeFile("/tmp/design_user.json", jtxt);

