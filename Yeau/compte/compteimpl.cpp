#include "compteimpl.h"
#include "error.h"

using namespace eau;

zeroptr<ICloudy> CCloudy::s_pInst = NULL;

zeroptr<ICloudy> CCloudy::inst()
{
    return s_pInst;
}

CCloudy::CCloudy()
{
    m_pCompte = NULL;
}

long CCloudy::Create(const string &user, const string &passwd)
{
    return EAU_S_OK;
}

long CCloudy::Update(const string &user, const string &passwd)
{
    return EAU_S_OK;
}

long CCloudy::SignIn(const string &user, const string &passwd)
{
    return EAU_S_OK;
}

long CCloudy::SignUp()
{
    return EAU_S_OK;
}

long CCloudy::GetCompte(zeroptr<ICompte> &pCompte)
{
    pCompte = m_pCompte;
    return EAU_S_OK;
}


