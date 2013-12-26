rhost="http://127.0.0.1:5984"

###
curl -X DELETE $rhost/db_users 2>/tmp/err.log 1>&2
curl -X PUT $rhost/db_users 2>/tmp/err.log 1>&2
curl -X POST $rhost/db_users \
     -H "Content-Type: application/json" \
     --data @"./jsons/design_user.json" 
###
