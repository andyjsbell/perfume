#include <string>
#include <iostream>
using namespace std;

#include "error.h"
#include "umisc.h"
#include "store.h"
#include "zeroptr.h"

#define FUNCTAG "["<<__FUNCTION__<<"]"

#define PRINT_LONG(f) \
    { long lret = f; cout << "["#f"]" << " called, lret=" << lret << endl; }

static int get_cnt()
{
    static int _cnt = 0;
    return _cnt++;
}

static int test_log()
{
    log_assert2(get_cnt(), 0);
    log_assert2(get_cnt(), 1);
    log_assert2(get_cnt(), 3);
    log_assert2(get_cnt(), 3);

    log_print("ok");
    return 0;
}

static int test_uuid()
{
    string uuidstr = eau::uuid_generate_string();
    cout << FUNCTAG << ", uuid=" << uuidstr <<endl;
    return 0;
}

static int test_store()
{
    eau::zeroptr<eau::CStore> pStore = new eau::RefCounted<eau::CStore>();
    string fname = "/tmp/unqlite_jx9_testing.db";
    PRINT_LONG(pStore->Open(fname, 0));

    eau::account_t acc;
    acc.user = "user_testing";
    PRINT_LONG(pStore->GetAccount(acc));
    PRINT_LONG(pStore->PutAccount(acc));
    PRINT_LONG(pStore->GetAccount(acc));
    return 0;
}

int main(int argc, char* argv[])
{
    test_log();
    test_uuid();
    test_store();
    return 0;
}

