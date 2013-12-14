#include "compte.h"
#include <string>

using namespace eau;

#define IMPL_IBASE_INTF \
    public: virtual bool GetUUID(uuid_t &id) { id = __uuid; return true; } \
    private: uuid_t __uuid

class Item : public IItem
{
    IMPL_IBASE_INTF;
public:
    Item() {}

private:
    std::string m_name;
    std::string m_desc;
    int32_t m_cash;
};

class Doc : public IDoc
{
    IMPL_IBASE_INTF;
public:
    Doc()
    {
    }
private:
};

class Compte : public ICompte
{
    IMPL_IBASE_INTF;
public
    Compte()
    {}
private:
};

