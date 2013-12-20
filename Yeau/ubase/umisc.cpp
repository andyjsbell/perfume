#include "umisc.h"
#include "libuuid/uuid.h"

namespace eau
{
    std::string uuid_generate_string()
    {
        uuid_t uuid;
        uuid_generate(uuid);

        std::string out;
        out.resize(37);
        uuid_unparse_lower(uuid, (char *)out.data());
        return out;
    }
}
