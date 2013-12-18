#include <string>
#include <iostream>
using namespace std;

#include "error.h"
#include "umisc.h"
#include "store.h"
#include "zeroptr.h"

#define FUNCTAG "["<<__FUNCTION__<<"]"

#define PRINT_BEGIN  cout << "=============" << endl << FUNCTAG << endl;
#define PRINT_CRLF  cout << endl;
#define PRINT_STR(s) cout << #s"=" <<(s).str() << endl;
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
    PRINT_BEGIN;

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
    PRINT_BEGIN;

    string uuidstr = eau::uuid_generate_string();
    cout << FUNCTAG << ", uuid=" << uuidstr <<endl;
    PRINT_CRLF;
    return 0;
}

static int test_account()
{
    PRINT_BEGIN;

    eau::zeroptr<eau::CStore> pStore = new eau::RefCounted<eau::CStore>();
    string fname = "/tmp/unqlite_jx9_testing.db";
    PRINT_FUNC_LONG(pStore->Open(fname, 0));
    PRINT_CRLF;

    eau::account_t acc;
    acc.set(eau::K_id, "user_testing");
    PRINT_FUNC_LONG(pStore->GetAccount(acc));

    acc.set(eau::K_passwd, "passwd_testing");
    PRINT_FUNC_LONG(pStore->PutAccount(acc));

    acc.set(eau::K_id, "user_testing");
    acc.set(eau::K_passwd, "passwd_testing2");
    PRINT_FUNC_LONG(pStore->GetAccount(acc));

    //PRINT_FUNC_LONG(pStore->Close());

    PRINT_CRLF;
    return 0;
}

static int test_db()
{
    PRINT_BEGIN;

    eau::zeroptr<eau::CStore> pStore = new eau::RefCounted<eau::CStore>();
    string fname = "/tmp/unqlite_jx9_testing.db";
    PRINT_FUNC_LONG(pStore->Open(fname, 0));
    PRINT_CRLF;

    eau::db_t db;
    db.set(eau::K_id, eau::uuid_generate_string());
    PRINT_FUNC_LONG(pStore->PutDB(db));
    PRINT_FUNC_LONG(pStore->GetDB(db));
    
    db.set(eau::K_title, "db title");
    PRINT_FUNC_LONG(pStore->PutDB(db));
    db.set(eau::K_title, "");
    PRINT_FUNC_LONG(pStore->GetDB(db));
    
    return 0;
}

int main(int argc, char* argv[])
{
    //test_log();
    //test_uuid();
    test_account();
    //test_db();
    return 0;
}

