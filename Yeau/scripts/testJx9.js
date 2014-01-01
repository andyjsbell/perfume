$tt = [
    {
    id: 12,
    key: 56,
    value: 78
    },
    {id:56}
];
$kk = {
    id: 12,
    key: 56,
    value: 78
    };

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

