###
curl -X DELETE http://127.0.0.1:5984/db_users
curl -X PUT http://127.0.0.1:5984/db_users
###
uinfo1="c210820e-3f27-49e9-a4da-ca2a4738890c"
uinfo2="a59caade-6d19-11e3-98d2-17ebdef4450e"
curl -X POST http://127.0.0.1:5984/db_users \
     -H "Content-Type: application/json" \
     -d '{"_id":"user1@gmail.com", "passwd":"passwd1", "uinfo":"'$uinfo1'"}'
curl -X POST http://127.0.0.1:5984/db_users \
     -H "Content-Type: application/json" \
     -d '{"_id":"user2@baidu.com", "passwd":"passwd2", "uinfo":"'$uinfo2'"}'


###
curl -X DELETE http://127.0.0.1:5984/db_uinfo
curl -X PUT http://127.0.0.1:5984/db_uinfo
###
proj1="32ce8be9-ca4a-4515-b6f0-e5d8ecbbe41f"
proj2="417887be-425e-43e9-b5b6-7ff6cdb5d917"
proj3="526cf7cc-6d1b-11e3-8245-0f764373fdf5"
curl -X POST http://127.0.0.1:5984/db_uinfo \
    -H "Content-Type: application/json" \
    -d '{"_id":"'$uinfo1'", "name":"Kancelin", "desc":"Cest La vie!", "logo":"", "date":"2013-12-25", "projs":["'$proj1'","'$proj2'"]}'
curl -X POST http://127.0.0.1:5984/db_uinfo \
    -H "Content-Type: application/json" \
    -d '{"_id":"'$uinfo2'", "name":"Tom James", "desc":"Merry Chrismas!", "logo":"", "date":"2013-12-25", "projs":["'$proj3'"]}'


###
curl -X DELETE http://127.0.0.1:5984/db_projs
curl -X PUT http://127.0.0.1:5984/db_projs
###
curl -X POST http://127.0.0.1:5984/db_projs \
    -H "Content-Type: application/json" \
    -d '{"_id":"'$proj1'", "name":"mobile cost", "desc":"mobile cost, network, ...", "logo":"", "date":"2013-12-15", "creator":"'$uinfo1'", "users":[], "bills":[]}'

curl -X POST http://127.0.0.1:5984/db_projs \
    -H "Content-Type: application/json" \
    -d '{"_id":"'$proj2'", "name":"mobile cost", "desc":"mobile cost, network, ...", "logo":"", "date":"2013-12-15", "creator":"'$uinfo1'", "users":[], "bills":[]}'

curl -X POST http://127.0.0.1:5984/db_projs \
    -H "Content-Type: application/json" \
    -d '{"_id":"'$proj3'", "name":"mobile cost", "desc":"mobile cost, network, ...", "logo":"", "date":"2013-12-15", "creator":"'$uinfo2'", "users":[], "bills":[]}'


###
curl -X DELETE http://127.0.0.1:5984/db_bills
curl -X PUT http://127.0.0.1:5984/db_bills
###
bill1="c39bb870-6d1b-11e3-a0ac-bf37982f503c"
bill2="c8e1ea8e-6d1b-11e3-997f-37eb48d16142"
bill3="cfcba89e-6d1b-11e3-a0f6-8be99dff6263"
curl -X POST http://127.0.0.1:5984/db_bills \
    -H "Content-Type: application/json" \
    -d '{"_id":"'$bill1'", "name":"long way call", "desc":"call with google", "logo":"", "date":"2013-12-15", "creator":"'$uinfo1'", "proj":"'$proj1'", "status":[], "items":[]}'

curl -X POST http://127.0.0.1:5984/db_bills \
    -H "Content-Type: application/json" \
    -d '{"_id":"'$bill2'", "name":"long way call", "desc":"call with google", "logo":"", "date":"2013-12-15", "creator":"'$uinfo2'", "proj":"'$proj2'", "status":[], "items":[]}'

curl -X POST http://127.0.0.1:5984/db_bills \
    -H "Content-Type: application/json" \
    -d '{"_id":"'$bill3'", "name":"long way call", "desc":"call with google", "logo":"", "date":"2013-12-15", "creator":"'$uinfo2'", "proj":"'$proj3'", "status":[], "items":[]}'
