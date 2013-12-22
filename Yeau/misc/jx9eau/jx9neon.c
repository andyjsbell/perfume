#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "unqlite/unqlite.h"

#include "ne_session.h"
#include "ne_request.h"
#include "ne_auth.h"

#ifndef CHECK_ARGV_RETURN
#define CHECK_ARGV_RETURN(ctx, argc, argv) { \
    if ((argc) != 1 || !unqlite_value_is_json_object(argv)) { \
        unqlite_context_throw_error((ctx), UNQLITE_CTX_WARNING, "wrong argc or argv[0]"); \
        unqlite_result_int64((ctx), -1); \
        return UNQLITE_OK; \
    }}
#endif

#ifndef PARSE_JSON_ELEM
#define unqlite_value_to_string(p) unqlite_value_to_string((p), 0);
#define unqlite_value_is_int64  unqlite_value_is_int
#define PARSE_JSON_ELEM(json, key, value, type) { \
    unqlite_value* kvpair = 0; \
    kvpair = unqlite_array_fetch((json), (key), strlen(key)); \
    if (kvpair && unqlite_value_is_##type(kvpair)) { \
        value = unqlite_value_to_##type(kvpair); \
    }}
#endif

/**
 * ne_session* ne_session_create(const char* scheme, const char* hostname, int port);
 */
int jx9_ne_session_create(unqlite_context* pCtx, int argc, unqlite_value** argv)
{
    long iret = -1;
    const char* scheme = NULL;
    const char* hostname = NULL;
    int port = 0;

    CHECK_ARGV_RETURN(pCtx, argc, argv[0]);

    do {
        PARSE_JSON_ELEM(argv[0], "scheme", scheme, string);
        PARSE_JSON_ELEM(argv[0], "hostname", hostname, string);
        PARSE_JSON_ELEM(argv[0], "port", port, int);
    }while(0);

    if (scheme && hostname && port > 0) {
        ne_session* session = ne_session_create(scheme, hostname, port);
        iret = (long)session;
    }
    unqlite_result_int64(pCtx, iret);
    return UNQLITE_OK;
}
int jx9_ne_close_connection(unqlite_context* pCtx, int argc, unqlite_value** argv)
{
    long iret = -1;
    long session = 0;

    CHECK_ARGV_RETURN(pCtx, argc, argv[0]);

    do {
        PARSE_JSON_ELEM(argv[0], "session", session, int64);
    }while(0);

    if (session) {
        ne_close_connection((ne_session *)session);
        iret = 0;
    }
    unqlite_result_int64(pCtx, iret);
    return UNQLITE_OK;
}
int jx9_ne_session_destroy(unqlite_context* pCtx, int argc, unqlite_value** argv)
{
    long iret = -1;
    long session = 0;

    CHECK_ARGV_RETURN(pCtx, argc, argv[0]);

    do {
        PARSE_JSON_ELEM(argv[0], "session", session, int64);
    }while(0);

    if (session) {
        ne_session_destroy((ne_session *)session);
        iret = 0;
    }
    unqlite_result_int64(pCtx, iret);
    return UNQLITE_OK;
}

/**
 * ne_request *ne_request_create(ne_session *session, const char *method, const char *path);
 */
int jx9_ne_request_create(unqlite_context* pCtx, int argc, unqlite_value** argv)
{
    long iret = -1;
    long session = 0;
    const char* method = NULL;
    const char* path = NULL;

    CHECK_ARGV_RETURN(pCtx, argc, argv[0]);

    do {
        PARSE_JSON_ELEM(argv[0], "session", session, int64);
        PARSE_JSON_ELEM(argv[0], "method", method, string);
        PARSE_JSON_ELEM(argv[0], "path", path, string);
    }while(0);

    if (session && method && path) {
        ne_request* request = ne_request_create((ne_session *)session, method, path);
        iret = (long)request;
    }
    unqlite_result_int64(pCtx, iret);
    return UNQLITE_OK;
}

struct jx9_ne_body_t {
    int bufsize;
    char *buf;
    int len;
};

static int ne_block_reader_callback(void *userdata, const char *buf, size_t len)
{
    if (buf && len > 0 && userdata) {
        //printf("[C] block reader: %s\n", buf);
        struct jx9_ne_body_t *ubody = (struct jx9_ne_body_t *)userdata;
        if (!ubody->buf || (ubody->bufsize < ubody->len + len)) {
            int newsize = (ubody->len + len) * 2;
            char *newbuf = malloc(newsize);
            bzero(newbuf, newsize);
            memcpy(newbuf, ubody->buf, ubody->len);

            free(ubody->buf);
            ubody->buf = newbuf;
            ubody->bufsize = newsize;
        }
        memcpy(ubody->buf+ubody->len, buf, len);
        ubody->len += len;
    }
    return 0;
}

