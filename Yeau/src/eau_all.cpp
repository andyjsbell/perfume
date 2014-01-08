#include "eau_all.h"

using namespace eau;

CEauApi::CEauApi()
{
    m_pSink = NULL;
}

CEauApi::~CEauApi()
{}


void CEauApi::SetSink(IEauSink *pSink)
{
    m_pSink = pSink;
}

bool CEauApi::Register(const char *name, const char *pass)
{
    m_szName = string(name);
    m_szPass = string(pass);
    return true;
}

bool CEauApi::SignIn(const char *name, const char *pass)
{
    m_szName = string(name);
    m_szPass = string(pass);
    m_bSigned = true;
    return true;
}

bool CEauApi::SignOut()
{
    m_bSigned = false;
}

bool CEauApi::GetProjects(vector<string> &pids)
{
    map<string, pinfo_t>::iterator iter = m_vProjs.begin();
    for(iter=m_vProjs.begin(); iter != m_vProjs.end(); iter++) {
        pids.push_back(iter->first);
    }
    return true;
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


