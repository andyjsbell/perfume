#ifndef _EAU_COMPTEIMPL_H_
#define _EAU_COMPTEIMPL_H_

#include "compte.h"
#include "store.h"

namespace eau 
{
    class CCloudy : public ICloudy
    {
    public:
        static zeroptr<ICloudy> inst();

    public:
        CCloudy();
        zeroptr<CStore> GetStore();

        virtual long Create(const string &user, const string &passwd);
        virtual long Update(const string &user, const string &passwd);
        virtual long SignIn(const string &user, const string &passwd);
        virtual long SignUp();
        virtual long GetCompte(zeroptr<ICompte> &pCompte);

    private:
        zeroptr<ICompte> m_pCompte; 
        zeroptr<CStore>  m_pStore;        

        static zeroptr<ICloudy> s_pInst;
    };

    class CCompte : public ICompte
    {
    public:
        virtual long CreateDB(const string &dbname, zeroptr<IDatabase> &db);
        virtual long GetAllDBs(vector<string> &dbids);
        virtual long OpenDB(const string &dbid, zeroptr<IDatabase> &db);
    };
        
    class CDatabase : public IDatabase
    {
    public:
        virtual long CreateDoc(const string &docname, zeroptr<IDocument> &doc);
        virtual long GetAllDocs(vector<string> &docids);
        virtual long OpenDoc(const string &docid, zeroptr<IDocument> &doc);
    };

    class CDocument : public IDocument
    {
    public:
    };

} // end of namespace eau

#endif
