#ifndef _JX9_API_H_
#define _JX9_API_H_

typedef int (*jx9_in_cb_t)(void* jx9_vm, void* data);
typedef int (*jx9_out_cb_t)(void* jx9_vm, void* data);
typedef int (*jx9_log_cb_t)(const void* msg, unsigned int len, void* data);

int run_jx9_exec(const char *script, const char *fname,
        jx9_in_cb_t pf_in,
        jx9_out_cb_t pf_out,
        jx9_log_cb_t pf_log);


#endif // _JX9_API_H
