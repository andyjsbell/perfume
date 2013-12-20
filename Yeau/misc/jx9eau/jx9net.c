#include "jx9net.h"
#include "jx9socket.c"


static const struct jx9_funcs {
    /* Name of the foreign function*/
    const char *zName; 
    /* Pointer to the C function performing the computation*/
    int (*xProc)(unqlite_context *, int, unqlite_value **); 
}aFuncs[] = {
    {"jsocket",     jx9_socket_func}, 
    {"jconnect",    jx9_connect_func}, 
    {"jsend",       jx9_send_func}, 
    {"jrecv",       jx9_recv_func}, 
};

int register_jx9net_funcs(unqlite_vm* pVM)
{
    for(int k=0; k < (int)sizeof(aFuncs)/sizeof(aFuncs[0]); k++){
        int iret = unqlite_create_function(pVM, aFuncs[k].zName, aFuncs[k].xProc, 0);
        if( iret != UNQLITE_OK )
            return -1;
    }
    return 0;
}

