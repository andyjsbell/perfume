#include "jx9net.h"
#include "jx9socket.c"


static int jx9_testnet_func(unqlite_context* pCtx, int argc, unqlite_value** argv);

static const struct jx9_funcs {
    /* Name of the foreign function*/
    const char *zName; 
    /* Pointer to the C function performing the computation*/
    int (*xProc)(unqlite_context *, int, unqlite_value **); 
}aFuncs[] = {
    {"socket",     jx9_socket_func}, 
    {"connect",    jx9_connect_func}, 
    {"send",       jx9_send_func}, 
    {"recv",       jx9_recv_func}, 
    {"listen",     jx9_listen_func}, 
    {"accept",     jx9_accept_func}, 
    {"sendto",     jx9_sendto_func}, 
    {"recvfrom",   jx9_recvfrom_func}, 
    {"testnet",    jx9_testnet_func} 
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

/**
 * @return number of msg bytes and @param out {msg:string}
 * int testnet({id:string})
 */
int jx9_testnet_func(unqlite_context* pCtx, int argc, unqlite_value** argv)
{
    int iret = -1;
    const char* idstr = NULL;
    int idlen = 0;

    if (argc != 1 || !unqlite_value_is_json_object(argv[0])) { 
        unqlite_context_throw_error(pCtx, UNQLITE_CTX_WARNING, "wrong argc or argv[0]");
        unqlite_result_int(pCtx, -2);
        return UNQLITE_OK; 
    }

    do {
        unqlite_value* kvpair = 0;
        kvpair = unqlite_array_fetch(argv[0], "id", strlen("id"));
        if (kvpair && unqlite_value_is_string(kvpair)) {
            idstr = unqlite_value_to_string(kvpair);
        }
    }while(0);

    if (idstr) {
        char msg[128] = "";
        bzero(msg, sizeof(msg));
        iret = snprintf(msg, sizeof(msg), "%s => 12345-67890", idstr);
        if (iret > 0) {
            unqlite_value *jx9_msg = unqlite_context_new_scalar(pCtx);
            unqlite_value_string(jx9_msg, (const char *)msg, iret);
            unqlite_array_add_strkey_elem(argv[0], "msg", jx9_msg);
            unqlite_context_release_value(pCtx, jx9_msg);
        }
    }
    unqlite_result_int(pCtx, iret);
    return UNQLITE_OK;
}

