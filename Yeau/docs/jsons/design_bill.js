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
    require("cash");
    require("proj");
    //require("desc");
    //require("creator");
    //require("logo");
    //require("status");
    //require("missing");
    return;
}.toString();

// uri: _changes?filter=svc/owned&creator=xxx
jdata.filters = new Object();
jdata.filters.owned = function(doc, req) {
    if (doc.creator == req.query.creator) {
        return true;
    }
    return false;
}.toString();

// uri: _design/svc/_show/detail/{docid}
jdata.shows = new Object();
jdata.shows.detail = function(doc, req){
    if (doc) return "bill: " + doc.name + ", " + doc.date;
    return "invalid bill";
}.toString();

// uri: _design/svc/_view/bill
jdata.views = new Object();
jdata.views.bill = new Object();
jdata.views.bill.map = function(doc) {
    if (doc)
        emit(doc.creator, doc.name);
}.toString();
jdata.views.bill.reduce = function(keys, values) {
    var msg = "Have " + values.length + " bills";
    return msg;
}.toString();

// uri: _design/svc/_update/setdesc?desc=xxxxxx
jdata.updates = new Object();
jdata.updates.setesc = function(doc, req) {
    if (!doc || !req.query.desc)
        return [null, "nothing"];
    doc.desc = req.query.desc;
    return [doc, "ok"];
}.toString();

var jtxt = JSON.stringify(jdata, null, "\t");
var fs = require('fs');
fs.writeFile("/tmp/design_bill.json", jtxt);

