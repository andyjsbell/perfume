#ifndef _EAU_STORE_H_
#define _EAU_STORE_H_

#include "types.h"
#include "struct.h"
#include "refcount.h"

#include <string>
using namespace std;

namespace eau
{
    class StoreImpl : public RefCount
    {
    public:
        explicit StoreImpl();
        ~StoreImpl();

        long Open(const string &fname, int mode);
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
    public:
        explicit SmartStore() : StoreImpl() {}

    private:
        SmartStore(const SmartStore &original);
        void operator =(const SmartStore &);
    };

    typedef SmartStore CStore;

} // namespace eau

#endif
