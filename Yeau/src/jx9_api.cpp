#include <string.h>
extern "C" {
#include "unqlite/unqlite.h"
#include "jx9net.h"
}

static int jx9_stdout_callback(const void* msg, unsigned int len, void* data)
{
    if (msg && len > 0) {
        char *pmsg = malloc(len+1);
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

    char *pProg = script;
    char *pName = fname;

    jx9_in_cb_t pfInCallback = pf_in;
    jx9_out_cb_t pfOutCallback = pf_out;
    jx9_out_cb_t pfLogCallback = pf_log;

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
        pfInCallback((void *)pVM, NULL);

    if(unqlite_vm_exec(pVM) != UNQLITE_OK)
        goto __error;

    if (pfOutCallback) 
        pfOutCallback((void *)pVM, NULL);

    iret = 0;
__error:
    if (pVM)
        unqlite_vm_release(pVM);
    if (pDB)
        unqlite_close(pDB);
    return iret;
}

