
0. The basic types
    a. string:  ascii + digit
    b. int: digit
    c. uuid: 16-byte string

1. The account structure
{ account: [
    {
        id: 'uuid'
        user: 'string', 
        passwd: 'string', 
        desc: 'string', 
    }, 
    ...
]}

2. The database structure
{ database: [
    {   
        id: 'uuid', 
        title: 'string', 
        desc: 'string', 
        logo: 'string', 
        status: 'string', 
        date: 'string',
        documents: [uuid, ...]
    },
    ...
]}

3. The document structure
{ database/documents: [
    {
        id: 'uuid', 
        title: 'string', 
        desc: 'string', 
        status: 'string', 
        attachments:[uuid,...]
    },
    ...
]}

4. The attachment structure
{ document/attachments: [
    {
        id: 'uuid', 
        title: 'string', 
        desc: 'string', 
        name: 'string', 
        mime: 'string', 
        status: 'string'
    },
    ...
]}
 
 
