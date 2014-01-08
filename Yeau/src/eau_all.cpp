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
    
    // only project owner and member can add new bill
    proj_t &proj = iter->second;
    map<string, user_t>::iterator uiter = proj.users.find(m_szName);
    if (uiter == proj.users.end())
        return false;
    user_t &user = uiter->second;
    if (user.role != "owner" && user.role != "member")
        return false; 

    // gen bill id(bid)
    binfo.bid = uuid_generate_string();
    binfo.creator = m_szName;

    // set bill stat
    bill_t bill(binfo);
    bill.stat = "wait";
    // new bill need all owners' permission
    for(uiter=proj.users.begin(); uiter != proj.users.end(); uiter++) {
        if (uiter->second.role == "owner")
            bill.todo[uiter->first] = "ing";
    }

    // if only one owner and also he/she is bill creator
    if (bill.todo.find(m_szName) != bill.todo.end()) {
        bill.todo[m_szName] = "yes";
        if (bill.todo.size() == 1)
            bill.stat = "approve";
    }
    proj.bills[binfo.bid] = bill;
    return true;
}

bool CEauApi::SetProjectBill(const string &pid, const string &bid, string todo_val)
{
    returnb_assert(m_bSigned);

    map<string, proj_t>::iterator iter = m_vProjs.find(pid);
    if (iter == m_vProjs.end())
        return false;

    proj_t &proj = iter->second;
    map<string, bill_t>::iterator biter = proj.bills.find(bid);
    if (biter == proj.bills.end())
        return false;

    bill_t &bill = biter->second;
    if (bill.stat == "approve" || bill.stat == "refuse") // final stat
        return false;

    // bill creator can set bill stat
    if (bill.creator == m_szName) { // nop for bill creator
        if (todo_val == "no")
            bill.stat = "desperate";
        return true;
    }

    if (bill.stat == "desperate") // final stat
        return false;

    // project owner can set bill stat
    map<string, user_t>::iterator uiter = proj.users.find(m_szName);
    if (uiter == proj.users.end())
        return false;
    user_t &user = uiter->second;
    if (user.role != "owner")
        return false; 

    map<string, string>::iterator titer = user.todo.find(m_szName);
    if (titer == user.todo.end())
        return false;
    user.todo[m_szName] = todo_val;

    // check bill stat
    string bstat = "approve";
    for(titer=bill.todo.begin(); titer != bill.todo.end(); titer++) {
        if (titer->second == "no") {
            bstat = "refuse"; // final stat
            break;
        }else if (titer->second == "yes") {
            // continue;
        }else { // "ing"
            bstat = "wait";
        }
    }
    bill.stat = bstat;
    return true;
}

bool CEauApi::AddProjectUser(const string &pid, uinfo_t &uinfo)
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
    if (user.role != "owner") // only project owner have permission
        return false; 

    uiter = proj.users.find(uinfo.uid);
    if (uiter != proj.users.end()) 
        return false; // user exist
    
    user_t new_user(uinfo);
    new_user.stat = "approve";
    if (uinfo.role == "owner") {
        // new owner need all owners' permission
        new_user.stat = "wait";
        for(uiter=proj.users.begin(); uiter != proj.users.end(); uiter++) {
            if (uiter->second.role == "owner")
                new_user.todo[uiter->first] = "ing";
        }

        // self owner approve automatically
        if (new_user.todo.find(m_szName) != new_user.todo.end()) {
            new_user.todo[m_szName] = "yes";
            if (new_user.todo.size() == 1)
                new_user.stat = "approve";
        }
    }
    proj.users[new_user.uid] = new_user;
    return true;
}

bool CEauApi::DelProjectUser(const string &pid, const string &uid)
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
    if (user.role != "owner") // only project owner have permission
        return false; 

    if (uid == proj.creator)
        return false; // cannot del user who also is project owner

    uiter = proj.users.find(uid);
    if (uiter == proj.users.end()) 
        return false; // user not exist

    if (uiter->second.role == "owner") {
        // owner can only be deleted by creator
        if (proj.creator != m_szName)
            return false;
    }
    proj.users.erase(uid);
    return true;
}

