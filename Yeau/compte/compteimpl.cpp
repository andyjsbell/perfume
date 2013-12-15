#include "compteimpl.h"
#include "error.h"
#include "umisc.h"

using namespace eau;

zeroptr<ICloudy> CCloudy::s_pInst = NULL;

zeroptr<ICloudy> CCloudy::inst()
{
    if (!s_pInst) {
        s_pInst = new RefCounted<CCloudy>();
    }

    return s_pInst;
}

CCloudy::CCloudy()
{
    m_pStore = NULL;
    m_pCompte = NULL;
    m_bInit = false;
    m_bSignIn = false;
}

CCloudy::~CCloudy()
{
    m_pStore = NULL;
    m_pCompte = NULL;
}

bool CCloudy::Init(const string &fname, int mode)
{
    returnv_assert(!m_bInit, false);

    m_pStore = new RefCounted<CStore>();
    long lret = m_pStore->Open(fname, mode);
    m_bInit = (lret == EAU_S_OK);
    return m_bInit;
}

bool CCloudy::Create(const string &user)
{
    returnv_assert(m_bInit, false);

    account_t acc = {user,};
    long lret = m_pStore->PutAccount(acc);
    return (lret == EAU_S_OK);
}

bool CCloudy::SignIn(const string &user, const string &passwd)
{
    returnv_assert(m_bInit, false);
    returnv_assert(!m_bSignIn, false);

    account_t acc = {user, };
    long lret = m_pStore->GetAccount(acc);
    returnv_assert2(lret, EAU_S_OK, false);
    if (acc.passwd != passwd)
        return false;

    m_bSignIn = true;
    m_cid = user; // maybe replace with random uuid
    m_pCompte = new RefCounted<CCompte>(m_cid, m_pStore);
    return true;
}

bool CCloudy::SignUp()
{
    m_bSignIn = false;
    m_pCompte = NULL;
    return true;
}

bool CCloudy::BeginCommit()
{
    returnv_assert(m_bInit, false);
    returnv_assert(m_bSignIn, false);

    m_account.user = m_cid;
    long lret = m_pStore->GetAccount(m_account);
    return (lret == EAU_S_OK);
}

bool CCloudy::EndCommit()
{
    returnv_assert(m_bInit, false);
    returnv_assert(m_bSignIn, false);

    long lret = m_pStore->PutAccount(m_account);
    return (lret == EAU_S_OK);
}

bool CCloudy::GetCompte(zeroptr<ICompte> &pCompte)
{
    returnv_assert(m_bInit, false);
    returnv_assert(m_bSignIn, false);

    pCompte = m_pCompte;
    return true;
}

///======================

CCompte::CCompte(const string &cid, zeroptr<CStore> pStore) 
    : m_cid(cid), m_pStore(pStore)
{}

bool CCompte::CreateDB(const string &title, zeroptr<IDatabase> &pDB)
{
    db_t db;
    db.id = uuid_generate_string(); 
    db.title = title;
    long lret = m_pStore->PutDB(db);
    returnv_assert2(lret, EAU_S_OK, false);

    pDB = new RefCounted<CDatabase>(db.id, m_pStore);
    return true;
}

bool CCompte::GetAllDBs(vector<string> &dbids)
{
    return true;
}

bool CCompte::OpenDB(const string &dbid, zeroptr<IDatabase> &pDB)
{
    db_t db;
    db.id = dbid; 
    long lret = m_pStore->GetDB(db);
    returnv_assert2(lret, EAU_S_OK, false);

    pDB = new RefCounted<CDatabase>(db.id, m_pStore);
    return true;
}

///======================

CDatabase::CDatabase(const string &dbid, zeroptr<CStore> pStore)
    : m_dbid(dbid), m_pStore(pStore)
{}

bool CDatabase::CreateDoc(const string &title, zeroptr<IDocument> &pDoc)
{
    doc_t doc;
    doc.id = uuid_generate_string();
    doc.title = title;
    long lret = m_pStore->PutDoc(doc);
    returnv_assert2(lret, EAU_S_OK, false);

    pDoc = new RefCounted<CDocument>(doc.id, m_pStore);
    return true;
}

bool CDatabase::GetAllDocs(vector<string> &docids)
{
    return true;
}

bool CDatabase::OpenDoc(const string &docid, zeroptr<IDocument> &pDoc)
{
    doc_t doc;
    doc.id = docid; 
    long lret = m_pStore->GetDoc(doc);
    returnv_assert2(lret, EAU_S_OK, false);

    pDoc = new RefCounted<CDocument>(doc.id, m_pStore);
    return true;
}

bool CDatabase::BeginCommit()
{
    m_db.id = m_dbid; 
    long lret = m_pStore->GetDB(m_db);
    return (lret == EAU_S_OK);
}

bool CDatabase::EndCommit()
{
    long lret = m_pStore->PutDB(m_db);
    return (lret == EAU_S_OK);
}

///======================

CDocument::CDocument(const string &docid, zeroptr<CStore> pStore)
    : m_docid(docid), m_pStore(pStore)
{}

bool CDocument::BeginCommit()
{
    m_doc.id = m_docid; 
    long lret = m_pStore->GetDoc(m_doc);
    return (lret == EAU_S_OK);
}

bool CDocument::EndCommit()
{
    long lret = m_pStore->PutDoc(m_doc);
    return (lret == EAU_S_OK);
}

