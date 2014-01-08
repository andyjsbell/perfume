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
    return true;
}

bool CEauApi::GetProjects(vector<string> &pids)
{
    map<string, proj_t>::iterator iter = m_vProjs.begin();
    for(iter=m_vProjs.begin(); iter != m_vProjs.end(); iter++) {
        pids.push_back(iter->first);
    }
    return true;
}

bool CEauApi::GetProjectInfo(const string &pid, pinfo_t &pinfo)
{
    map<string, proj_t>::iterator iter = m_vProjs.find(pid);
    if (iter != m_vProjs.end()) {
        pinfo = iter->second;
        return true;
    }
    return false;
}

bool CEauApi::GetProjectUsers(const string &pid, vector<string> &uids)
{
    map<string, proj_t>::iterator iter = m_vProjs.find(pid);
    if (iter != m_vProjs.end()) {
        map<string, user_t> &users = iter->second.users;
        map<string, user_t>::iterator uiter;
        for (uiter=users.begin(); uiter != users.end(); uiter++) {
            uids.push_back(uiter->first);
        }
        return true;
    }

    return false;
}

bool CEauApi::GetProjectUserInfo(const string &pid, const string &uid, uinfo_t &uinfo)
{
    map<string, proj_t>::iterator iter = m_vProjs.find(pid);
    if (iter != m_vProjs.end()) {
        map<string, user_t> &users = iter->second.users;
        map<string, user_t>::iterator uiter = users.find(uid);
        if (uiter != users.end()) {
            uinfo = uiter->second;
            return true;
        }
    }
    return false;
}

bool CEauApi::GetProjectBills(const string &pid, vector<string> &bids)
{
    map<string, proj_t>::iterator iter = m_vProjs.find(pid);
    if (iter != m_vProjs.end()) {
        map<string, bill_t> &bills = iter->second.bills;
        map<string, bill_t>::iterator biter;
        for (biter=bills.begin(); biter != bills.end(); biter++) {
            bids.push_back(biter->first);
        }
        return true;
    }
    return false;
}

bool CEauApi::GetProjectBillInfo(const string &pid, const string &bid, binfo_t &binfo)
{
    map<string, proj_t>::iterator iter = m_vProjs.find(pid);
    if (iter != m_vProjs.end()) {
        map<string, bill_t> &bills = iter->second.bills;
        map<string, bill_t>::iterator biter = bills.find(bid);
        if (biter != bills.end()) {
            binfo = biter->second;
            return true;
        }
    }
    return false;
}

bool CEauApi::CreateProject()
{
    return false;
}

bool CEauApi::AddProjectBill()
{
    return false;
}

bool CEauApi::AddProjectUser()
{
    return false;
}


