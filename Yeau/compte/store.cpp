#include "store.h"

extern "C" {
#include "unqlite.h"
}

using namespace eau;

StoreImpl::StoreImpl() : m_pDB(0)
{
}

StoreImpl::~StoreImpl()
{}


long StoreImpl::Open(string &szName, int iMode)
{
    if (m_pDB) {
        return EAU_E_EXIST;
    }

    int ret = unqlite_open(&m_pDB, szName, iMode);
    returnv_if_fail(ret != UNQLITE_OK, EAU_E_FAIL);

    return EAU_S_OK;
}

long StoreImpl::Close()
{
    unqlite_close(m_pDB);
    m_pDB = NULL;
}

long StoreImpl::PutAccout(string &szUser, string &szPasswd)
{
    int ret = 0;
    char jx9_prog[1024];
    int jx9_size = sizeof(jx9_prog);
    unqlite_vm* jx9_vm = NULL;

    jx9_size = snprintf(jx9_prog, kPutAccountFmt, 
        szUser.c_str(), 
        szPasswd.c_str(), 
        jx9_size);
    returnv_if_fail(jx9_size > 0, EAU_E_FAIL);
    
    ret = unqlite_compile(m_pDB, jx9_prog, jx9_size, &jx9_vm);
    returnv_if_fail(ret != UNQLITE_OK, EAU_E_FAIL);

    ret = unqlite_vm_exec(jx9_vm);
    if (ret != UNQLITE_OK) {
        unqlite_vm_release(jx9_vm);
        return EAU_E_FAIL;
    }

    unqlite_vm_release(jx9_vm);

    return EAU_S_OK;
}

static const char kGetAccountFmt[] = "{user: '%s', passwd:'%s'}";
long StoreImpl::GetAccout(string &szUser, string &szPasswd)
{
    returnv_if_fail(!szUser.empty(), EAU_E_INVALIDARG);

    size_t usersize = 0;
    int ret = unqlite_kv_fetch(m_pDB, "user", -1, NULL, &usersize);
    returnv_if_fail(ret != UNQLITE_OK, EAU_E_FAIL);
    if (usersize != szUser.size()) {
        return EAU_E_FAIL;
    }
        
    string user;
    user.resize(usersize);
    ret = unqlite_kv_fetch(m_pDB, "user", -1, user.data(), &usersize);
    returnv_if_fail(ret != UNQLITE_OK, EAU_E_FAIL);
    if (memcmp(szUser.c_str(), user.c_str(), usersize) != 0) {
        return EAU_E_FAIL;
    }

    ret = unqlite_kv_(m_pDB, "passwd", szPasswd.c_str(), szPasswd.size());
    returnv_if_fail(ret != UNQLITE_OK, EAU_E_FAIL);

}


