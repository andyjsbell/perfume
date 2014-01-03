#ifndef _EAU_STRUCT_H_
#define _EAU_STRUCT_H_

#include <map>
#include <vector>
#include <string>
#include <stdexcept>
using namespace std;

/// define common types for nosql
namespace eau
{
    // <string, string>
    typedef std::pair<string, string> pair_t;
    // <string, void *>
    typedef std::pair<string, void *> pair_ptr_t;
    // format: {k1:v1, k2:v2, ...}
    typedef vector<pair_t> json1_t;
    // format: {k1:{k11:v11, k12:v12}, k2:...}
    typedef map<string, vector<pair_t> > json2_t;

    static inline const string kKeyName(int idx);
    struct atom_t : public pair_t {
        atom_t() {}
        atom_t(int idx) : pair_t(kKeyName(idx), "") {}
        atom_t(int idx, const string &_second) : pair_t(kKeyName(idx), _second) {}
        atom_t(const string &_first, const string &_second) : pair_t(_first, _second) {}
        atom_t(const string &_second) {this->second = _second;}

        const string &str() {return this->second;}
        const char* c_str() {return this->second.c_str();}
        const char* data() {return this->second.data();}
        size_t size() {return this->second.size();}
        bool empty() {return this->second.empty();}
        const string &key() const {return this->first;}

        atom_t &operator=(const string &value) {this->second=value; return *this;}
        atom_t &operator=(const char* value) {this->second=value; return *this;}
        atom_t &operator=(const pair_t &elem) {this->first=elem.first; this->second=elem.second; return *this;}
        atom_t &operator=(const atom_t &elem) {this->first=elem.first; this->second=elem.second; return *this;}
        bool operator!=(const string &value) {return this->second != value;}
        bool operator!=(const char* value) {return this->second != string(value);}
        void operator>>(json1_t &elems) const {elems.push_back(pair_t(this->first, this->second));}
    };

    // key index
    enum eKeyIndex {
        K_Unknown = 0,
        K_id,
        K_name,
        K_passwd,
        K_desc,
        K_cash,
        K_creator,
        K_date,
        K_role,
        K_stat,
        K_todo,
        K_All
    };

    // key name
    static inline const string kKeyName(int idx)
    {
        switch(idx)
        {
        case K_id:          return "_id";
        case K_name:        return "name";
        case K_passwd:      return "passwd";
        case K_desc:        return "desc";
        case K_cash:        return "cash";
        case K_creator:     return "creator";
        case K_date:        return "date";
        case K_role:        return "role";
        case K_stat:        return "stat";
        case K_todo:        return "todo";
        }
        return "none";
    }

    struct atom_t;
    static const atom_t kKeyAtom(int idx, const string val="")
    {
        return atom_t(kKeyName(idx), val);
    }

    static const int kKeyIndex(const pair_t &elem)
    {
        for (int k=K_Unknown; k < K_All; k++)
            if (elem.first == kKeyName(k))
                return k;
        return K_Unknown;
    }

} // namespace eau

#endif // _EAU_STRUCT_H_

