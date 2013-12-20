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
 * @define: int socket();
 * @define: int socket({type:int});
 * @define: int socket({domain:int, type:int, proto:int});
 * int socket(int domain, int type, int protocol);
 */
int jx9_socket_func(unqlite_context* pCtx, int argc, unqlite_value** argv)
{
    int iret = -1;
    int domain = PF_INET;
    int type = SOCK_STREAM; // SOCK_STREAM or SOCK_DGRAM
    int proto = 0; // not set

    if (argc != 0) {
        CHECK_ARGV_RETURN(pCtx, argc, argv[0]);

        do {
            PARSE_JSON_ELEM(argv[0], "domain", domain, int);
            PARSE_JSON_ELEM(argv[0], "type", type, int);
            PARSE_JSON_ELEM(argv[0], "proto", proto, int);
        }while(0);
    }

    iret = socket(domain, type, proto);
    unqlite_result_int(pCtx, iret);
    return UNQLITE_OK;
}

/**
 * @define: int connect({sock: int, host:string, port:int});
 * int connect(int socket, const struct sockaddr *address, socklen_t address_len);
 */
int jx9_connect_func(unqlite_context* pCtx, int argc, unqlite_value** argv)
{
    int iret = -1;
    int sock = 0;
    const char *host = NULL;
    int port = 0;
    struct sockaddr_in raddr; 

    CHECK_ARGV_RETURN(pCtx, argc, argv[0]);

    do {
        PARSE_JSON_ELEM(argv[0], "sock", sock, int);
        PARSE_JSON_ELEM(argv[0], "host", host, string);
        PARSE_JSON_ELEM(argv[0], "port", port, int);
    }while(0);

    bzero(&raddr, sizeof(raddr));
    raddr.sin_family = AF_INET;  
    raddr.sin_port = htons(port);  
    inet_pton(AF_INET, host, &raddr.sin_addr); 

    iret = connect(sock, (struct sockaddr*)&raddr, sizeof(struct sockaddr));
    unqlite_result_int(pCtx, iret);
    return UNQLITE_OK;
}

/**
 * @define: int send({sock:int, msg:string, size:int, flags:int});
 * ssize_t send(int socket, const void *buffer, size_t length, int flags);
 */
int jx9_send_func(unqlite_context* pCtx, int argc, unqlite_value** argv)
{
    int iret = -1;
    int sock = 0;
    const char *msg = NULL;
    int size = 0;
    int flags = 0;

    CHECK_ARGV_RETURN(pCtx, argc, argv[0]);

    do {
        PARSE_JSON_ELEM(argv[0], "sock", sock, int);
        PARSE_JSON_ELEM(argv[0], "msg", msg, string);
        PARSE_JSON_ELEM(argv[0], "size", size, int);
        PARSE_JSON_ELEM(argv[0], "flags", flags, int);
    }while(0);

    if (size > 0) {
        iret = send(sock, (const void *)msg, size, flags);
    }
    unqlite_result_int(pCtx, iret);
    return UNQLITE_OK;
}

/**
 * @return msg len and @param out {msg:string}
 * @define: int recv({sock:int, size:int, flags:int});
 * ssize_t recv(int socket, void *buffer, size_t length, int flags);
 */
int jx9_recv_func(unqlite_context* pCtx, int argc, unqlite_value** argv)
{
    int iret = -1;
    int sock = 0;
    int size = 0;
    int flags = 0;

    CHECK_ARGV_RETURN(pCtx, argc, argv[0]);

    do {
        PARSE_JSON_ELEM(argv[0], "sock", sock, int);
        PARSE_JSON_ELEM(argv[0], "size", size, int);
        PARSE_JSON_ELEM(argv[0], "flags", flags, int);
    }while(0);

    if (size > 0) {
        void* msg = malloc(size);
        iret = recv(sock, msg, size, flags);
        if (iret > 0) {
            unqlite_value *jx9_msg = unqlite_context_new_scalar(pCtx);
            unqlite_value_string(jx9_msg, (const char *)msg, iret);
            unqlite_array_add_strkey_elem(argv[0], "msg", jx9_msg);
            unqlite_context_release_value(pCtx, jx9_msg);
        }
        free(msg);
    }
    unqlite_result_int(pCtx, iret);
    return UNQLITE_OK;
}

/**
 * @define: int listen({sock:int});
 * @define: int listen({sock:int, backlog:int});
 * int listen(int socket, int backlog);
 */
int jx9_listen_func(unqlite_context* pCtx, int argc, unqlite_value** argv)
{
    int iret = -1;
    int sock = 0; 
    int backlog = 3; 

    CHECK_ARGV_RETURN(pCtx, argc, argv[0]);

    do {
        PARSE_JSON_ELEM(argv[0], "sock", sock, int);
        PARSE_JSON_ELEM(argv[0], "backlog", backlog, int);
    }while(0);

    iret = listen(sock, backlog);
    unqlite_result_int(pCtx, iret);
    return UNQLITE_OK;
}

/**
 * @return fd and @param out {host:string, port:int}
 * @define: int accept({sock:int});
 * int accept(int socket, struct sockaddr *restrict address, socklen_t *restrict address_len);
 */
