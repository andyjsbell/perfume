#ifndef _EAU_JX9UTIL_H_
#define _EAU_JX9UTIL_H_

#include <stdarg.h>

#include <map>
#include <string>
using namespace std;

extern "C" {
#include "unqlite.h"
}

#include "error.h"

namespace eau
{
    static const char kEauRecordInVar[] = "eau_record_in";
    static const char kEauRecordOutVar[] = "eau_record_out";

    static const char kPutAccountScript[] = "scripts/putaccount.jx9";
    static const char kGetAccountScript[] = "scripts/getaccount.jx9";

    static const char kPutDBScript[] = "scripts/putdb.jx9";
    static const char kGetDBScript[] = "scripts/getdb.jx9";

    static const char kPutDocScript[] = "scripts/putdoc.jx9";
    static const char kGetDocScript[] = "scripts/getdoc.jx9";

    static long check_jx9_return(unqlite_vm* jx9_vm)
    {
        returnv_if_fail(jx9_vm, EAU_E_INVALIDARG);

        unqlite_value* jx9_return = NULL;
        int ret = unqlite_vm_config(jx9_vm, UNQLITE_VM_CONFIG_EXEC_VALUE, &jx9_return);
        returnv_if_fail(ret == UNQLITE_OK, EAU_E_FAIL);

        ret = unqlite_value_to_int(jx9_return);
        unqlite_vm_release_value(jx9_vm, jx9_return);

        return (ret == 0) ? EAU_S_OK : EAU_E_FAIL;
    }

    static long parse_jx9_value(unqlite_value* jx9_array, const string &key, int &value)
    {
        returnv_if_fail(jx9_array, EAU_E_INVALIDARG);
        returnv_if_fail(!key.empty(), EAU_E_INVALIDARG);

        unqlite_value* jx9_elem = unqlite_array_fetch(jx9_array, key.c_str(), key.size());
        returnv_if_fail(jx9_elem, EAU_E_FAIL);
        value = unqlite_value_to_int(jx9_elem);

        return EAU_S_OK;
    }

    static long parse_jx9_value(unqlite_value* jx9_array, const string &key, string &value)
    {
        returnv_if_fail(jx9_array, EAU_E_INVALIDARG);
        returnv_if_fail(!key.empty(), EAU_E_INVALIDARG);

        unqlite_value* jx9_elem = unqlite_array_fetch(jx9_array, key.c_str(), key.size());
        returnv_if_fail(jx9_elem, EAU_E_FAIL);
        value = (string)unqlite_value_to_string(jx9_elem, NULL);

        return EAU_S_OK;
    }

    static long config_jx9_argv(unqlite_vm* jx9_vm, char *fmt, ...)
    {
        returnv_if_fail(jx9_vm, EAU_E_INVALIDARG);

        va_list ap;
        char *str = NULL;
        int ret = UNQLITE_OK;

        va_start(ap, fmt);
        while(*fmt) 
        {
            switch(*fmt++){
            case 's':
                str = va_arg(ap, char *);
                ret = unqlite_vm_config(jx9_vm, UNQLITE_VM_CONFIG_ARGV_ENTRY, str);;
                break;
            }
            if (ret != UNQLITE_OK) {
                break;
            }
        }
        va_end(ap);

        return (ret == UNQLITE_OK) ? EAU_S_OK : EAU_E_FAIL;
    }
    
    static long config_jx9_variable(unqlite_vm* jx9_vm, map<string, string> &kv_map)
    {
        returnv_if_fail(jx9_vm, EAU_E_INVALIDARG);
        returnv_if_fail(!kv_map.empty(), EAU_E_INVALIDARG);

        unqlite_value* jx9_array = unqlite_vm_new_array(jx9_vm);
        returnv_if_fail(jx9_array, EAU_E_FAIL);
        
        int ret = UNQLITE_OK;
        map<string, string>::iterator iter = kv_map.begin();
        for(iter=kv_map.begin(); iter != kv_map.end(); iter++) {
            unqlite_value* jx9_val = unqlite_vm_new_scalar(jx9_vm);
            ret = unqlite_value_string(jx9_val, iter->second.c_str(), iter->second.size());
            break_if_fail(ret == UNQLITE_OK);
            ret = unqlite_array_add_strkey_elem(jx9_array, iter->first.c_str(), jx9_val);
            unqlite_vm_release_value(jx9_vm, jx9_val);
            break_if_fail(ret == UNQLITE_OK);
        }

        returnv_if_fail(ret == UNQLITE_OK, EAU_E_FAIL);
        ret = unqlite_vm_config(jx9_vm, UNQLITE_VM_CONFIG_CREATE_VAR, kEauRecordInVar, jx9_array);
        unqlite_vm_release_value(jx9_vm, jx9_array);

        return (ret == UNQLITE_OK) ? EAU_S_OK : EAU_E_FAIL;
    }

    static long process_jx9_put(unqlite* jx9_db, const char* jx9_prog, map<string, string> &in_map)
    {
        returnv_if_fail(jx9_db, EAU_E_INVALIDARG);
        returnv_if_fail(jx9_prog, EAU_E_INVALIDARG);

        long lret = EAU_E_FAIL;
        unqlite_vm* jx9_vm = NULL;

        int ret = unqlite_compile_file(jx9_db, jx9_prog, &jx9_vm);
        returnv_if_fail(ret == UNQLITE_OK, EAU_E_FAIL);

        do {
            break_if_fail(config_jx9_variable(jx9_vm, in_map) == EAU_S_OK);
            break_if_fail(unqlite_vm_exec(jx9_vm) == UNQLITE_OK);
            break_if_fail(check_jx9_return(jx9_vm) == EAU_S_OK);

            lret = EAU_S_OK;
        }while(false);

        unqlite_vm_release(jx9_vm);
        return lret;
    }

    static long process_jx9_get(unqlite* jx9_db, const char* jx9_prog, map<string, string> &in_map, map<string, string> &io_map)
    {
        returnv_if_fail(jx9_db, EAU_E_INVALIDARG);
        returnv_if_fail(jx9_prog, EAU_E_INVALIDARG);

        long lret = EAU_E_FAIL;
        unqlite_vm* jx9_vm = NULL;
        unqlite_value* jx9_record = NULL;

        int ret = unqlite_compile_file(jx9_db, jx9_prog, &jx9_vm);
        returnv_if_fail(ret == UNQLITE_OK, EAU_E_FAIL);

        do {
            break_if_fail(config_jx9_variable(jx9_vm, in_map) == EAU_S_OK);
            break_if_fail(unqlite_vm_exec(jx9_vm) == UNQLITE_OK);
            break_if_fail(check_jx9_return(jx9_vm) == EAU_S_OK);

            // extract value from jx9 script
            jx9_record = unqlite_vm_extract_variable(jx9_vm, kEauRecordOutVar);
            break_if_fail(jx9_record);

            map<string, string>::iterator iter;
            for (iter=io_map.begin(); iter != io_map.end(); iter++) {
                break_if_fail(parse_jx9_value(jx9_record, iter->first, iter->second) == EAU_S_OK);
            }
            lret = EAU_S_OK;
        }while(false);

        unqlite_vm_release_value(jx9_vm, jx9_record);
        unqlite_vm_release(jx9_vm);
        return lret;
    }

} // namespace eau

#endif
