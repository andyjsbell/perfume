#ifndef _COMPTE_H_20131213_
#define _COMPTE_H_20131213_

#include <string>
#include <vector>

namespace eau {

typedef unsigned long long qdate_t;
typedef struct {
    uint8_t id1[8];
    uint8_t id2[4];
    uint8_t id3[4];
    uint8_t id4[4];
    uint8_t id5[12];
}uuid_t;
typedef std::vector<uuid_t> uuids_t;

/**  
 * Description:
 *  one compte => one or many documents
 *  one document => one or many items
 */

class IID
{
public:
    virtual bool GetID(uuid_t & id) = 0;
};

class IItem : public IID
{
protected:
    virtual ~IItem() {}
};

class IDoc : public IID
{
public:
    virtual long GetAllItems(uuids_t &ids) = 0;
    virtual long GetItem(const uuid_t &id, IItem & item) = 0;
    virtual long PutItem(IItem & item) = 0;

protected:
    virtual ~IDoc() {}
};

class ICompte : public IID
{
public:
    virtual long GetAllDocs(uuids_t &ids) = 0;
    virtual long GetDoc(const uuid_t &id, IDoc & doc) = 0;
    virtual long PutDoc(IDoc & doc) = 0;
};

static long OpenCompte(uuid_t &id, ICompte * &compte);


} // end of namespace eau

#endif