int jx9_ne_request_dispatch(unqlite_context* pCtx, int argc, unqlite_value** argv)
{
    long iret = -1;
    long request = 0;

    CHECK_ARGV_RETURN(pCtx, argc, argv[0]);

    do {
        PARSE_JSON_ELEM(argv[0], "request", request, int64);
    }while(0);

    if (request) {
        struct jx9_ne_body_t ubody; 
        bzero(&ubody, sizeof(ubody));
        ne_add_response_body_reader((ne_request *)request, 
            ne_accept_always, ne_block_reader_callback, (void*)&ubody);
        iret = ne_request_dispatch((ne_request *)request);
        
        if (ubody.buf && ubody.len > 0) { 
            unqlite_value *jx9_msg = unqlite_context_new_scalar(pCtx);
            unqlite_value_string(jx9_msg, (const char *)ubody.buf, ubody.len);
            unqlite_array_add_strkey_elem(argv[0], "body", jx9_msg);
            unqlite_context_release_value(pCtx, jx9_msg);
        }
        free(ubody.buf);
    }
    unqlite_result_int64(pCtx, iret);
    return UNQLITE_OK;
}
int jx9_ne_request_destroy(unqlite_context* pCtx, int argc, unqlite_value** argv)
{
    long iret = -1;
    long request = 0;

    CHECK_ARGV_RETURN(pCtx, argc, argv[0]);

    do {
        PARSE_JSON_ELEM(argv[0], "request", request, int64);
    }while(0);

    if (request) {
        ne_request_destroy((ne_request *)request);
        iret = 0;
    }
    unqlite_result_int64(pCtx, iret);
    return UNQLITE_OK;
}

/**
 * void ne_add_request_header( ne_request *request, const char *name, const char *value);
 */
int jx9_ne_add_request_header(unqlite_context* pCtx, int argc, unqlite_value** argv)
{
    long iret = -1;
    long request = 0;
    const char* name = NULL;
    const char* value = NULL;

    CHECK_ARGV_RETURN(pCtx, argc, argv[0]);

    do {
        PARSE_JSON_ELEM(argv[0], "request", request, int64);
        PARSE_JSON_ELEM(argv[0], "name", name, string);
        PARSE_JSON_ELEM(argv[0], "value", value, string);
    }while(0);

    if (request && name && value) {
        ne_add_request_header((ne_request *)request, name, value);
        iret = 0;
    }
    unqlite_result_int64(pCtx, iret);
    return UNQLITE_OK;
}

/**
 * const char *ne_get_response_header(ne_request *request, const char *name);
 */
int jx9_ne_get_response_header(unqlite_context* pCtx, int argc, unqlite_value** argv)
{
    long iret = -1;
    long request = 0;
    const char* name = NULL;

    CHECK_ARGV_RETURN(pCtx, argc, argv[0]);

    do {
        PARSE_JSON_ELEM(argv[0], "request", request, int64);
        PARSE_JSON_ELEM(argv[0], "name", name, string);
    }while(0);

#if 0
    /* iterator all response-headers */
    if (request) {
        void *cursor = NULL;
        printf("[C] start\n");
        do{
            const char *name = NULL;
            const char *value = NULL;
            cursor = ne_response_header_iterate((ne_request *)request, cursor, &name, &value);
            printf("[C] name: %s, value: %s\n", name, value);
        }while(cursor != NULL);
        printf("[C] end\n");
    }
#endif
    if (request && name) {
        const char* value = ne_get_response_header((ne_request *)request, name);
        if (value) {
            iret = strlen(value);
            unqlite_value *jx9_msg = unqlite_context_new_scalar(pCtx);
            unqlite_value_string(jx9_msg, (const char *)value, iret);
            unqlite_array_add_strkey_elem(argv[0], "value", jx9_msg);
            unqlite_context_release_value(pCtx, jx9_msg);
        } 
    }
    unqlite_result_int64(pCtx, iret);
    return UNQLITE_OK;
}

/**
 * void ne_set_useragent(ne_session *session, const char *product);
 */
