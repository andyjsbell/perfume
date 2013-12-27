rhost="http://127.0.0.1:5984"
sh bld_design.sh

###
user1="user1@gmail.com"
pass1="pass1"
user2="user2@163.com"
pass2="pass2"

proj1="32ce8be9-ca4a-4515-b6f0-e5d8ecbbe41f"
proj2="417887be-425e-43e9-b5b6-7ff6cdb5d917"
proj3="526cf7cc-6d1b-11e3-8245-0f764373fdf5"

curl -X POST $rhost/db_users \
     -H "Content-Type: application/json" \
     -d '{"_id":"'$user1'", "passwd":"'$pass1'", "name":"Kance Lin", "desc":"Cest La vie!", "logo":"", "projs":["'$proj1'","'$proj2'"]}'

curl -X POST $rhost/db_users \
     -H "Content-Type: application/json" \
     -d '{"_id":"'$user2'", "passwd":"'$pass2'", "name":"Tom James", "desc":"Merry Christmas!", "logo":"", "projs":["'$proj3'"]}'

exit 0

###
curl -X POST $rhost/db_projs \
    -H "Content-Type: application/json" \
    -d '{"_id":"'$proj1'", "name":"mobile cost1", "desc":"mobile cost, network, ...1", "logo":"", "users":[], "bills":[]}'

curl -X POST $rhost/db_projs \
    -H "Content-Type: application/json" \
    -d '{"_id":"'$proj2'", "name":"mobile cost2", "desc":"mobile cost, network, ...2", "logo":"", "users":[], "bills":[]}'

curl -X POST $rhost/db_projs \
    -H "Content-Type: application/json" \
    -d '{"_id":"'$proj3'", "name":"mobile cost3", "desc":"mobile cost, network, ...3", "logo":"", "users":[], "bills":[]}'


###
bill1="c39bb870-6d1b-11e3-a0ac-bf37982f503c"
bill2="c8e1ea8e-6d1b-11e3-997f-37eb48d16142"
bill3="cfcba89e-6d1b-11e3-a0f6-8be99dff6263"
curl -X POST $rhost/db_bills \
    -H "Content-Type: application/json" \
    -d '{"_id":"'$bill1'", "name":"long call1", "cash":100, "desc":"call google1", "logo":"", "proj":"'$proj1'", "status":[], "missing":[]}'

curl -X POST $rhost/db_bills \
    -H "Content-Type: application/json" \
    -d '{"_id":"'$bill2'", "name":"long call2", "cash":500, "desc":"call google2", "logo":"", "proj":"'$proj2'", "status":[], "missing":[]}'

curl -X POST $rhost/db_bills \
    -H "Content-Type: application/json" \
    -d '{"_id":"'$bill3'", "name":"long call3", "cash":1000, "desc":"call google3", "logo":"", "proj":"'$proj3'", "status":[], "missing":[]}'

