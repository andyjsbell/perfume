#include "store.h"

extern "C" {
#include "unqlite.h"
}

using namespace eau;

namespace eau
{
    static const char kGetAccountScript[] = "scripts/getaccount.jx9";
    static const char kPutAccountScript[] = "scripts/putaccount.jx9";

    static long check_jx9_result(unqlite_vm* jx9_vm)
    {
        returnv_if_fail(jx9_vm, EAU_E_FAIL);

        unqlite_value* jx9_result = unqlite_vm_extract_variable(jx9_vm, "__result__");
        returnv_if_fail(jx9_result, EAU_E_FAIL);

        int result = unqlite_value_to_int(jx9_result, NULL);
        long lret = (result == 0) ? EAU_S_OK : EAU_E_FAIL;
        unqlite_vm_release_value(jx9_vm, jx9_result);

        return lret;
    }
}

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

long StoreImpl::PutAccout(const string &szUser, const string &szPasswd, const string &szDesc)
{
    returnv_if_fail(!szUser.empty(), EAU_E_INVALIDARG);
    returnv_if_fail(!szPasswd.empty(), EAU_E_INVALIDARG);
    returnv_if_fail(!szDesc.empty(), EAU_E_INVALIDARG);

    unqlite_vm* jx9_vm = NULL;
    int ret = unqlite_compile_file(m_pDB, kPutAccountScript, &jx9_vm);
    returnv_if_fail(ret != UNQLITE_OK, EAU_E_FAIL);

    long lret = EAU_E_FAIL;

    do 
    {
        // need three $argv[0,1,2] in script
        ret = unqlite_vm_config(jx9_vm, UNQLITE_VM_CONFIG_ARGV_ENTRY, szUser.c_str());
        break_if_fail(ret != UNQLITE_OK);
        ret = unqlite_vm_config(jx9_vm, UNQLITE_VM_CONFIG_ARGV_ENTRY, szPasswd.c_str());
        break_if_fail(ret != UNQLITE_OK);
        ret = unqlite_vm_config(jx9_vm, UNQLITE_VM_CONFIG_ARGV_ENTRY, szDesc.c_str());
        break_if_fail(ret != UNQLITE_OK);

        ret = unqlite_vm_exec(jx9_vm);
        break_if_fail(ret != UNQLITE_OK);

        lret = check_jx9_result(jx9_vm);
    }while(false);

    unqlite_vm_release(jx9_vm);

    return lret;
}

//
// The account structure
// { account: [
//      {user: 'string', passwd: 'string', desc: 'string', id: 'uuid'}, 
//      ...
//      ]
// }
long StoreImpl::GetAccout(const string &szUser, string &szPasswd)
{
    returnv_if_fail(!szUser.empty(), EAU_E_INVALIDARG);

    unqlite_vm* jx9_vm = NULL;
    int ret = unqlite_compile_file(m_pDB, kGetAccountScript, &jx9_vm);
    returnv_if_fail(ret != UNQLITE_OK, EAU_E_FAIL);

    long lret = EAU_E_FAIL;
    unqlite_value* jx9_passwd = NULL;
    unqlite_value* jx9_desc = NULL;

    do 
    {
        // need one $argv[0] in script
        ret = unqlite_vm_config(jx9_vm, UNQLITE_VM_CONFIG_ARGV_ENTRY, szUser.c_str());
        break_if_fail(ret != UNQLITE_OK);

        ret = unqlite_vm_exec(jx9_vm);
        break_if_fail(ret != UNQLITE_OK);

        // extract value from jx9 script
        jx9_passwd = unqlite_vm_extract_variable(jx9_vm, "passwd");
        break_if_fail(jx9_passwd);
        szPasswd = (string)unqlite_value_to_string(jx9_passwd, NULL);

        jx9_desc = unqlite_vm_extract_variable(jx9_vm, "desc");
        break_if_fail(jx9_desc);
        string szDesc = (string)unqlite_value_to_string(jx9_desc, NULL);

        lret = check_jx9_result(jx9_vm);
    }while(false);

    unqlite_vm_release_value(jx9_vm, jx9_passwd);
    unqlite_vm_release_value(jx9_vm, jx9_desc);
    unqlite_vm_release(jx9_vm);

    return lret;
}

//
// The database structure
// { database : [
//      {id: 'uuid', title: 'string', desc: 'string', logo: 'string', documents: [uuid, ...]},
//      {id: 'uuid', title: 'string', desc: 'string', logo: 'string', documents: [uuid, ...]},
//      ...
//      ]
// }
//
// { database/documents : [
//      {id: 'uuid', title: 'string', desc: 'string', attachments:[uuid,...]},
//      {id: 'uuid', title: 'string', desc: 'string', attachments:[uuid,...]},
//      ...
//      ]
// }
//
// { document/attachments : [
//      {id: 'uuid', title: 'string', desc: 'string', name: 'string', mime: 'string'},
//      {id: 'uuid', title: 'string', desc: 'string', name: 'string', mime: 'string'},
//      ...
//      ]
// }
//  
 
long StoreImpl::PushDoc()
{}

long StoreImpl::GetDoc()
{}

