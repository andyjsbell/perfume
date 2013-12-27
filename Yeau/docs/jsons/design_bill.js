var jdata = new Object();

jdata._id = "_design/bill";
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
    require("cash");
    require("proj");
    //require("desc");
    //require("creator");
    //require("logo");
    //require("status");
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
    if (doc) return "bill: " + doc.name;
    return "invalid bill";
}.toString();

jdata.views = new Object();
jdata.views.bill = new Object();
jdata.views.bill.map = function(doc) {
    if (doc)
        emit(doc.creator, doc.name);
}.toString();

jdata.views.bill.reduce = function(keys, values) {
    return sum(values);
}.toString();

jdata.updates = new Object();
jdata.updates.addesc = function(doc, req) {
    if (!doc || !req.query.desc)
        return [null, "nothing"];
    doc["desc"] = req.query.desc;
    return [doc, "ok"];
}.toString();

var jtxt = JSON.stringify(jdata, null, "\t");
var fs = require('fs');
fs.writeFile("/tmp/design_bill.json", jtxt);

