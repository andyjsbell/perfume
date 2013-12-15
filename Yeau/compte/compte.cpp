#include "compte.h"
#include "error.h"

using namespace eau;

SinCloudy* SinCloudy::s_pInst = NULL;

SinCloudy* SinCloudy::inst()
{
    return s_pInst;
}

SinCloudy::SinCloudy()
{
    m_pCompte = NULL;
}

long SinCloudy::Create(const string &user, const string &passwd)
{
    return EAU_S_OK;
}

long SinCloudy::Update(const string &user, const string &passwd)
{
    return EAU_S_OK;
}

long SinCloudy::SignIn(const string &user, const string &passwd)
{
    return EAU_S_OK;
}

long SinCloudy::SignUp()
{
    return EAU_S_OK;
}

long SinCloudy::GetCompte(zeroptr<ICompte> pCompte)
{
    pCompte = m_pCompte;
    return EAU_S_OK;
}


