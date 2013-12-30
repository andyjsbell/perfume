rhost="http://127.0.0.1:5984"
node=node

db="db_yeau"
$node jsons/yeau_design_ex.js
curl -X DELETE $rhost/$db 2>/tmp/err.log 1>&2
curl -X PUT $rhost/$db 2>/tmp/err.log 1>&2
curl -X POST $rhost/$db \
     -H "Content-Type: application/json" \
     --data @"/tmp/yeau_design.json" 2>/tmp/err.log 1>&2


datas="data_0 data_1 data_2"
for json in $datas;
do
    curl -X POST $rhost/$db \
         -H "Content-Type: application/json" \
        --data @"/tmp/yeau_$json.json" 2>/tmp/err.log 1>&2
done

