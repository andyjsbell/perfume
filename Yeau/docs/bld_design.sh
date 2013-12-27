rhost="http://127.0.0.1:5984"

alldb="db_users db_projs db_bills"
for db in $alldb; 
do
    curl -X DELETE $rhost/$db 2>/tmp/err.log 1>&2
    curl -X PUT $rhost/$db 2>/tmp/err.log 1>&2
    curl -X POST $rhost/$db \
         -H "Content-Type: application/json" \
         --data @"/tmp/design_${db:3:4}.json" 
done

