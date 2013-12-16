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

    mode = UNQLITE_OPEN_CREATE;
    int ret = unqlite_open((unqlite **)&m_pHandle, fname.c_str(), mode);
    returnv_assert2(ret, UNQLITE_OK, EAU_E_FAIL);
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
    vector<pair_t> ivar;
    account.user >> ivar;
    account.passwd >> ivar;
    account.desc >> ivar;

    long lret = process_jx9_put((unqlite *)m_pHandle, kPutAccountScript, ivar);
    return lret;
}

long StoreImpl::GetAccount(account_t &account)
{
    // set var in script
    vector<pair_t> ivar;
    account.user >> ivar;

    // get var from script
    vector<pair_t> ovar;
    account.passwd >> ovar;
    account.desc >> ovar;

    long lret = process_jx9_get((unqlite *)m_pHandle, kGetAccountScript, ivar, ovar);
    account.passwd << ovar[0];
    account.desc << ovar[1];

    return lret;
}

long StoreImpl::PutDB(const db_t &db)
{
    // set var in script
    vector<pair_t> ivar;
    db.id >> ivar;
    db.title >> ivar;
    db.desc >> ivar;
    db.logo >> ivar;
    db.status >> ivar;

    long lret = process_jx9_put((unqlite *)m_pHandle, kPutDBScript, ivar);
    return lret;
}

long StoreImpl::GetDB(db_t &db)
{
    // set var in script
    vector<pair_t> ivar;
    db.id >> ivar;

    // get var from script
    vector<pair_t> ovar;
    db.title >> ovar;
    db.desc >> ovar;
    db.logo >> ovar;
    db.status >> ovar;

    long lret = process_jx9_get((unqlite *)m_pHandle, kGetDBScript, ivar, ovar);
    db.title << ovar[0];
    db.desc << ovar[1];
    db.logo << ovar[2];
    db.status << ovar[3];

    return lret;
}

long StoreImpl::PutDoc(const doc_t &doc)
{
    // set var in script
    vector<pair_t> ivar;
    doc.id >> ivar;
    doc.title >> ivar;
    doc.desc >> ivar;
    doc.logo >> ivar;
    doc.status >> ivar;

    long lret = process_jx9_put((unqlite *)m_pHandle, kPutDocScript, ivar);
    return lret;
}

long StoreImpl::GetDoc(doc_t &doc)
{
    // set var in script
    vector<pair_t> ivar;
    doc.id >> ivar;

    // get var from script
    vector<pair_t> ovar;
    doc.title >> ovar;
    doc.desc >> ovar;
    doc.logo >> ovar;
    doc.status >> ovar;

    long lret = process_jx9_get((unqlite *)m_pHandle, kGetDocScript, ivar, ovar);
    doc.title << ovar[0];
    doc.desc << ovar[1];
    doc.logo << ovar[2];
    doc.status << ovar[3];

    return lret;
}

