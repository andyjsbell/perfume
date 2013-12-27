var dateStr = function() {
    var d = new Date();
    var year = d.getFullYear();
    var mon = d.getMonth() + 1;
    var day = d.getDate();
    var hour = d.getHours();
    var min = d.getMinutes();
    var sec = d.getSeconds();
    return year+"-"+mon+"-"+day+" "+hour+":"+min+":"+sec;
};

console.log(dateStr());
