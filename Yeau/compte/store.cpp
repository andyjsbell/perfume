#include "store.h"
#include "jx9util.h"
#include "error.h"

using namespace eau;


StoreImpl::StoreImpl() : m_pHandle(NULL)
{
}

StoreImpl::~StoreImpl()
{
    Close();
}

long StoreImpl::Open(const string &fname, int mode)
{
    if (m_pHandle) {
        return EAU_E_EXIST;
    }

    int ret = unqlite_open((unqlite **)&m_pHandle, fname.c_str(), mode);
    returnv_if_fail(ret == UNQLITE_OK, EAU_E_FAIL);
    return EAU_S_OK;
}

long StoreImpl::Close()
{
    unqlite_close((unqlite *)m_pHandle);
    m_pHandle = NULL;
    return EAU_S_OK;
}

long StoreImpl::PutAccount(const account_t &account)
{
    // set var in script
    map<string, string> kv_map;
    kv_map["user"] = account.user;
    kv_map["passwd"] = account.passwd;
    kv_map["desc"] = account.desc;
    kv_map["cdate"] = account.cdate;
    kv_map["mdate"] = account.mdate;

    long lret = process_jx9_put((unqlite *)m_pHandle, kPutAccountScript, kv_map);
    return lret;
}

long StoreImpl::GetAccount(account_t &account)
{
    // set var in script
    map<string, string> kv_map;
    kv_map["user"] = account.user;

    // get var from script
    map<string, string> io_map;
    //io_map["user"] = account.user;
    io_map["passwd"] = account.passwd;
    io_map["desc"] = account.desc;
    kv_map["cdate"] = account.cdate;
    kv_map["mdate"] = account.mdate;

    long lret = process_jx9_get((unqlite *)m_pHandle, kGetAccountScript, kv_map, io_map);
    return lret;
}

long StoreImpl::PutDB(const db_t &db)
{
    // set var in script
    map<string, string> kv_map;
    kv_map["id"] = db.id;
    kv_map["title"] = db.title;
    kv_map["desc"] = db.desc;
    kv_map["logo"] = db.logo;
    kv_map["status"] = db.status;
    kv_map["cdate"] = db.cdate;
    kv_map["mdate"] = db.mdate;

    long lret = process_jx9_put((unqlite *)m_pHandle, kPutDBScript, kv_map);
    return lret;
}

long StoreImpl::GetDB(db_t &db)
{
    // set var in script
    map<string, string> kv_map;
    kv_map["id"] = db.id;

    // get var from script
    map<string, string> io_map;
    //io_map["id"] = db.id;
    io_map["title"] = db.title;
    io_map["desc"] = db.desc;
    io_map["logo"] = db.logo;
    io_map["status"] = db.status;
    kv_map["cdate"] = db.cdate;
    kv_map["mdate"] = db.mdate;

    long lret = process_jx9_get((unqlite *)m_pHandle, kGetDBScript, kv_map, io_map);
    return lret;
}

long StoreImpl::PutDoc(const doc_t &doc)
{
    // set var in script
    map<string, string> kv_map;
    kv_map["id"] = doc.id;
    kv_map["title"] = doc.title;
    kv_map["desc"] = doc.desc;
    kv_map["logo"] = doc.logo;
    kv_map["status"] = doc.status;
    kv_map["cdate"] = doc.cdate;
    kv_map["mdate"] = doc.mdate;

    long lret = process_jx9_put((unqlite *)m_pHandle, kPutDocScript, kv_map);
    return lret;
}

long StoreImpl::GetDoc(doc_t &doc)
{
    // set var in script
    map<string, string> kv_map;
    kv_map["id"] = doc.id;

    // get var from script
    map<string, string> io_map;
    //io_map["id"] = db.id;
    io_map["title"] = doc.title;
    io_map["desc"] = doc.desc;
    io_map["logo"] = doc.logo;
    io_map["status"] = doc.status;
    kv_map["cdate"] = doc.cdate;
    kv_map["mdate"] = doc.mdate;

    long lret = process_jx9_get((unqlite *)m_pHandle, kGetDocScript, kv_map, io_map);
    return lret;
}

