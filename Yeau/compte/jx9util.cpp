#include "jx9util.h"

namespace eau
{
    long check_jx9_return(unqlite_vm* jx9_vm)
    {
        returnv_assert(jx9_vm, EAU_E_INVALIDARG);

        unqlite_value* jx9_return = NULL;
        int ret = unqlite_vm_config(jx9_vm, UNQLITE_VM_CONFIG_EXEC_VALUE, &jx9_return);
        returnv_assert2(ret, UNQLITE_OK, EAU_E_FAIL);

        ret = unqlite_value_to_int(jx9_return);
        unqlite_vm_release_value(jx9_vm, jx9_return);
        log_assert2(ret, UNQLITE_OK);

        return (ret == UNQLITE_OK) ? EAU_S_OK : EAU_E_FAIL;
    }

    long parse_jx9_value(unqlite_value* jx9_array, const string &key, int &value)
    {
        returnv_assert(jx9_array, EAU_E_INVALIDARG);
        returnv_assert(!key.empty(), EAU_E_INVALIDARG);

        unqlite_value* jx9_elem = unqlite_array_fetch(jx9_array, key.c_str(), key.size());
        returnv_assert(jx9_elem, EAU_E_FAIL);
        value = unqlite_value_to_int(jx9_elem);

        return EAU_S_OK;
    }

    long parse_jx9_value(unqlite_value* jx9_array, const string &key, string &value)
    {
        returnv_assert(jx9_array, EAU_E_INVALIDARG);
        returnv_assert(!key.empty(), EAU_E_INVALIDARG);

        unqlite_value* jx9_elem = unqlite_array_fetch(jx9_array, key.c_str(), key.size());
        returnv_assert(jx9_elem, EAU_E_FAIL);
        value = (string)unqlite_value_to_string(jx9_elem, NULL);

        return EAU_S_OK;
    }

    long config_jx9_argv(unqlite_vm* jx9_vm, char *fmt, ...)
    {
        returnv_assert(jx9_vm, EAU_E_INVALIDARG);

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
    
    long config_jx9_variable(unqlite_vm* jx9_vm, const vector<pair_t> &ivar)
    {
        returnv_assert(jx9_vm, EAU_E_INVALIDARG);
        returnv_assert(!ivar.empty(), EAU_E_INVALIDARG);

        unqlite_value* jx9_array = unqlite_vm_new_array(jx9_vm);
        returnv_assert(jx9_array, EAU_E_FAIL);
        
        int ret = UNQLITE_OK;
        vector<pair_t>::const_iterator iter;
        for(iter=ivar.begin(); iter != ivar.end(); iter++) {
            unqlite_value* jx9_val = unqlite_vm_new_scalar(jx9_vm);
            ret = unqlite_value_string(jx9_val, iter->second.c_str(), iter->second.size());
            break_assert2(ret, UNQLITE_OK);
            ret = unqlite_array_add_strkey_elem(jx9_array, iter->first.c_str(), jx9_val);
            unqlite_vm_release_value(jx9_vm, jx9_val);
            break_assert2(ret, UNQLITE_OK);
        }

        returnv_assert2(ret, UNQLITE_OK, EAU_E_FAIL);
        ret = unqlite_vm_config(jx9_vm, UNQLITE_VM_CONFIG_CREATE_VAR, kEauRecordInVar, jx9_array);
        unqlite_vm_release_value(jx9_vm, jx9_array);

        return (ret == UNQLITE_OK) ? EAU_S_OK : EAU_E_FAIL;
    }

    long process_jx9_put(unqlite* jx9_db, const char* jx9_prog, const vector<pair_t> &ivar)
    {
        returnv_assert(jx9_db, EAU_E_INVALIDARG);
        returnv_assert(jx9_prog, EAU_E_INVALIDARG);

        long lret = EAU_E_FAIL;
        unqlite_vm* jx9_vm = NULL;

        int ret = unqlite_compile_file(jx9_db, jx9_prog, &jx9_vm);
        returnv_assert2(ret, UNQLITE_OK, EAU_E_FAIL);

        do {
            break_assert2(config_jx9_variable(jx9_vm, ivar), EAU_S_OK);
            break_assert2(unqlite_vm_exec(jx9_vm), UNQLITE_OK);
            break_assert2(check_jx9_return(jx9_vm), EAU_S_OK);
            lret = EAU_S_OK;
        }while(false);
        unqlite_vm_release(jx9_vm);
        log_assert2(lret, EAU_S_OK);

        return lret;
    }

    long process_jx9_get(unqlite* jx9_db, const char* jx9_prog, const vector<pair_t> &ivar, vector<pair_t> &ovar)
    {
        returnv_assert(jx9_db, EAU_E_INVALIDARG);
        returnv_assert(jx9_prog, EAU_E_INVALIDARG);

        long lret = EAU_E_FAIL;
        unqlite_vm* jx9_vm = NULL;
        unqlite_value* jx9_record = NULL;

        int ret = unqlite_compile_file(jx9_db, jx9_prog, &jx9_vm);
        returnv_assert2(ret, UNQLITE_OK, EAU_E_FAIL);

        do {
            break_assert2(config_jx9_variable(jx9_vm, ivar), EAU_S_OK);
            break_assert2(unqlite_vm_exec(jx9_vm), UNQLITE_OK);
            break_assert2(check_jx9_return(jx9_vm), EAU_S_OK);

            // extract value from jx9 script
            jx9_record = unqlite_vm_extract_variable(jx9_vm, kEauRecordOutVar);
            break_assert(jx9_record);

            vector<pair_t>::iterator iter;
            for (iter=ovar.begin(); iter != ovar.end(); iter++) {
                break_assert2(parse_jx9_value(jx9_record, iter->first, iter->second), EAU_S_OK);
            }
            lret = EAU_S_OK;
        }while(false);

        unqlite_vm_release_value(jx9_vm, jx9_record);
        unqlite_vm_release(jx9_vm);
        return lret;
    }

} // namespace eau