int jx9_accept_func(unqlite_context* pCtx, int argc, unqlite_value** argv)
{
    int iret = -1;
    int sock = 0; 
    struct sockaddr_in raddr;
    socklen_t addrlen = sizeof(struct sockaddr);

    CHECK_ARGV_RETURN(pCtx, argc, argv[0]);

    do {
        PARSE_JSON_ELEM(argv[0], "sock", sock, int);
    }while(0);

    bzero(&raddr, sizeof(raddr));
    iret = accept(sock, (struct sockaddr*)&raddr, &addrlen);
    if (iret != -1 ){
        int port = 0;
        char host[64] = {0};
        unqlite_value* jx9_port = NULL;
        unqlite_value* jx9_host = NULL;

        inet_ntop(AF_INET, &raddr.sin_addr, host, sizeof(host));
        jx9_host = unqlite_context_new_scalar(pCtx);
        unqlite_value_string(jx9_host, host, strlen(host));
        unqlite_array_add_strkey_elem(argv[0], "host", jx9_host);
        unqlite_context_release_value(pCtx, jx9_host);

        port = ntohs(raddr.sin_port);
        jx9_port = unqlite_context_new_scalar(pCtx);
        unqlite_value_int(jx9_port, port);
        unqlite_array_add_strkey_elem(argv[0], "port", jx9_port);
        unqlite_context_release_value(pCtx, jx9_port);
    }
    unqlite_result_int(pCtx, iret);
    return UNQLITE_OK;
}

/**
 * @return sent the number of bytes
 * @define: int sendto({sock:int, msg:string, size:int, flags:int, host:string, port:int})
 * ssize_t sendto(int socket, const void *buffer, size_t length, int flags,
 *          const struct sockaddr *dest_addr, socklen_t dest_len);
 */
int jx9_sendto_func(unqlite_context* pCtx, int argc, unqlite_value** argv)
{
    int iret = -1;
    int sock = 0; 
    const char* msg = NULL;
    int size = 0;
    int flags = 0;
    const char* host = NULL;
    int port = 0;
    struct sockaddr_in raddr;
    int addrlen = sizeof(struct sockaddr);

    CHECK_ARGV_RETURN(pCtx, argc, argv[0]);

    do {
        PARSE_JSON_ELEM(argv[0], "sock", sock, int);
        PARSE_JSON_ELEM(argv[0], "msg", msg, string);
        PARSE_JSON_ELEM(argv[0], "size", size, int);
        PARSE_JSON_ELEM(argv[0], "flags", flags, int);
        PARSE_JSON_ELEM(argv[0], "host", host, string);
        PARSE_JSON_ELEM(argv[0], "port", port, int);
    }while(0);

    if (size > 0) {
        bzero(&raddr, sizeof(raddr));
        raddr.sin_family = AF_INET;  
        raddr.sin_port = htons(port);  
        inet_pton(AF_INET, host, &raddr.sin_addr); 
        iret = sendto(sock, msg, size, flags, (struct sockaddr*)&raddr, addrlen);
    }
    unqlite_result_int(pCtx, iret);
    return UNQLITE_OK;
}

/**
 * @return the size of recevied bytes and @param out {msg:string}
 * @define: int recvfrom({sock:int, size:int, flags:int, host:string, port:int})
 * ssize_t recvfrom(int socket, void *restrict buffer, size_t length, int flags,
 *          struct sockaddr *restrict address, socklen_t *restrict address_len);
 */
int jx9_recvfrom_func(unqlite_context* pCtx, int argc, unqlite_value** argv)
{
    int iret = -1;
    int sock = 0; 
    int size = 0;
    int flags = 0;
    const char* host = NULL;
    int port = 0;
    struct sockaddr_in raddr;
    socklen_t addrlen = sizeof(struct sockaddr);

    CHECK_ARGV_RETURN(pCtx, argc, argv[0]);

    do {
        PARSE_JSON_ELEM(argv[0], "sock", sock, int);
        PARSE_JSON_ELEM(argv[0], "size", size, int);
        PARSE_JSON_ELEM(argv[0], "flags", flags, int);
        PARSE_JSON_ELEM(argv[0], "host", host, string);
        PARSE_JSON_ELEM(argv[0], "port", port, int);
    }while(0);

    if (size > 0) {
        void* msg = malloc(size);
        bzero(&raddr, sizeof(raddr));
        raddr.sin_family = AF_INET;  
        raddr.sin_port = htons(port);  
        inet_pton(AF_INET, host, &raddr.sin_addr); 
        iret = recvfrom(sock, msg, size, flags, (struct sockaddr*)&raddr, &addrlen);
        if (iret > 0) {
            unqlite_value *jx9_msg = unqlite_context_new_scalar(pCtx);
            unqlite_value_string(jx9_msg, (const char *)msg, iret);
            unqlite_array_add_strkey_elem(argv[0], "msg", jx9_msg);
            unqlite_context_release_value(pCtx, jx9_msg);
        }
        free(msg);
    }
    unqlite_result_int(pCtx, iret);
    return UNQLITE_OK;
}

