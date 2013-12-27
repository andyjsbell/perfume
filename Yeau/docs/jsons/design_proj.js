var jdata = new Object();

jdata._id = "_design/proj";
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
    //require("desc");
    //require("creator");
    //require("logo");
    //require("users"); //[email_id, ...]
    //require("bills");
    //require("missing");
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
    if (doc) return "proj: " + doc.name;
    return "invalid proj";
}.toString();

jdata.views = new Object();
jdata.views.proj = new Object();
jdata.views.proj.map = function(doc) {
    if (doc)
        emit(doc.creator, doc.name);
}.toString();

jdata.views.proj.reduce = function(keys, values) {
    return sum(values);
}.toString();

jdata.updates = new Object();
jdata.updates.addesc = function(doc, req) {
    if (!doc || !req.desc)
        return [null, "nothing"];
    doc["desc"] = req.desc;
    return [doc, "ok"];
}.toString();

var jtxt = JSON.stringify(jdata, null, "\t");
var fs = require('fs');
fs.writeFile("/tmp/design_proj.json", jtxt);

