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
#if 0
    json1_t juri;
    juri.push_back(pair_t("col", "account"));

    json1_t jkey;
    account.id >> jkey;

    json1_t jval;
    account.passwd >> jval;
    account.desc >> jval;

    json1_t jscm;

    json2_t json;
    json["uri"] = juri;
    json["key"] = jkey;
    json["val"] = jval;
    json["scm"] = jscm;
   
    bool bret = process_jx9_json_put((unqlite*)m_pHandle, json);
    log_assert(bret);
#endif
    return 0;
}

long StoreImpl::GetAccount(account_t &account)
{
#if 0
    json1_t juri;
    juri.push_back(pair_t("col", "account"));

    json1_t jkey;
    account.id >> jkey;

    json1_t jout;
    account.passwd >> jout;
    account.desc >> jout;

    json2_t json;
    json["uri"] = juri;
    json["key"] = jkey;
    json["val"] = jout;
   
    long lret = process_jx9_json_get((unqlite*)m_pHandle, json, jout);
    log_assert(lret);
    account.passwd = jout[0];
    account.desc = jout[1];
#endif
    return 0;
}

long StoreImpl::PutDB(const db_t &db)
{
#if 0
    // set var in script
    vector<pair_t> ivar;
    db.id >> ivar;
    db.title >> ivar;
    db.desc >> ivar;
    db.logo >> ivar;
    db.status >> ivar;

    long lret = process_jx9_put((unqlite *)m_pHandle, kPutDBScript, ivar);
    return lret;
#endif
    return 0;
}

long StoreImpl::GetDB(db_t &db)
{
#if 0
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
    db.title = ovar[0];
    db.desc = ovar[1];
    db.logo = ovar[2];
    db.status = ovar[3];
    return lret;
#endif
    return 0;
}

long StoreImpl::PutDoc(const doc_t &doc)
{
#if 0
    // set var in script
    vector<pair_t> ivar;
    doc.id >> ivar;
    doc.title >> ivar;
    doc.desc >> ivar;
    doc.logo >> ivar;
    doc.status >> ivar;

    long lret = process_jx9_put((unqlite *)m_pHandle, kPutDocScript, ivar);
    return lret;
#endif
    return 0;
}

long StoreImpl::GetDoc(doc_t &doc)
{
#if 0
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
    doc.title = ovar[0];
    doc.desc = ovar[1];
    doc.logo = ovar[2];
    doc.status = ovar[3];
    return lret;
#endif
    return 0;
}

