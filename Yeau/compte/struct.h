#ifndef _EAU_STRUCT_H_
#define _EAU_STRUCT_H_

#include <map>
#include <vector>
#include <string>
using namespace std;

namespace eau
{
    // key index
    enum eKeyIndex {
        K_id,
        K_user,
        K_name,
        K_title,
        K_passwd,
        K_desc,
        K_cdate,
        K_mdate,
        K_logo,
        K_status,
    };

    // key name
    static inline const string kKeyName(int idx)
    {
        switch(idx)
        {
        case K_id:          return "id";
        case K_user:        return "user";
        case K_name:        return "name";
        case K_title:       return "title";
        case K_passwd:      return "passwd";
        case K_desc:        return "desc";
        case K_cdate:       return "cdate";
        case K_mdate:       return "mdate";
        case K_logo:        return "logo";
        case K_status:      return "status";
        }
        return "none";
    }

    // <key, value>
    typedef std::pair<string, string> pair_t;

    // <key:idx, value>
    template <int idx>
    struct atom_t : public pair_t {
        atom_t() {this->first = kKeyName(idx);}
        atom_t(const string &value) {this->second = value;}

        const string &str() {return this->second;}
        const char* c_str() {return this->second.c_str();}
        const char* data() {return this->second.data();}
        size_t size() {return this->second.size();}
        bool empty() {return this->second.empty();}

        atom_t &operator=(const string &value) {this->second=value; return *this;}
        atom_t &operator=(const char *value) {this->second=value; return *this;}
        bool operator!=(const string &value) {return this->second != value;}
        void operator>>(vector<pair_t> &vp) const {vp.push_back(pair_t(this->first, this->second));}
        void operator>>(pair_t &p) const {p=pair_t(this->first, this->second);}
    };

    // user account
    typedef struct account_t {
        atom_t<K_user>      user;
        atom_t<K_passwd>    passwd;
        atom_t<K_desc>      desc;
        atom_t<K_cdate>     cdate;
        atom_t<K_mdate>     mdate;
    }account_t;

    // user db
    typedef struct {
        atom_t<K_id>        id;
        atom_t<K_title>     title;
        atom_t<K_desc>      desc;
        atom_t<K_logo>      logo;
        atom_t<K_status>    status;
        atom_t<K_cdate>     cdate;
        atom_t<K_mdate>     mdate;
    }db_t;

    // user document
    typedef struct {
        atom_t<K_id>        id;
        atom_t<K_title>     title;
        atom_t<K_desc>      desc;
        atom_t<K_logo>      logo;
        atom_t<K_status>    status;
        atom_t<K_cdate>     cdate;
        atom_t<K_mdate>     mdate;
    }doc_t;

} // namespace eau

#endif
