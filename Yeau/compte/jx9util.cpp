#include "jx9util.h"

namespace eau
{
    int jx9_out_callback(const void* msg, unsigned int len, void* data)
    {
        string sztag = "jx9";
        if (data) {
            sztag = (char *)data;
        }
        sztag = "[" + sztag + "]";

        string szmsg = "";
        if (msg && len > 0) {
            szmsg.resize(len+1);
            memcpy((void *)szmsg.data(), msg, len);
        }
    
        cout << sztag << " jx9 msg: " << szmsg << endl;
        return 0;
    }

    long check_jx9_return(unqlite_vm* jx9_vm)
    {
        returnv_assert(jx9_vm, EAU_E_INVALIDARG);

        unqlite_value* jx9_return = NULL;
        int ret = unqlite_vm_config(jx9_vm, UNQLITE_VM_CONFIG_EXEC_VALUE, &jx9_return);
        returnv_assert2(ret, UNQLITE_OK, EAU_E_FAIL);

        ret = unqlite_value_to_int(jx9_return);
        unqlite_vm_release_value(jx9_vm, jx9_return);

        returnv_assert2(ret, UNQLITE_OK, EAU_E_FAIL);        
        return EAU_S_OK;
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

        returnv_assert2(ret, UNQLITE_OK, EAU_E_FAIL);        
        return EAU_S_OK;
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

        returnv_assert2(ret, UNQLITE_OK, EAU_E_FAIL);        
        return EAU_S_OK;
    }

    long config_jx9_output(unqlite_vm* jx9_vm, jx9_out_cb_t pf_out, void *data)
    {
        returnv_assert(jx9_vm, EAU_E_INVALIDARG);

        int ret = unqlite_vm_config(jx9_vm, UNQLITE_VM_CONFIG_OUTPUT, pf_out, data);
        returnv_assert2(ret, UNQLITE_OK, EAU_E_FAIL);        
        return EAU_S_OK;
    }

