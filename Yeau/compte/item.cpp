#include "compte.h"
#include <string>

using namespace eau;

#define IMPL_IBASE_INTF \
    public: virtual bool GetUUID(uuid_t &id) { id = __uuid; return true; } \
    private: uuid_t __uuid


class ItemImpl : public IItem
{
    IMPL_IBASE_INTF;
public:
    ItemImpl() {}

private:
    std::string m_name;
    std::string m_desc;
    int32_t m_cash;
    std::vector<std::string> m_exts;
};

