#include <iostream>
extern "C" {
#include "unqlite/unqlite.h"
#include "jx9net.h"
}

#define println(p)   std::cout<<p<<std::endl;
const char kDBFname[] = ".jx9eau_unqlite.db";

int jx9_stdout_callback(const void* msg, unsigned int len, void* data)
{
    std::string szmsg = "";
    if (msg && len > 0) {
        szmsg.resize(len+1);
        memcpy((void *)szmsg.data(), msg, len);
    }
    println(szmsg);
    return 0;
}

int main(int argc, char* argv[])
{
    unqlite *pDB = NULL;
    unqlite_vm *pVM = NULL;
    char *pProg = NULL;

    if (argc != 2) {
        println("usage: "<<argv[0]<<" .jx9");
        return -1;
    }
    
    if(unqlite_open(&pDB, kDBFname, UNQLITE_OPEN_CREATE) != UNQLITE_OK)
        goto __error;

    pProg = argv[1];
    if(unqlite_compile_file(pDB, pProg, &pVM) != UNQLITE_OK) {
        const char *err_log = NULL;
        int err_len = 0;
        if (unqlite_config(pDB, UNQLITE_CONFIG_JX9_ERR_LOG, &err_log, &err_len) == UNQLITE_OK) {
            println("[compile] "<<err_log);
        }
        goto __error;
    }

    register_jx9net_funcs(pVM);

    if(unqlite_vm_config(pVM, UNQLITE_VM_CONFIG_OUTPUT, jx9_stdout_callback, NULL) != UNQLITE_OK)
        goto __error;

    if(unqlite_vm_exec(pVM) != UNQLITE_OK)
        goto __error;

__error:
    if (pVM)
        unqlite_vm_release(pVM);
    if (pDB)
        unqlite_close(pDB);
    return 0;
}
