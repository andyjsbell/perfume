#include "eau_all.h"

using namespace eau;

CEauApi::CEauAPI()
{
    m_pSink = NULL;
}

CEauApi::~CEauAPI()
{}


void CEauApi::SetSink(IEauSink *pSink)
{
    m_pSink = pSink;
}

bool CEauApi::Register(const char *name, const *pass)
{
    m_szName = string(name);
    m_szPass = string(pass);
    return true;
}

bool CEauApi::SignIn(const char *name, const *pass)
{
    m_szName = string(name);
    m_szPass = string(pass);
    m_bSigned = true;
    return true;
}

bool CEauApi::SignOut()
{
    m_bbSigned = false;
}

bool CEauApi::GetProjects()
{
}

bool CEauApi::GetProjectInfo()
{}

bool CEauApi::GetProjectUsers()
{}

bool CEauApi::GetProjectBills()
{}

bool CEauApi::CreateProject()
{}

bool CEauApi::AddProjectBill()
{}

bool CEauApi::AddProjectUser()
{}


