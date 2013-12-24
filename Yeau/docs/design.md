The Structure
=============

0. The basic types
    a. string:  ascii + digit
    b. int: digit
    c. uuid: 16-byte string

1. The account structure
{ account: {
        id: 'uuid',
        passwd: 'string', 
        name: 'string', 
        desc: 'string', 
        mdate: 'string',
        cdate: 'string',
    }, 
    ...
}

2. The database structure
{ database: {   
        id: 'uuid', 
        title: 'string', 
        desc: 'string', 
        logo: 'string', 
        status: 'string', 
        mdate: 'string',
        cdate: 'string',
        documents: [uuid, ...]
    },
    ...
}

3. The document structure
{ database/documents: {
        id: 'uuid', 
        title: 'string', 
        desc: 'string', 
        status: 'string', 
        mdate: 'string',
        cdate: 'string',
        attachments:[uuid,...]
    },
    ...
}

4. The attachment structure
{ document/attachments: {
        id: 'uuid', 
        title: 'string', 
        desc: 'string', 
        mime: 'string', 
        status: 'string',
        mdate: 'string',
        cdate: 'string',
    },
    ...
}



#############################

1. logging
user/passwd: user1@gmail.com/passwd1
proj role: creator and [owner/member/viewer]
bill state: submit/in-processing/approved

(design1)
GET /db_users/user1@gmail.com
    {"id:", "name:", "passwd", "date", "desc", "logo"}
GET /db_users_projs/user1@gmail.com
    {"id:", "projs[proj1<uuid>, proj2<uuid>]"}
GET /db_users_projs_submit/user1@gmail.com
    {"id:", "projs[proj1<uuid>, proj2<uuid>]"}

GET /db_projs/proj1_uuid
    {"id<uuid>:", "name:", "creator<id>", "users[id1,id2]", "date", "desc", "logo"}
GET /db_projs_bills/proj1_uuid
    {"id<uuid>:", "bills[bill1<uuid>, bill2<uuid>]"}
GET /db_projs_bills_approve/proj1_uuid
    {"id<uuid>:", "bills[bill1<uuid>, bill2<uuid>]"}
GET /db_projs_bills_submit/proj1_uuid
    {"id<uuid>:", "bills[bill1<uuid>, bill2<uuid>]"}

GET /db_bills/bill1_uuid
    {"id<uuid>":, "name:", "creator<id>", "cash", "state[owners]", "date", "desc", "logo", "proj(uuid)"}
GET /db_bills_annex/bill1_uuid
    {"id":, "items[attachments]"}




#############################

1. logging
user/passwd: user1@gmail.com/passwd1
proj roles: creator/owner/member/viewer
proj state: invite/wait/approve
bill state: draft/submit/process/approve/refuse

(design2)
GET /db_users/_all_docs
GET /db_users/{docid}
GET /db_users/user1@gmail.com
    {"_id<uid>", 
        passwd, 
        db_uinfo<uuid,uri>
    }

GET /db_uinfo/_all_docs
GET /db_uinfo/{docid}
    {_id<uuid>, 
        name, desc, date, logo,
        db_projs[<uuid>,..],
    }

GET /db_projs/_all_docs
GET /db_projs/{docid}
    {_id<uuid>, creator<uuid>,
        name, desc, date, logo,
        users[<uuid, prole, pstate>]
        bills[<uuid, bstate>, ..],
    }

GET /db_bills/_all_docs
GET /db_bills/{docid}
    {_id<uuid>, creator<uuid>,
        name, desc, date, logo,
        cash,
        db_proj<uuid>, 
        items[attachments]
    }


//example:
(a) TODO db
    check db of db_users: HEAD /db_users
    create db of db_users: PUT /db_users
    create doc of user1@gmail.com: POST /db_users  {_id:user1@gmail.com, ...., uinfo:db_uinfo_uuid}
    create db of db_uinfo_uuid: PUT /db_uinfo_uuid [uuid or email]

[note] 
    HEAD /{db} => 200 OK/404 Not Found
    GET /{db} => 200/404, {committed_update_seq,db_name,doc_count,update_seq, ...}
    PUT /{db} => 201/400/401/412, {ok, error, reason}
    POST /{db}[?batch=ok] [{_id, X-Couch-Full-Commit:true/false}] => 201/202/400/401/404/409, {id,ok,rev}

(b) TODO doc
[note]
    HEAD /{db}/{doc}, if-None-Match: $rev => 200/304/401/404, Content-Length/ETag
    GET /{db}/{doc}, if-None-Match: $rev, {..} => 200/304/400/401/404, Content-Type/ETag, {_id,_rev,...}
    PUT /db/doc, {..} => Content-Type/ETag/Location, {id,ok,rev}
    GET /db/doc?attachments=true => return document with all attached files content 


======================================================

/**
 * step1: register and signin
 */
POST /db_users
{_id:user1@gmail.com, passwd:passwd1,db_uinfo:uinfo1_uuid}


/**
 * step2: create db_uinfo
 */
POST /db_uinfo
{_id:uinfo1_uuid, name, desc, date, logo, db_projs[]}


/**
 * step3: create db_projs
 */
POST /db_projs
{_id:proj1_uuid, 
    name, desc, date, logo, 
    users: [uinfo1_uuid:[
                        prole,
                        status: [uinfo1_uuid:pstate] ##TODO
                        ]
            ],
    creator:uinfo1_uuid, 
    bills: []
}
PUT /db_uinfo/uinfo1_uuid
{db_projs[proj1_uuid, ..]}


/**
 * step4: create db_bills
 */
POST /db_bills
{_id:bill1_uuid, 
    name, desc, date, logo, 
    cash,
    status: [uinfo1_uuid:bstate, ], ##TODO
    creator: uinfo1_uuid, 
    db_proj: proj1_uuid, 
    items[]
}
PUT /db_projs/proj1_uuid
{bills=[bill1_uuid, ..]}

