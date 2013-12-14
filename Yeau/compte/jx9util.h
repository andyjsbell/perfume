#ifndef _EAU_JX9UTIL_H_
#define _EAU_JX9UTIL_H_

extern "C" {
#include "unqlite.h"
}

namespace eau
{
    static const char kGetAccountScript[] = "scripts/getaccount.jx9";
    static const char kPutAccountScript[] = "scripts/putaccount.jx9";
    static const char kGetDBScript[] = "scripts/getdb.jx9";
    static const char kPutDBScript[] = "scripts/putdb.jx9";

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

    static long parse_json_string(unqlite_value* jx9_array, const string &key, string &value)
    {
        unqlite_value* jx9_elem = unqlite_array_fetch(jx9_array, key.c_str(), key.size());
        returnv_if_fail(jx9_elem, EAU_E_FAIL);
        value = (string)unqlite_value_to_string(jx9_elem, NULL);
        unqlite_vm_release_value(jx9_vm, jx9_elem);
        return EAU_S_OK;
    }

    static long config_jx9_argv(unqlite_vm* jx9_vm, const char *fmt, ...)
    {
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

        long lret = (ret == UNQLITE_OK) ? EAU_S_OK : EAU_E_FAIL;
        return lret;
    }
}

#endif
