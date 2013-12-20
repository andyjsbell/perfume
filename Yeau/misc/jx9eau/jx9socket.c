#include "unqlite.h"
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#define CHECK_ARGV_RETURN(ctx, argc, argv) { \
    if ((argc) != 1 || !unqlite_value_is_json_object(argv)) { \
        unqlite_context_throw_error((ctx), UNQLITE_CTX_WARNING, "wrong argc or argv[0]"); \
        unqlite_result_int((ctx), -1); \
        return UNQLITE_OK; \
    }}

#define unqlite_value_to_string(p) unqlite_value_to_string((p), 0);
#define PARSE_JSON_ELEM(json, key, value, type) { \
    unqlite_value* kvpair = 0; \
    kvpair = unqlite_array_fetch((json), (key), strlen(key)); \
    if (kvpair && unqlite_value_is_##type(kvpair)) { \
        value = unqlite_value_to_##type(kvpair); \
    }}

/**
 * int socket({type:int});
 * int socket({domain:int, type:int, proto:int});
 * int socket(int domain, int type, int protocol);
 */
int jx9_socket_func(unqlite_context* pCtx, int argc, unqlite_value** argv)
{
    int iret = -1;
    int domain = PF_INET;
    int type = 0; // SOCK_STREAM or SOCK_DGRAM
    int proto = 0; // not set

    CHECK_ARGV_RETURN(pCtx, argc, argv[0]);

    do {
        PARSE_JSON_ELEM(argv[0], "domain", domain, int);
        PARSE_JSON_ELEM(argv[0], "type", type, int);
        PARSE_JSON_ELEM(argv[0], "proto", proto, int);
    }while(0);

    iret = socket(domain, type, proto);
    unqlite_result_int(pCtx, iret);
    return UNQLITE_OK;
}

/**
 * int connect({sock: int, addr:string, port:int});
 * int connect(int socket, const struct sockaddr *address, socklen_t address_len);
 */
int jx9_connect_func(unqlite_context* pCtx, int argc, unqlite_value** argv)
{
    int iret = -1;
    int sock = 0;
    const char *addr = NULL;
    int port = 0;
    struct sockaddr_in raddr; 

    CHECK_ARGV_RETURN(pCtx, argc, argv[0]);

    do {
        PARSE_JSON_ELEM(argv[0], "sock", sock, int);
        PARSE_JSON_ELEM(argv[0], "addr", addr, string);
        PARSE_JSON_ELEM(argv[0], "port", port, int);
    }while(0);

    bzero(&raddr, sizeof(raddr));
    raddr.sin_family = AF_INET;  
    raddr.sin_port = htons(port);  
    inet_pton(AF_INET, addr, &raddr.sin_addr); 

    iret = connect(sock, (struct sockaddr*)&raddr, sizeof(struct sockaddr));
    unqlite_result_int(pCtx, iret);
    return UNQLITE_OK;
}

/**
 * int send({sock:int, msg:string, size:int});
 * ssize_t send(int socket, const void *buffer, size_t length, int flags);
 */
int jx9_send_func(unqlite_context* pCtx, int argc, unqlite_value** argv)
{
    int iret = -1;
    int sock = 0;
    const char *msg = NULL;
    int size = 0;

    CHECK_ARGV_RETURN(pCtx, argc, argv[0]);

    do {
        PARSE_JSON_ELEM(argv[0], "sock", sock, int);
        PARSE_JSON_ELEM(argv[0], "msg", msg, string);
        PARSE_JSON_ELEM(argv[0], "size", size, int);
    }while(0);

    iret = send(sock, (const void *)msg, size, 0);
    unqlite_result_int(pCtx, iret);
    return UNQLITE_OK;
}

/**
 * @return msg's length and {msg:string} merged into {sock:int, size:int}
 * int recv({sock:int, size:int});
 * ssize_t recv(int socket, void *buffer, size_t length, int flags);
 */
int jx9_recv_func(unqlite_context* pCtx, int argc, unqlite_value** argv)
{
    int iret = -1;
    int sock = 0;
    int size = 0;
    char *msg = NULL;

    CHECK_ARGV_RETURN(pCtx, argc, argv[0]);

    do {
        PARSE_JSON_ELEM(argv[0], "sock", sock, int);
        PARSE_JSON_ELEM(argv[0], "size", size, int);
    }while(0);

    if (size > 0) {
        msg = (char *)malloc(size);
        iret = recv(sock, (void *)msg, size, 0);
        if (iret > 0) {
            unqlite_value *jx9_msg = unqlite_context_new_scalar(pCtx);
            unqlite_value_string(jx9_msg, msg, iret);
            unqlite_array_add_strkey_elem(argv[0], "msg", jx9_msg);
            unqlite_context_release_value(pCtx, jx9_msg);
        }
        free(msg);
    }
    unqlite_result_int(pCtx, iret);
    return UNQLITE_OK;
}

