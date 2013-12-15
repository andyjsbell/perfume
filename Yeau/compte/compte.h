#ifndef _EAU_COMPTE_H_
#define _EAU_COMPTE_H_

#include "refcount.h"
#include "zeroptr.h"

#include <vector>
#include <string>
using namespace std;

namespace eau {

    class ICompte;
    class IDatabase;
    class IDocument;

    /**  
     * Description:
     */

    class SinCloudy
    {
    public:
        static SinCloudy *inst();

        long Create(const string &user, const string &passwd);
        long Update(const string &user, const string &passwd);
        long SignIn(const string &user, const string &passwd);
        long SignUp();
        long GetCompte(zeroptr<ICompte> pCompte);

    private:
        SinCloudy();
        SinCloudy(const SinCloudy &original);
        void operator =(const SinCloudy &);

    private:
        zeroptr<ICompte> m_pCompte; 
        static SinCloudy* s_pInst;
    };

    class ICompte : public RefCount
    {
    public:
        virtual long CreateDB(const string &dbname, IDatabase &db) = 0;
        virtual long GetAllDBs(vector<string> &dbids) = 0;
        virtual long OpenDB(const string &dbid, IDatabase &db) = 0;

    protected:
        virtual ~ICompte() {}
    };
        
    class IDatabase : public RefCount
    {
    public:
        virtual long CreateDoc(const string &docname, IDocument &doc) = 0;
        virtual long GetAllDocs(vector<string> &docids) = 0;
        virtual long OpenDoc(const string &docid, IDocument &doc) = 0;
    };

    class IDocument : public RefCount
    {
    public:
    };

} // end of namespace eau

#endif
