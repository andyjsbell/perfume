#ifndef _EAU_STORE_H_
#define _EAU_STORE_H_

#include "types.h"

#include <string>
using namespace std;

namespace eau
{
    typedef struct {
        string user;
        string passwd;
        string desc;
        string cdate;
        string mdate;
    }account_t;

    typedef struct {
        string id;
        string title;
        string desc;
        string logo;
        string status;
        string cdate;
        string mdate;
    }db_t;

    typedef struct {
        string id;
        string title;
        string desc;
        string logo;
        string status;
        string cdate;
        string mdate;
    }doc_t;


    class StoreImpl
    {
    public:
        explicit StoreImpl();
        ~StoreImpl();

        long Open(const char *pName, int iMode);
        long Close();

        long PutAccount(const account_t &account);
        long GetAccount(account_t &account);
        long PutDB(const db_t &db);
        long GetDB(db_t &db);
        long PutDoc(const doc_t &doc);
        long GetDoc(doc_t &doc);

    private:
        StoreImpl(const StoreImpl &original);
        void operator =(const StoreImpl &);

    private:
        handle_t m_pHandle;
    };

    class SmartStore : public StoreImpl
    {
    private:
        SmartStore(const SmartStore &original);
        void operator =(const SmartStore &);
    };

    typedef SmartStore Store;

} // namespace eau

#endif
