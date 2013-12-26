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
    require("_id");
    require("passwd");
    require("uinfo");
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
    if (doc) return "user: " + doc._id;
    return "invalid user";
}.toString();

jdata.views = new Object();
jdata.views.user = new Object();
jdata.views.user.map = function(doc) {
    if (doc)
        emit(doc._id, doc.passwd);
}.toString();

jdata.views.user.reduce = function(keys, values) {
    return sum(values);
}.toString()

jdata.updates = new Object();
jdata.updates.setpass = function(doc, req) {
    if (!doc || !req.passwd)
        return [null, "nothing"];
    doc.passwd = req.passwd;
    return [doc, "ok"];
}.toString()

var jtxt = JSON.stringify(jdata, null, "\t");
var fs =  require('fs');
fs.writeFile("design_user.json", jtxt);

