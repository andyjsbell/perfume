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
}

/// define common elements for nosql
namespace eau
{
    // key index
    enum eKeyIndex {
        K_Unknown = 0,
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
        K_All
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

}

/// define the structures of account, db and doc.
namespace eau
{
    // element generator class
    struct gen_t {
        explicit gen_t(const int elems[], int num) {
            _init(elems, num);
        }

        atom_t &get(int idx) {
            return _all[(_all.find(idx)==_all.end() ? K_Unknown : idx)];
        }
        bool set(int idx, const string &str) {
            get(idx) = str;
            return (_all.find(idx) != _all.end());
        }
        bool set(int idx, const pair_t &elem) {
            return set(idx, elem.second);
        }
        bool set(const pair_t &elem) {
            int idx = kKeyIndex(elem);
            return set(idx, elem.second);
        }
        bool set(const json1_t &elems) {
            for(int k=0; k < elems.size(); k++) {
                set(kKeyIndex(elems[k]), elems[k]);
            }
            return true;
        }

        const json1_t &schema() {return _schema;}
        const json1_t &uri() {return _uri;}
        void uri(const pair_t &elem) {
            for(int k=0; k < _uri.size(); k++) {
                if (_uri[k].first == elem.first) {
                    _uri[k].second = elem.second;
                    return;
                }
            }
            _uri.push_back(elem);
        }

    protected:
        void _init(const int elems[], int num) {
            _all[K_Unknown] = atom_t("none", "none"); // reserved
            for (int k=0; k < num; k++) {
                _all[elems[k]] = kKeyAtom(elems[k]);
                _schema.push_back(kKeyAtom(elems[k], "none"));
            }
        }
        map<int, atom_t> _all;
        json1_t _schema;
        json1_t _uri;
    };

    // user account
    const static int kAccountElems[] = {
        K_id, K_passwd, K_name, K_desc, K_cdate, K_mdate
    };
    struct account_t : public gen_t {
        explicit account_t() : gen_t(kAccountElems, sizeof(kAccountElems)/sizeof(int)){
            uri(pair_t("col", "account"));
        }
    };

    // user db
    const static int kDBElems[] = {
        K_id, K_title, K_desc, K_logo, K_status, K_cdate, K_mdate
    };
    struct db_t : public gen_t{
        explicit db_t() : gen_t(kDBElems, sizeof(kDBElems)/sizeof(int)){}
    };

    // user document
    const static int kDocElems[] = {
        K_id, K_title, K_desc, K_logo, K_status, K_cdate, K_mdate
    };
    struct doc_t : public gen_t{
        explicit doc_t() : gen_t(kDocElems, sizeof(kDocElems)/sizeof(int)){}
    };

} // namespace eau

#endif
