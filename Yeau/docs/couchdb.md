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
    


