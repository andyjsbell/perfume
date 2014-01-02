$tt = [
    {
    id: 12,
    key: 56,
    value: 78
    },
    {id:56}
];
$tt2 = [
    {
    id: 12,
    key: 56,
    value: 79,
    },
    {id:56}
];
print ($tt == $tt2);

$kk = {
    id: 12,
    key: 56,
    value: 78,
    misc: {t1: $tt, t2:0},
    };
$kk2 = {
    misc: {t1: $tt2, t2:0},
    key: 56,
    value: 78,
    id: 12,
    };
print ":::";
print ($kk == $kk2);

foreach ($kk as $k, $v) {
    if ("$k" == "id")
    print "$k ==> $v";
    else
    print "$k => $v";
}

foreach($tt as $t) {
    //print $t;
    if ($t.key){}
}

$rev = "3344-232fafdf";
$int = (int)$rev;
print $int;

$s = {};
$s[t] = test;
print $s;

if ($s.t == test)
    print $s;

