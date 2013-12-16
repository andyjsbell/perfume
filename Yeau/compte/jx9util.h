#ifndef _EAU_JX9UTIL_H_
#define _EAU_JX9UTIL_H_

#include <stdarg.h>

#include <map>
#include <string>
using namespace std;

extern "C" {
#include "unqlite.h"
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


    long check_jx9_return(unqlite_vm* jx9_vm);
    long parse_jx9_value(unqlite_value* jx9_array, const string &key, int &value);
    long parse_jx9_value(unqlite_value* jx9_array, const string &key, string &value);

    long config_jx9_argv(unqlite_vm* jx9_vm, char *fmt, ...);
    long config_jx9_variable(unqlite_vm* jx9_vm, const vector<pair_t> &ivar);

    long process_jx9_put(unqlite* jx9_db, const char* jx9_prog, const vector<pair_t> &ivar);
    long process_jx9_get(unqlite* jx9_db, const char* jx9_prog, const vector<pair_t> &ivar, vector<pair_t> &ovar);

} // namespace eau

#endif
