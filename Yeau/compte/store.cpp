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

long StoreImpl::PutAccount(account_t &acc)
{
    json1_t jkey;
    acc.get(K_id) >> jkey;

    json1_t jval;
    acc.get(K_passwd) >> jval;
    acc.get(K_desc) >> jval;

    bool bret = Putter(acc, jkey, jval);
    log_assert(bret);
    return 0;
}

long StoreImpl::GetAccount(account_t &acc)
{
    json1_t jkey;
    acc.get(K_id) >> jkey;

    json1_t jval;
    acc.get(K_passwd) >> jval;
    acc.get(K_desc) >> jval;

    bool bret = Getter(acc, jkey, jval);
    acc.set(jval);
    log_assert(bret);
    return 0;
}

long StoreImpl::PutDB(db_t &db)
{
    json1_t jkey;
    db.get(K_id) >> jkey;

    json1_t jval;
    db.get(K_title) >> jval;
    db.get(K_desc) >> jval;
    db.get(K_logo) >> jval;

    bool bret = Putter(db, jkey, jval);
    log_assert(bret);
    return 0;
}

long StoreImpl::GetDB(db_t &db)
{
    json1_t jkey;
    db.get(K_id) >> jkey;

    json1_t jval;
    db.get(K_title) >> jval;
    db.get(K_desc) >> jval;
    db.get(K_logo) >> jval;

    bool bret = Getter(db, jkey, jval);
    db.set(jval);
    log_assert(bret);
    return 0;
}

long StoreImpl::PutDoc(doc_t &doc)
{
    json1_t jkey;
    doc.get(K_id) >> jkey;

    json1_t jval;
    doc.get(K_title) >> jval;
    doc.get(K_desc) >> jval;
    doc.get(K_logo) >> jval;

    bool bret = Putter(doc, jkey, jval);
    log_assert(bret);
    return 0;
}

long StoreImpl::GetDoc(doc_t &doc)
{
    json1_t jkey;
    doc.get(K_id) >> jkey;

    json1_t jval;
    doc.get(K_title) >> jval;
    doc.get(K_desc) >> jval;
    doc.get(K_logo) >> jval;

    bool bret = Getter(doc, jkey, jval);
    doc.set(jval);
    log_assert(bret);
    return 0;
}

bool StoreImpl::Putter(gen_t &gen, json1_t &jkey, json1_t &jval)
{
    json2_t json;
    json["uri"] = gen.uri();
    json["key"] = jkey;
    json["val"] = jval;
    json["scm"] = gen.schema();
    return process_jx9_json_put((unqlite*)m_pHandle, json);
}

bool StoreImpl::Getter(gen_t &gen, json1_t &jkey, json1_t &jval)
{
    json2_t json;
    json["uri"] = gen.uri();
    json["key"] = jkey;
    json["val"] = jval;
    json["scm"] = gen.schema();
    return process_jx9_json_get((unqlite*)m_pHandle, json, jval);
}