int jx9_ne_set_useragent(unqlite_context* pCtx, int argc, unqlite_value** argv)
{
    long iret = -1;
    long session = 0;
    const char* product = NULL;

    CHECK_ARGV_RETURN(pCtx, argc, argv[0]);

    do {
        PARSE_JSON_ELEM(argv[0], "session", session, int64);
        PARSE_JSON_ELEM(argv[0], "product", product, string);
    }while(0);

    if (session && product) {
        ne_set_useragent((ne_session *)session, product);
        iret = 0;
    }
    unqlite_result_int64(pCtx, iret);
    return UNQLITE_OK;
}
int jx9_ne_set_read_timeout(unqlite_context* pCtx, int argc, unqlite_value** argv)
{
    long iret = -1;
    long session = 0;
    int timeout = 0;

    CHECK_ARGV_RETURN(pCtx, argc, argv[0]);

    do {
        PARSE_JSON_ELEM(argv[0], "session", session, int64);
        PARSE_JSON_ELEM(argv[0], "timeout", timeout, int);
    }while(0);

    if (session && timeout > 0) {
        ne_set_read_timeout((ne_session *)session, timeout);
        iret = 0;
    }
    unqlite_result_int64(pCtx, iret);
    return UNQLITE_OK;
}
int jx9_ne_set_connect_timeout(unqlite_context* pCtx, int argc, unqlite_value** argv)
{
    long iret = -1;
    long session = 0;
    int timeout = 0;

    CHECK_ARGV_RETURN(pCtx, argc, argv[0]);

    do {
        PARSE_JSON_ELEM(argv[0], "session", session, int64);
        PARSE_JSON_ELEM(argv[0], "timeout", timeout, int);
    }while(0);

    if (session && timeout > 0) {
        ne_set_connect_timeout((ne_session *)session, timeout);
        iret = 0;
    }
    unqlite_result_int64(pCtx, iret);
    return UNQLITE_OK;
}

/**
 * void ne_set_request_body_buffer(ne_request *req, const char *buf, size_t count);
 */
int jx9_ne_set_request_body_buffer(unqlite_context* pCtx, int argc, unqlite_value** argv)
{
    long iret = -1;
    long request = 0;
    const char* buffer = NULL;
    int count = 0;

    CHECK_ARGV_RETURN(pCtx, argc, argv[0]);

    do {
        PARSE_JSON_ELEM(argv[0], "request", request, int64);
        PARSE_JSON_ELEM(argv[0], "buffer", buffer, string);
        PARSE_JSON_ELEM(argv[0], "count", count, int);
    }while(0);

    if (request && buffer && count > 0) {
        ne_set_request_body_buffer((ne_request *)request, buffer, count);
        iret = 0;
    }
    unqlite_result_int64(pCtx, iret);
    return UNQLITE_OK;
}

/**
 * const ne_status *ne_get_status(const ne_request *request);
 */
int jx9_ne_get_status(unqlite_context* pCtx, int argc, unqlite_value** argv)
{
    long iret = -1;
    long request = 0;

    CHECK_ARGV_RETURN(pCtx, argc, argv[0]);

    do {
        PARSE_JSON_ELEM(argv[0], "request", request, int64);
    }while(0);

    if (request) {
        const ne_status* status = ne_get_status((const ne_request *)request);
        if (status) {
            unqlite_value *jx9_msg = unqlite_context_new_scalar(pCtx);
            unqlite_value_string_format(jx9_msg, "%d:%d", 
                status->major_version, status->minor_version);
            unqlite_array_add_strkey_elem(argv[0], "version", jx9_msg);

            unqlite_value_int(jx9_msg, status->code); 
            unqlite_array_add_strkey_elem(argv[0], "code", jx9_msg);

            unqlite_value_int(jx9_msg, status->klass); 
            unqlite_array_add_strkey_elem(argv[0], "klass", jx9_msg);

            unqlite_value_string_format(jx9_msg, "%s", status->reason_phrase); 
            unqlite_array_add_strkey_elem(argv[0], "reason", jx9_msg);
            unqlite_context_release_value(pCtx, jx9_msg);
        }
        iret = 0;
    }
    unqlite_result_int64(pCtx, iret);
    return UNQLITE_OK;
}

/**
 * void ne_set_server_auth(ne_session *session, ne_auth_creds callback, void *userdata);
 */
static int ne_auth_creds_callback(void *userdata, const char* realm, int attempt, 
    char *username, char *password)
{
    if (userdata) {
        char *sep = strstr((char *)userdata, ":");
        if (sep) {
            int ilen = (int)(sep - (char *)userdata);
            int ulen = (ilen <= NE_ABUFSIZ) ? ilen : NE_ABUFSIZ;
            strncpy(username, userdata, ulen);
            strncpy(password, userdata+(ilen+1), NE_ABUFSIZ);
        }
    }
    return attempt;
}
int jx9_ne_set_server_auth(unqlite_context* pCtx, int argc, unqlite_value** argv)
{
    long iret = -1;
    long session = 0;
    const char* userdata = NULL;

    CHECK_ARGV_RETURN(pCtx, argc, argv[0]);

    do {
        PARSE_JSON_ELEM(argv[0], "session", session, int64);
        PARSE_JSON_ELEM(argv[0], "userdata", userdata, string);
    }while(0);

    if (session && userdata) {
        ne_set_server_auth((ne_session*)session, ne_auth_creds_callback, (void *)userdata);
        iret = 0;
    }
    unqlite_result_int64(pCtx, iret);
    return UNQLITE_OK;
}


