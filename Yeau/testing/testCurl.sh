rhost="http://127.0.0.1:5984"

test_myprojs() {
    #curl -X GET "$rhost/db_yeau/_changes?filter=svc/myprojs&uid=user2@163.com"
    #curl -X GET "$rhost/db_yeau/_changes?filter=svc/myprojs&role=owner&uid=user2@163.com"
    #curl -X GET "$rhost/db_yeau/_changes?filter=svc/myprojs&role=member&uid=user2@163.com"

    curl -X GET "$rhost/db_yeau/_changes?filter=svc/myprojs&role=owner&uid=user1@gmail.com"
    curl -X GET "$rhost/db_yeau/_changes?filter=svc/myprojs&role=member&uid=user1@gmail.com"
}

test_setuser() {
    curl -X POST "$rhost/db_yeau/_design/svc/_update/setuser/user2@163.com:proj1" \
        -H "Content-Type: application/json" \
        -d '{"uid":"user2@163.com", "nuid":"user1@gmail.com", "role":"member"}'

}

test_deluser(){
    curl -X POST "$rhost/db_yeau/_design/svc/_update/deluser/user2@163.com:proj1" \
        -H "Content-Type: application/json" \
        -d '{"uid":"user2@163.com", "ouid":"user1@gmail.com"}'
}


test_myprojs;
#test_setuser;
#test_deluser;


