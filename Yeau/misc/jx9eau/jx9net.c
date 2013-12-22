#include "jx9net.h"
#include "jx9socket.c"
#include "jx9neon.c"


static int jx9_testnet_func(unqlite_context* pCtx, int argc, unqlite_value** argv);

struct jx9_funcs {
    /* Name of the foreign function*/
    const char *zName; 
    /* Pointer to the C function performing the computation*/
    int (*xProc)(unqlite_context *, int, unqlite_value **); 
};

static const struct jx9_funcs aSocketFuncs[] = {
    {"socket",     jx9_socket_func}, 
    {"connect",    jx9_connect_func}, 
    {"listen",     jx9_listen_func}, 
    {"bind",       jx9_bind_func}, 
    {"send",       jx9_send_func}, 
    {"recv",       jx9_recv_func}, 
    {"accept",     jx9_accept_func}, 
    {"sendto",     jx9_sendto_func}, 
    {"recvfrom",   jx9_recvfrom_func}, 
    {"shutdown",   jx9_shutdown_func}, 
    {"close",      jx9_close_func}, 
    {"testnet",    jx9_testnet_func} 
};

static const struct jx9_funcs aNeonFuncs[] = {
    {"ne_session_create",           jx9_ne_session_create}, 
    {"ne_close_connection",         jx9_ne_close_connection}, 
    {"ne_session_destroy",          jx9_ne_session_destroy}, 
    {"ne_request_create",           jx9_ne_request_create}, 
    {"ne_request_dispatch",         jx9_ne_request_dispatch}, 
    {"ne_request_destroy",          jx9_ne_request_destroy}, 
    {"ne_add_request_header",       jx9_ne_add_request_header}, 
    {"ne_get_response_header",      jx9_ne_get_response_header}, 
    {"ne_set_useragent",            jx9_ne_set_useragent}, 
    {"ne_set_read_timeout",         jx9_ne_set_read_timeout}, 
    {"ne_set_connect_timeout",      jx9_ne_set_connect_timeout}, 
    {"ne_set_request_body_buffer",  jx9_ne_set_request_body_buffer}, 
    {"ne_set_server_auth",          jx9_ne_set_server_auth}, 
    {"ne_get_status",               jx9_ne_get_status}, 
};

int register_jx9net_funcs(unqlite_vm* pVM)
{
    for(int k=0; k < (int)sizeof(aSocketFuncs)/sizeof(aSocketFuncs[0]); k++){
        int iret = unqlite_create_function(pVM, aSocketFuncs[k].zName, aSocketFuncs[k].xProc, 0);
        if( iret != UNQLITE_OK )
            return -1;
    }

    for(int k=0; k < (int)sizeof(aNeonFuncs)/sizeof(aNeonFuncs[0]); k++){
        int iret = unqlite_create_function(pVM, aNeonFuncs[k].zName, aNeonFuncs[k].xProc, 0);
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
        printf(">");
        gets(msg);
        //iret = snprintf(msg, sizeof(msg), "%s => 12345-67890", idstr);
        iret = strlen(msg);
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

