#include <string>
#include <iostream>
using namespace std;

#include "error.h"
#include "umisc.h"
#include "store.h"
#include "zeroptr.h"

#define FUNCTAG "["<<__FUNCTION__<<"]"

#define PRINT_CRLF  cout << endl;
#define PRINT_STR(s) cout << #s"=" <<(s) << endl;
#define PRINT_FUNC_LONG(f) \
    {   cout << "["#f"]" << " --- begin" << endl; \
        long lret = f; \
        cout << "["#f"]" << " --- end, lret=" << lret << endl; }

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
    PRINT_CRLF;
    return 0;
}

static int test_uuid()
{
    string uuidstr = eau::uuid_generate_string();
    cout << FUNCTAG << ", uuid=" << uuidstr <<endl;
    PRINT_CRLF;
    return 0;
}

static int test_store()
{
    eau::zeroptr<eau::CStore> pStore = new eau::RefCounted<eau::CStore>();
    string fname = "/tmp/unqlite_jx9_testing.db";
    PRINT_FUNC_LONG(pStore->Open(fname, 0));
    PRINT_CRLF;

    eau::account_t acc;
    acc.user = "user_testing";
    PRINT_FUNC_LONG(pStore->GetAccount(acc));
    PRINT_CRLF;

    acc.passwd = "passwd_testing";
    PRINT_FUNC_LONG(pStore->PutAccount(acc));
    PRINT_CRLF;

    acc.user = "user_testing";
    acc.passwd = "";
    PRINT_FUNC_LONG(pStore->GetAccount(acc));
    PRINT_STR(acc.passwd.first);
    PRINT_CRLF;
    return 0;
}

int main(int argc, char* argv[])
{
    test_log();
    test_uuid();
    test_store();
    return 0;
}

