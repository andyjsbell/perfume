rhost="http://127.0.0.1:5984"
node=node

alldb="db_users db_projs db_bills"
for db in $alldb; 
do
    key=${db:3:4}
    $node jsons/design_$key.js
    curl -X DELETE $rhost/$db 2>/tmp/err.log 1>&2
    curl -X PUT $rhost/$db 2>/tmp/err.log 1>&2
    curl -X POST $rhost/$db \
         -H "Content-Type: application/json" \
         --data @"/tmp/design_$key.json" 2>/tmp/err.log 1>&2
done

