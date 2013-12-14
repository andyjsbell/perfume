#include "store.h"
#include "jx9util.h"

using namespace eau;


StoreImpl::StoreImpl() : m_pDB(NULL)
{
}

StoreImpl::~StoreImpl()
{}


long StoreImpl::Open(string &szName, int iMode)
{
    returnv_if_fail(!szName.empty(), EAU_E_INVALIDARG);

    if (m_pDB) {
        return EAU_E_EXIST;
    }

    int ret = unqlite_open(&m_pDB, szName.c_str(), iMode);
    returnv_if_fail(ret == UNQLITE_OK, EAU_E_FAIL);
    return EAU_S_OK;
}

long StoreImpl::Close()
{
    unqlite_close(m_pDB);
    m_pDB = NULL;
    return EAU_S_OK;
}

long StoreImpl::PutAccount(const account_t &account)
{
    // set var in script
    map<string, string> kv_map;
    kv_map["user"] = account.user;
    kv_map["passwd"] = account.passwd;
    kv_map["desc"] = account.desc;

    long lret = process_jx9_put(m_pDB, kPutAccountScript, kv_map);
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

    long lret = process_jx9_get(m_pDB, kGetAccountScript, kv_map, io_map);
    return lret;
}

long StoreImpl::PutDB(db_t &db)
{
    // set var in script
    map<string, string> kv_map;
    kv_map["id"] = db.id;
    kv_map["title"] = db.title;
    kv_map["desc"] = db.desc;
    kv_map["logo"] = db.logo;
    kv_map["status"] = db.status;
    kv_map["date"] = db.date;

    long lret = process_jx9_put(m_pDB, kPutDBScript, kv_map);
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
    io_map["date"] = db.date;

    long lret = process_jx9_get(m_pDB, kGetDBScript, kv_map, io_map);
    return lret;
}

