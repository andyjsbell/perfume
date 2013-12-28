rhost="http://127.0.0.1:5984"
node=node

db="db_yeau"
$node jsons/yeau_design.js
curl -X DELETE $rhost/$db 2>/tmp/err.log 1>&2
curl -X PUT $rhost/$db 2>/tmp/err.log 1>&2
curl -X POST $rhost/$db \
     -H "Content-Type: application/json" \
     --data @"/tmp/yeau_design.json" 2>/tmp/err.log 1>&2


accounts="acco_0 acco_1"
projects="proj_0 proj_1 proj_2"
bills="bill_0 bill_1 bill_2"

all_jsons="$accounts $projects $bills"
for json in $all_jsons;
do
    curl -X POST $rhost/$db \
         -H "Content-Type: application/json" \
        --data @"/tmp/yeau_$json.json" 2>/tmp/err.log 1>&2
done

