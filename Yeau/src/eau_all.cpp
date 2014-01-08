#include "eau_all.h"
#include "error.h"
#include "umisc.h"

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
    returnb_assert(!m_bSigned);
    m_szName = string(name);
    m_szPass = string(pass);
    m_bSigned = true;
    return true;
}

bool CEauApi::SignOut()
{
    returnb_assert(m_bSigned);
    m_bSigned = false;
    return true;
}

bool CEauApi::GetProjects(vector<string> &pids)
{
    returnb_assert(m_bSigned);

    map<string, proj_t>::iterator iter = m_vProjs.begin();
    for(iter=m_vProjs.begin(); iter != m_vProjs.end(); iter++) {
        pids.push_back(iter->first);
    }
    return true;
}

bool CEauApi::GetProjectInfo(const string &pid, pinfo_t &pinfo)
{
    returnb_assert(m_bSigned);

    map<string, proj_t>::iterator iter = m_vProjs.find(pid);
    if (iter != m_vProjs.end()) {
        pinfo = iter->second;
        return true;
    }
    return false;
}

bool CEauApi::GetProjectUsers(const string &pid, vector<string> &uids)
{
    returnb_assert(m_bSigned);

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
    returnb_assert(m_bSigned);

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
    returnb_assert(m_bSigned);

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
    returnb_assert(m_bSigned);

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

bool CEauApi::AddProject(pinfo_t &pinfo)
{
    returnb_assert(m_bSigned);

    pinfo.pid = uuid_generate_string();
    pinfo.creator = m_szName;
    proj_t proj(pinfo);

    // default project creator is also owner
    user_t user;
    user.uid = m_szName;
    user.role = "owner";
    user.stat = "approve";
    proj.users[user.uid] = user;

    m_vProjs[pinfo.pid] = proj;
    return true;
}

bool CEauApi::AddProjectBill(const string &pid, binfo_t &binfo)
{
    returnb_assert(m_bSigned);

    map<string, proj_t>::iterator iter = m_vProjs.find(pid);
    if (iter == m_vProjs.end())
        return false;
    
    proj_t &proj = iter->second;
    map<string, user_t>::iterator uiter = proj.users.find(m_szName);
    if (uiter == proj.users.end())
        return false;

    user_t &user = uiter->second;
    if (user.role != "owner" && user.role != "member")
        return false; 

    binfo.bid = uuid_generate_string();
    binfo.creator = m_szName;

    bill_t bill(binfo);
    // new bill need all owners' permission
    for(uiter=proj.users.begin(); uiter != proj.users.end(); uiter++) {
        if (uiter->second.role == "owner") {
            bill.todo[uiter->first] = "ing";
            if (uiter->first == m_szName)
                bill.todo[uiter->first] = "yes";
        }
    }
    proj.bills[binfo.bid] = bill;
    return true;
}

bool CEauApi::AddProjectUser()
{
    returnb_assert(m_bSigned);

    return false;
}


