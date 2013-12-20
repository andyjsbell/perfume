#ifndef _EAU_JX9UTIL_H_
#define _EAU_JX9UTIL_H_

#include <stdarg.h>

#include <map>
#include <string>
using namespace std;

extern "C" {
#include "unqlite/unqlite.h"
}

#include "struct.h"
#include "error.h"

namespace eau
{
    static const char kEauRecordInVar[] = "eau_record_in";
    static const char kEauRecordOutVar[] = "eau_record_out";

    static const char kPutAccountScript[] = "scripts/putaccount.jx9";
    static const char kGetAccountScript[] = "scripts/getaccount.jx9";

    static const char kPutDBScript[] = "scripts/putdb.jx9";
    static const char kGetDBScript[] = "scripts/getdb.jx9";

    static const char kPutDocScript[] = "scripts/putdoc.jx9";
    static const char kGetDocScript[] = "scripts/getdoc.jx9";

    //
    // $eau_jx9_arg = {
    //      ## [mandary] for col, [option] for url
    //      uri: {col:"collection name"[, url:"http put/get url"]}, 
    //      ## if not exists, process all in uri.col; else process one of this key. 
    //      key: {full/part members of record in uri.col},
    //      ## [option] only valid for putter
    //      val: {full/part members of record in uri.col},  
    //      ## [option] for col's schema
    //      scm: {},
    //      ## [option] see below
    //      err: {...},
    //  }
    //
    // $eau_jx9_arg.err = {
    //      s_ok:           {ret: 0,    msg: jx9_s_ok},
    //      s_false:        {ret: -1,   msg: jx9_s_false},
    //      e_fail:         {ret: 1,    msg: jx9_e_fail},
    //      e_uri:          {ret: 10,   msg: jx9_e_uri},
    //      e_arg:          {ret: 11,   msg: jx9_e_arg},
    //      e_db_create:    {ret: 20,   msg: jx9_e_db_create},
    //      e_db_store:     {ret: 21,   msg: jx9_e_db_store},
    //      e_db_exists:    {ret: 22,   msg: jx9_e_db_exists},
    // };
    //
    // jx9 @return = {one of $eau_jx9_arg.err's values}, e.g, {ret: 0,  msg: jx9_s_ok};
    static const char kEauJx9Arg[] = "eau_jx9_arg"; 

    //
    // $eau_jx9_out: output results, one $eau_jx9_arg.col's record, e.g, {k1:v1, k2:v2, ...}, 
    static const char kEauJx9Out[] = "eau_jx9_out";

    static const char kEauJx9Putter[] = "scripts/jx9_putter.jx9";
    static const char kEauJx9Getter[] = "scripts/jx9_getter.jx9";


    long check_jx9_return(unqlite_vm* jx9_vm);
    long parse_jx9_value(unqlite_value* jx9_array, const string &key, int &value);
    long parse_jx9_value(unqlite_value* jx9_array, const string &key, string &value);

    long config_jx9_argv(unqlite_vm* jx9_vm, char *fmt, ...);
    long config_jx9_variable(unqlite_vm* jx9_vm, const vector<pair_t> &ivar);

    typedef int (*jx9_out_cb_t)(const void* msg, unsigned int len, void* data);
    long config_jx9_output(unqlite_vm* jx9_vm, jx9_out_cb_t pf_out, void *data);

    long process_jx9_put(unqlite* jx9_db, const char* jx9_prog, const vector<pair_t> &ivar);
    long process_jx9_get(unqlite* jx9_db, const char* jx9_prog, const vector<pair_t> &ivar, vector<pair_t> &ovar);

} // namespace eau

namespace eau 
{
    //
    // @param ivar: see $eau_jx9_arg above
    // @param ovar: see $eau_jx9_out above
    bool process_jx9_json_get(unqlite* jx9_db, const json2_t &ivar, json1_t &ovar);
    bool process_jx9_json_put(unqlite* jx9_db, const json2_t &ivar);
    bool process_jx9_json(unqlite* jx9_db, const char* jx9_prog, const json2_t &ivar, json1_t &ovar);
}

#endif
