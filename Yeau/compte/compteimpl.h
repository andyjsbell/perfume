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
        explicit CCloudy();
        virtual ~CCloudy();

        virtual bool Init(const string &fname, int mode);

        virtual bool Create(const string &user);
        virtual bool SignIn(const string &user, const string &passwd);
        virtual bool SignUp();

        virtual bool BeginCommit();
        virtual void SetPasswd(const string &passwd) {m_account.set(K_passwd, passwd);}
        virtual void SetDesc(const string &desc) {m_account.set(K_desc, desc);}
        virtual bool EndCommit();

        virtual bool GetCompte(zeroptr<ICompte> &pCompte);

    private:
        zeroptr<CStore>  m_pStore;        
        zeroptr<ICompte> m_pCompte; 

        account_t m_account;
        atom_t m_cid;
        bool m_bInit;
        bool m_bSignIn;

        static zeroptr<ICloudy> s_pInst;
    };

    class CCompte : public ICompte
    {
    public:
        explicit CCompte(const string &cid, zeroptr<CStore> pStore);

        virtual bool CreateDB(const string &title, zeroptr<IDatabase> &pDB);
        virtual bool GetAllDBs(vector<string> &dbids);
        virtual bool OpenDB(const string &dbid, zeroptr<IDatabase> &pDB);

    private:
        atom_t m_cid;
        zeroptr<CStore>  m_pStore;        
    };
        
    class CDatabase : public IDatabase
    {
    public:
        explicit CDatabase(const string &dbid, zeroptr<CStore> pStore);

        virtual bool CreateDoc(const string &title, zeroptr<IDocument> &pDoc);
        virtual bool GetAllDocs(vector<string> &docids);
        virtual bool OpenDoc(const string &docid, zeroptr<IDocument> &pDoc);

        virtual bool BeginCommit();
        virtual void SetTitle(const string &title) {m_db.set(K_title, title);}
        virtual void SetDesc(const string &desc) {m_db.set(K_desc, desc);}
        virtual void SetLogo(const string &logo) {m_db.set(K_logo, logo);}
        virtual bool EndCommit();

    private:
        db_t m_db;
        atom_t m_dbid;
        zeroptr<CStore>  m_pStore;        
    };

    class CDocument : public IDocument
    {
    public:
        explicit CDocument(const string &docid, zeroptr<CStore> pStore);

        virtual bool BeginCommit();
        virtual void SetTitle(const string &title) {m_doc.set(K_title, title);}
        virtual void SetDesc(const string &desc) {m_doc.set(K_desc, desc);}
        virtual void SetLogo(const string &logo) {m_doc.set(K_logo, logo);}
        virtual bool EndCommit();
        
    private:
        doc_t m_doc;
        atom_t m_docid;
        zeroptr<CStore>  m_pStore;        
    };

} // end of namespace eau

#endif
