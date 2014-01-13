#ifndef _JX9_API_H_
#define _JX9_API_H_

#include "eau_struct.h"

extern "C" {
#include "unqlite/unqlite.h"
}

namespace eau
{
    typedef unqlite_vm*  vmptr_t;
    typedef unqlite_value* valptr_t;
    typedef int (*jx9_log_t)(const void* msg, unsigned int len, void* data);

    class IJx9Sink {
    public:
        virtual int OnInput(vmptr_t jx9_vm, void* data) = 0;
        virtual int OnOutput(vmptr_t jx9_vm, void* data) = 0;
    };

    int run_jx9_exec(const char *script, const char *fname, IJx9Sink *sink, void *data=NULL, jx9_log_t pflog=NULL);

    bool set_jx9_variable(vmptr_t jx9_vm, const char *jx9_name, const void *jx9_value);

    bool new_jx9_json_value(vmptr_t jx9_vm, const string &value, valptr_t &jx9_value);
    bool add_jx9_json_object(vmptr_t jx9_vm, const pair_ptr_t &key, valptr_t &jx9_json);
    bool add_jx9_json_object(vmptr_t jx9_vm, const pair_t &key, valptr_t &jx9_json);
    bool add_jx9_json_object(vmptr_t jx9_vm, const json1_t &keys, valptr_t &jx9_json);
    bool add_jx9_json_object(vmptr_t jx9_vm, const json2_t &keys, valptr_t &jx9_json);

} // namespace eau

#endif // _JX9_API_H_

