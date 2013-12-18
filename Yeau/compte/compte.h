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
        virtual bool Init(const string &fname, int mode) = 0;

        virtual bool Create(const string &user) = 0;
        virtual bool SignIn(const string &user, const string &passwd) = 0;
        virtual bool SignUp() = 0;

        virtual bool BeginCommit() = 0;
        virtual void SetPasswd(const string &passwd) = 0;
        virtual void SetDesc(const string &desc) = 0;
        virtual bool EndCommit() = 0;

        virtual bool GetCompte(zeroptr<ICompte> &pCompte) = 0;

    protected:
        virtual ~ICloudy() {}
    };

    class ICompte : public RefCount
    {
    public:
        virtual bool CreateDB(const string &title, zeroptr<IDatabase> &pDB) = 0;
        virtual bool GetAllDBs(vector<string> &dbids) = 0;
        virtual bool OpenDB(const string &dbid, zeroptr<IDatabase> &pDB) = 0;

    protected:
        virtual ~ICompte() {}
    };
        
    class IDatabase : public RefCount
    {
    public:
        virtual bool CreateDoc(const string &title, zeroptr<IDocument> &pDoc) = 0;
        virtual bool GetAllDocs(vector<string> &docids) = 0;
        virtual bool OpenDoc(const string &docid, zeroptr<IDocument> &pDoc) = 0;

        virtual bool BeginCommit() = 0;
        virtual void SetTitle(const string &title) = 0;
        virtual void SetDesc(const string &desc) = 0;
        virtual void SetLogo(const string &logo) = 0;
        virtual bool EndCommit() = 0;
    };

    class IDocument : public RefCount
    {
    public:
        virtual bool BeginCommit() = 0;
        virtual void SetTitle(const string &title) = 0;
        virtual void SetDesc(const string &desc) = 0;
        virtual void SetLogo(const string &logo) = 0;
        virtual bool EndCommit() = 0;
    };

} // end of namespace eau

#endif
