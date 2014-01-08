#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "jx9_api.h"
#include "error.h"

extern "C" {
#include "unqlite/unqlite.h"
#include "jx9net.h"
}

namespace eau
{

static int jx9_stdout_callback(const void* msg, unsigned int len, void* data)
{
    if (msg && len > 0) {
        char *pmsg = (char *)malloc(len+1);
        memset(pmsg, 0, len+1);
        memcpy(pmsg, msg, len);
        printf("%s\n", pmsg);
        free(pmsg);
    }
    return 0;
}

int run_jx9_exec(const char *script, const char *fname, 
        jx9_in_cb_t pf_in,
        jx9_out_cb_t pf_out,
        jx9_log_cb_t pf_log)
{
    int iret = -1;
    unqlite *pDB = NULL;
    unqlite_vm *pVM = NULL;

    const char *pProg = script;
    const char *pName = fname;

    jx9_in_cb_t pfInCallback = pf_in;
    jx9_out_cb_t pfOutCallback = pf_out;
    jx9_log_cb_t pfLogCallback = pf_log;

    if (!pProg || !pName) {
        printf("usage: %s jx9_script db_fname\n", __FUNCTION__);
        return -1;
    }

    if (!pfLogCallback) {
        pfLogCallback = jx9_stdout_callback;
    }
    
    if(unqlite_open(&pDB, pName, UNQLITE_OPEN_CREATE) != UNQLITE_OK)
        goto __error;

    if(unqlite_compile_file(pDB, pProg, &pVM) != UNQLITE_OK) {
        const char *err_log = NULL;
        int err_len = 0;
        if (unqlite_config(pDB, UNQLITE_CONFIG_JX9_ERR_LOG, &err_log, &err_len) == UNQLITE_OK) {
            printf("[compile] %s\n", err_log);
        }
        goto __error;
    }

    register_jx9net_funcs(pVM);

    if(unqlite_vm_config(pVM, UNQLITE_VM_CONFIG_OUTPUT, pfLogCallback, NULL) != UNQLITE_OK)
        goto __error;

    // input users' variables
    if (pfInCallback)
        pfInCallback(pVM, NULL);

    if(unqlite_vm_exec(pVM) != UNQLITE_OK)
        goto __error;

    if (pfOutCallback) 
        pfOutCallback(pVM, NULL);

    iret = 0;
__error:
    if (pVM)
        unqlite_vm_release(pVM);
    if (pDB)
        unqlite_close(pDB);
    return iret;
}

bool set_jx9_variable(vmptr_t jx9_vm, const char *jx9_name, const valptr_t jx9_value)
{
    returnb_assert(jx9_vm);
    returnb_assert(jx9_name);
    returnb_assert(jx9_value);
    bool bret = (unqlite_vm_config(jx9_vm, UNQLITE_VM_CONFIG_CREATE_VAR, jx9_name, jx9_value) == UNQLITE_OK);
    return bret;
}

//=======================================

bool new_jx9_json_value(vmptr_t jx9_vm, const string &value, valptr_t &jx9_value)
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
bool add_jx9_json_object(vmptr_t jx9_vm, const pair_ptr_t &key, valptr_t &jx9_json)
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
bool add_jx9_json_object(vmptr_t jx9_vm, const pair_t &key, valptr_t &jx9_json)
{
    returnb_assert(jx9_vm);

    bool bret = false;
    valptr_t pvalue = NULL;
    do {
        break_assert(new_jx9_json_value(jx9_vm, key.second, pvalue));
        const pair_ptr_t key_ptr(key.first, pvalue);
        bret = add_jx9_json_object(jx9_vm, key_ptr, jx9_json);
    }while(false);
    unqlite_vm_release_value(jx9_vm, pvalue);
    return bret;
}

// json format:  {k1:v1, k2:v2, ...}
bool add_jx9_json_object(vmptr_t jx9_vm, const json1_t &keys, valptr_t &jx9_json)
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
bool add_jx9_json_object(vmptr_t jx9_vm, const json2_t &keys, valptr_t &jx9_json)
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

} // namespace eau
