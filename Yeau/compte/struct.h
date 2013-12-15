#ifndef _EAU_STRUCT_H_
#define _EAU_STRUCT_H_

#include <string>
using namespace std;

namespace eau
{
    typedef string atom_t;

    typedef struct {
        atom_t user;
        atom_t passwd;
        atom_t desc;
        atom_t cdate;
        atom_t mdate;
    }account_t;

    typedef struct {
        atom_t id;
        atom_t title;
        atom_t desc;
        atom_t logo;
        atom_t status;
        atom_t cdate;
        atom_t mdate;
    }db_t;

    typedef struct {
        atom_t id;
        atom_t title;
        atom_t desc;
        atom_t logo;
        atom_t status;
        atom_t cdate;
        atom_t mdate;
    }doc_t;

} // namespace eau

#endif
