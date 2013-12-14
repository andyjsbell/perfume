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
    returnv_if_fail(ret != UNQLITE_OK, EAU_E_FAIL);

    return EAU_S_OK;
}

long StoreImpl::Close()
{
    unqlite_close(m_pDB);
    m_pDB = NULL;
}

long StoreImpl::PutAccount(const string &szUser, const string &szPasswd, const string &szDesc)
{
    returnv_if_fail(!szUser.empty(), EAU_E_INVALIDARG);
    returnv_if_fail(!szPasswd.empty(), EAU_E_INVALIDARG);
    returnv_if_fail(!szDesc.empty(), EAU_E_INVALIDARG);

    long lret = EAU_E_FAIL;
    unqlite_vm* jx9_vm = NULL;

    int ret = unqlite_compile_file(m_pDB, kPutAccountScript, &jx9_vm);
    returnv_if_fail(ret != UNQLITE_OK, EAU_E_FAIL);

    do 
    {
        // set $argv[0,1,2] in script
        lret = config_jx9_argv(jx9_vm, szUser.c_str(), szPasswd.c_str(), szDesc.c_str());
        break_if_fail(lret != EAU_S_OK);

        ret = unqlite_vm_exec(jx9_vm);
        break_if_fail(ret != UNQLITE_OK);

        lret = check_jx9_result(jx9_vm);
    }while(false);

    unqlite_vm_release(jx9_vm);

    return lret;
}

long StoreImpl::GetAccout(const string &szUser, string &szPasswd)
{
    returnv_if_fail(!szUser.empty(), EAU_E_INVALIDARG);

    long lret = EAU_E_FAIL;
    unqlite_vm* jx9_vm = NULL;
    unqlite_value* jx9_record = NULL;

    int ret = unqlite_compile_file(m_pDB, kGetAccountScript, &jx9_vm);
    returnv_if_fail(ret != UNQLITE_OK, EAU_E_FAIL);

    do 
    {
        // set $argv[0] in script
        lret = config_jx9_argv(jx9_vm, szUser.c_str());
        break_if_fail(lret != EAU_S_OK);

        ret = unqlite_vm_exec(jx9_vm);
        break_if_fail(ret != UNQLITE_OK);

        // extract value from jx9 script
        jx9_record = unqlite_vm_extract_variable(jx9_vm, "__record__");
        break_if_fail(jx9_record);

        parse_json_string(jx9_record, string("passwd"), szPasswd);
        //parse_json_string(jx9_record, string("desc"), szDesc);

        lret = check_jx9_result(jx9_vm);
    }while(false);

    unqlite_vm_release_value(jx9_vm, jx9_record);
    unqlite_vm_release(jx9_vm);

    return lret;
}

long StoreImpl::PutDB(string &szID, db_t &db)
{
    long lret = EAU_E_FAIL;
    unqlite_vm* jx9_vm = NULL;
    int ret = unqlite_compile_file(m_pDB, kPushDBScript, &jx9_vm);
    returnv_if_fail(ret != UNQLITE_OK, EAU_E_FAIL);

    do
    {
        // set $argv[0,..] in script
        lret = config_jx9_argv(jx9_vm, db.id.c_str(), 
            db.title.c_str(), db.desc.c_str(), db.logo.c_str(), db.status.c_str(), db.date.c_str());
        break_if_fail(lret != EAU_S_OK);

        ret = unqlite_vm_exec(jx9_vm);
        break_if_fail(ret != UNQLITE_OK);

        lret = check_jx9_result(jx9_vm);
    }while(false);

    unqlite_vm_release(jx9_vm);

    return lret;
}

long StoreImpl::GetDB(string &szID, db_t &db)
{
    long lret = EAU_E_FAIL;
    unqlite_vm* jx9_vm = NULL;
    int ret = unqlite_compile_file(m_pDB, kGetDBScript, &jx9_vm);
    returnv_if_fail(ret != UNQLITE_OK, EAU_E_FAIL);

    do
    {
        // set $argv[0,..] in script
        lret = config_jx9_argv(jx9_vm, db.id.c_str()); 
        break_if_fail(lret != EAU_S_OK);

        ret = unqlite_vm_exec(jx9_vm);
        break_if_fail(ret != UNQLITE_OK);

        lret = check_jx9_result(jx9_vm);
    }while(false);

    unqlite_vm_release(jx9_vm);

    return lret;
}


