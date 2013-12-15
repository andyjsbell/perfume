#ifndef _EAU_COMPTE_H_
#define _EAU_COMPTE_H_

#include "refcount.h"
#include "zeroptr.h"

#include <vector>
#include <string>
using namespace std;

namespace eau
{
    class ICloudy;
    class ICompte;
    class IDatabase;
    class IDocument;
}

//extern "C" 
long GetCloudyInstance(eau::zeroptr<eau::ICloudy> &pCloudy);

namespace eau 
{
    /**  
     * Description:
     */

    class ICloudy : public RefCount
    {
    public:
        virtual long Create(const string &user, const string &passwd) = 0;
        virtual long Update(const string &user, const string &passwd) = 0;
        virtual long SignIn(const string &user, const string &passwd) = 0;
        virtual long SignUp() = 0;
        virtual long GetCompte(zeroptr<ICompte> &pCompte) = 0;

    protected:
        virtual ~ICloudy() {}
    };

    class ICompte : public RefCount
    {
    public:
        virtual long CreateDB(const string &dbname, zeroptr<IDatabase> &db) = 0;
        virtual long GetAllDBs(vector<string> &dbids) = 0;
        virtual long OpenDB(const string &dbid, zeroptr<IDatabase> &db) = 0;

    protected:
        virtual ~ICompte() {}
    };
        
    class IDatabase : public RefCount
    {
    public:
        virtual long CreateDoc(const string &docname, zeroptr<IDocument> &doc) = 0;
        virtual long GetAllDocs(vector<string> &docids) = 0;
        virtual long OpenDoc(const string &docid, zeroptr<IDocument> &doc) = 0;
    };

    class IDocument : public RefCount
    {
    public:
    };

} // end of namespace eau

#endif
