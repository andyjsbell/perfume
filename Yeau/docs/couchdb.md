couch db
========

1. Find one
The URI to query to get a view’s result is /database/_design/designdocname/_view/viewname. 
This gives you a list of all rows in the view. 

To find a single document, we would use /blog/_design/docs/_view/by_date?key="2009/01/30 18:04:11" to get the “Biking” blog post. 


2. Find many
/blog/_design/docs/_view/by_date?startkey="2010/01/01 00:00:00"&endkey="2010/02/00 00:00:00". The startkey and endkey parameters specify an inclusive range on which we can search.

Replace 
{
  "date": "2009/01/31 00:00:00"
}
with
{
  "date": [2009, 1, 31, 0, 0, 0]
}
It can be used to find some results more widely, e.g. only by month or day.
And then: /blog/_design/docs/_view/by_date?startkey=[2010, 1, 1, 0, 0, 0]&endkey=[2010, 2, 1, 0, 0, 0].

To retrieve view results in reverse order, use the descending=true query parameter


3. 
(a) For account
[*]Create db:   PUT /db_account
Add user:       PUT /db_account/user1_uuid {id,name,desc}
                PUT /db_account/user2_uuid {id,name,desc}
    

4.
(1) curl -X GET http://127.0.0.1:5984/test_db/_changes
(2) curl -X PUT http://127.0.0.1:5984/test_db/6e1295ed6c29495e54cc05947f18c8af -d '{"title":"There is Nothing Left to Lose","artist":"Foo Fighters"}'
    {"ok":true,"id":"6e1295ed6c29495e54cc05947f18c8af","rev":"1-2902191555"}
(3) curl -X GET http://127.0.0.1:5984/albums/6e1295ed6c29495e54cc05947f18c8af
    {"_id":"6e1295ed6c29495e54cc05947f18c8af","_rev":"1-2902191555","title":"There is Nothing Left to Lose","artist":"Foo Fighters"}

(4)curl -X PUT http://127.0.0.1:5984/albums/6e1295ed6c29495e54cc05947f18c8af \
     -d '{"title":"There is Nothing Left to Lose","artist":"Foo Fighters","year":"1997"}'
    {"error":"conflict","reason":"Document update conflict."}

(5) update or delete
If you want to update or delete a document, CouchDB expects you to include the _rev field of the revision you wish to change.
curl -X PUT http://127.0.0.1:5984/albums/6e1295ed6c29495e54cc05947f18c8af \
     -d '{"_rev":"1-2902191555","title":"There is Nothing Left to Lose","artist":"Foo Fighters","year":"1997"}'
{"ok":true,"id":"6e1295ed6c29495e54cc05947f18c8af","rev":"2-8aff9ee9d06671fa89c99d20a4b3ae"}

(6) attachment
curl -vX PUT http://127.0.0.1:5984/albums/70b50bfa0a4b3aed1f8aff9e92dc16a0 \
     -d '{"title":"Blackened Sky","artist":"Biffy Clyro","year":2002}'
> PUT /albums/70b50bfa0a4b3aed1f8aff9e92dc16a0 HTTP/1.1
>
< HTTP/1.1 201 Created
< Location: http://127.0.0.1:5984/albums/70b50bfa0a4b3aed1f8aff9e92dc16a0
< ETag: "1-e89c99d29d06671fa0a4b3ae8aff9e"
<
{"ok":true,"id":"70b50bfa0a4b3aed1f8aff9e92dc16a0","rev":"1-e89c99d29d06671fa0a4b3ae8aff9e"}

curl -vX PUT http://127.0.0.1:5984/albums/6e1295ed6c29495e54cc05947f18c8af/artwork.jpg?rev=2-2739352689 \
     --data-binary @artwork.jpg -H "Content-Type:image/jpg"
curl -vX GET ...  ?attachments=true: will get base64 of artwork.jpg.

(7) replica: support ["create_target":true]
curl -X PUT http://127.0.0.1:5984/albums-replica
curl -vX POST http://127.0.0.1:5984/_replicate \
     -d '{"source":"albums","target":"albums-replica"}' \
     -H "Content-Type: application/json"

curl -vX POST http://127.0.0.1:5984/_replicate \
     -d '{"source":"albums","target":"http://example.org:5984/albums-replica"}' \
     -H "Content-Type:application/json"

curl -vX POST http://127.0.0.1:5984/_replicate \
     -d '{"source":"http://example.org:5984/albums-replica","target":"albums"}' \
     -H "Content-Type:application/json"

curl -vX POST http://127.0.0.1:5984/_replicate \
     -d '{"source":"http://example.org:5984/albums","target":"http://example.org:5984/albums-replica"}' \
     -H"Content-Type: application/json"

(8) server methods
    /_all_dbs, /_replicat, /_uuids, /_utils, /favicon.ico,
    /_log, /_stats, /_db_updates, /_active_tasks,
    /_session, /_config[/section, /section/key], ..

(9) database methods
    /db, /db/_all_docs, /db/_bulk_docs, /db/_changes, /db/_compact
    /db/_security, /db/_revs_diff, /db/_revs_limit, /db/_purge, ..
    /db/_local/id

(10) document methods
    /db/doc, /db/doc/attachment

(11) design document
    /db/_design/design-doc[/attachment, /_info]
    /db/_design/design-doc/_view/view-name
    /db/_design/design-doc/_show/show-name[/doc-id]
    /db/_design/design-doc/_list/list-name[/view-name,/other-ddoc/view-name]
    /db/_design/design-doc/_update/update-name[/doc-id]
    /db/_design/design-doc/_rewrite/path


curl -X GET http://127.0.0.1:5984/test_db/70b50bfa0a4b3aed1f8aff9e92dc16a0 -u admin1:admin1123
curl -X DELETE http://admin:password@127.0.0.1:5984/_config/admins/admin
