#ifndef _COMPTE_H_20131213_
#define _COMPTE_H_20131213_

#include "types.h"
#include "refcount.h"

namespace eau {

    /**  
     * Description:
     *  one compte => one or many documents
     *  one document => one or many items
     */

    class IBase : public RefCounted<IBase>
    {
    public:
        IBase()
        {
            memset(m_uuid, 0, sizeof(m_uuid));
        }

        virtual bool GetUUID(uuid_t & id)
        {
            id = m_uuid;
            return true;
        }
    
    protected:
        virtual ~IBase() {}
        uuid_t m_uuid;
    };

    class IItem : public IBase
    {
    protected:
        virtual ~IItem() {}
    };

    class IDoc : public IBase
    {
    public:
        virtual long GetAllItems(uuids_t &ids) = 0;
        virtual long GetItem(const uuid_t &id, IItem & item) = 0;
        virtual long PutItem(IItem & item) = 0;

    protected:
        virtual ~IDoc() {}
    };

    class ICompte : public IBase
    {
    public:
        virtual long GetAllDocs(uuids_t &ids) = 0;
        virtual long GetDoc(const uuid_t &id, IDoc & doc) = 0;
        virtual long PutDoc(IDoc & doc) = 0;
    };

    static long OpenCompte(uuid_t &id, ICompte * &compte);

} // end of namespace eau

#endif