    long process_jx9_put(unqlite* jx9_db, const char* jx9_prog, const vector<pair_t> &ivar)
    {
        returnv_assert(jx9_db, EAU_E_INVALIDARG);
        returnv_assert(jx9_prog, EAU_E_INVALIDARG);

        long lret = EAU_E_FAIL;
        unqlite_vm* jx9_vm = NULL;

        int ret = unqlite_compile_file(jx9_db, jx9_prog, &jx9_vm);
        returnv_assert2(ret, UNQLITE_OK, EAU_E_FAIL);
        config_jx9_output(jx9_vm, jx9_out_callback, (void *)jx9_prog);

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
        config_jx9_output(jx9_vm, jx9_out_callback, (void *)jx9_prog);

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

namespace eau
{
    bool new_jx9_json_value(unqlite_vm* jx9_vm, 
        const string &value, 
        unqlite_value* &jx9_value)
    {
        returnb_assert(jx9_vm);

        unqlite_value* pvalue = jx9_value;
        if (pvalue == NULL) {
            returnb_assert((pvalue=unqlite_vm_new_scalar(jx9_vm)));
        }

        bool bret = (unqlite_value_string(pvalue, value.c_str(), value.size()) == UNQLITE_OK);
        if(bret) {
            jx9_value = pvalue;
        }else if (jx9_value == NULL && pvalue != NULL) {
            unqlite_vm_release_value(jx9_vm, pvalue);
        }
        return bret;
    }

    // json format: {k:v}, {..., k:v}
    bool add_jx9_json_object(unqlite_vm* jx9_vm,
        const pair_ptr_t &key, 
        unqlite_value* &jx9_json)
    {
        returnb_assert(jx9_vm);

        unqlite_value* pjson = jx9_json;
        if (pjson == NULL) {
            returnb_assert((pjson=unqlite_vm_new_array(jx9_vm)));
        }

        unqlite_value* pvalue = (unqlite_value*) key.second;
        bool bret = (unqlite_array_add_strkey_elem(pjson, key.first.c_str(), pvalue) == UNQLITE_OK);
        if (bret) {
            jx9_json = pjson;
        }else if (jx9_json == NULL && pjson != NULL) {
            unqlite_vm_release_value(jx9_vm, pjson);
        }
        return bret;
    }

    // json format: {k:v}, {..., k:v}
    bool add_jx9_json_object(unqlite_vm* jx9_vm,
            const pair_t &key, 
            unqlite_value* &jx9_json)
    {
        returnb_assert(jx9_vm);

        bool bret = false;
        unqlite_value* pvalue = NULL;
        do {
            break_assert(new_jx9_json_value(jx9_vm, key.second, pvalue));
            pair_ptr_t key_ptr(key.first, pvalue);
            bret = add_jx9_json_object(jx9_vm, key_ptr, jx9_json);
        }while(false);
        unqlite_vm_release_value(jx9_vm, pvalue);
        return bret;
    }

    // json format:  {k1:v1, k2:v2, ...}
    bool add_jx9_json_object(unqlite_vm* jx9_vm,
            const json1_t &keys, 
            unqlite_value* &jx9_json)
    {
        returnb_assert(jx9_vm);

        bool bret = false;
        unqlite_value* json1 = NULL;
        json1_t::const_iterator iter;
        for (iter=keys.begin(); iter != keys.end(); iter++) {
            break_assert((bret=add_jx9_json_object(jx9_vm, *iter, json1)));
        }

        if (bret) {
            jx9_json = json1;
        }else {
            unqlite_vm_release_value(jx9_vm, json1);
        }
        return bret;
    }

    // json format:  {k1:{k11:v11,k12:v12}, k2:{..}, ...}
    bool add_jx9_json_object(unqlite_vm* jx9_vm, 
            const json2_t &keys, 
            unqlite_value* &jx9_json)
    {
        returnb_assert(jx9_vm);

        bool bret = false;
        unqlite_value* json1 = NULL;
        unqlite_value* json2 = NULL;

        json2_t::const_iterator iter1;
        for (iter1=keys.begin(); iter1 != keys.end(); iter1++) {
            break_assert((bret=add_jx9_json_object(jx9_vm, iter1->second, json1)));

            pair_ptr_t kv_pair(iter1->first, json1);
            break_assert((bret=add_jx9_json_object(jx9_vm, kv_pair, json2)));

            unqlite_vm_release_value(jx9_vm, json1);
            json1 = NULL;
        }
        unqlite_vm_release_value(jx9_vm, json1);

        if (bret) {
            jx9_json = json2;
        }else {
            unqlite_vm_release_value(jx9_vm, json2);
        }
        return bret;
    }

    bool set_jx9_json_variable(unqlite_vm* jx9_vm, const char *jx9_var_name, const unqlite_value *jx9_var)
    {
        returnb_assert(jx9_vm);
        returnb_assert(jx9_var_name);
        returnb_assert(jx9_var);
        bool bret = (unqlite_vm_config(jx9_vm, UNQLITE_VM_CONFIG_CREATE_VAR, jx9_var_name, jx9_var) == UNQLITE_OK);
        return bret;
    }

    bool set_jx9_json_output(unqlite_vm* jx9_vm, jx9_out_cb_t pf_out, void *data)
    {
        returnb_assert(jx9_vm);
        bool bret = (unqlite_vm_config(jx9_vm, UNQLITE_VM_CONFIG_OUTPUT, pf_out, data) == UNQLITE_OK);
        return bret;
    }

    bool parse_jx9_json_return(unqlite_vm* jx9_vm, int &value)
    {
        returnb_assert(jx9_vm);
        
        bool bret = false;
        unqlite_value* jx9_return = NULL;
        int ret = unqlite_vm_config(jx9_vm, UNQLITE_VM_CONFIG_EXEC_VALUE, &jx9_return);
        returnb_assert2(ret, UNQLITE_OK);
        bret = (parse_jx9_value(jx9_return, "ret", value) == EAU_S_OK);
        return bret;
    }

    bool process_jx9_json(unqlite* jx9_db, const char* jx9_prog, const json2_t &ivar, json1_t &ovar) 
    {
        returnb_assert(jx9_db);
        returnb_assert(jx9_prog);

        bool bret = false;
        unqlite_vm* jx9_vm = NULL;

        returnb_assert(unqlite_compile_file(jx9_db, jx9_prog, &jx9_vm) == UNQLITE_OK);
        bret = set_jx9_json_output(jx9_vm, jx9_out_callback, (void *)jx9_prog);

        unqlite_value* jx9_json = NULL;
        unqlite_value* jx9_arg = NULL;
        do {
            if (!ivar.empty()) {
                break_assert(bret=add_jx9_json_object(jx9_vm, ivar, jx9_json));
                break_assert(bret=set_jx9_json_variable(jx9_vm, kEauJx9Arg, jx9_json));
            }

            break_assert2(unqlite_vm_exec(jx9_vm), UNQLITE_OK);
            
            int jx9_ret = 0xff;
            break_assert(parse_jx9_json_return(jx9_vm, jx9_ret));
            break_assert2(jx9_ret, 0);

            // extract value from jx9 script
            if(!ovar.empty()) {
                jx9_arg = unqlite_vm_extract_variable(jx9_vm, kEauJx9Out);
                json1_t::iterator iter;
                for (iter=ovar.begin(); iter != ovar.end(); iter++) {
                    parse_jx9_value(jx9_arg, iter->first, iter->second);
                }
            }
            bret = true;
        }while(false);

        unqlite_vm_release_value(jx9_vm, jx9_json);
        unqlite_vm_release_value(jx9_vm, jx9_arg);
        return bret;
    }

    bool process_jx9_json_get(unqlite* jx9_db, const json2_t &ivar, json1_t &ovar)
    {
        return process_jx9_json(jx9_db, kEauJx9Getter, ivar, ovar);
    }

    bool process_jx9_json_put(unqlite* jx9_db, const json2_t &ivar) 
    {
        json1_t ovar;
        return process_jx9_json(jx9_db, kEauJx9Putter, ivar, ovar);
    }

} // namespace eau

