#ifndef _EAU_ALL_H_
#define _EAU_ALL_H_

#include "ieau_api.h"
#include "eau_struct.h"
#include "jx9_api.h"

namespace eau
{
    struct user_t : public uinfo_t {
        map<string, string> todo; // <uid, "ing,no,yes">
        user_t(){}
        user_t(uinfo_t &info) : uinfo_t(info) {}
    };
    struct bill_t : public binfo_t {
        map<string, string> todo; // <uid, "ing,no,yes">
        bill_t(){}
        bill_t(binfo_t &info) : binfo_t(info) {}
    };
    struct proj_t : public pinfo_t {
        map<string, user_t> users; // <uid, ..>
        map<string, bill_t> bills; // <bid, ..>
        proj_t(){}
        proj_t(pinfo_t &info) : pinfo_t(info) {}
    };

    class CEauApi : public IEauApi, public IJx9Sink
    {
    public:
        explicit CEauApi();
        virtual ~CEauApi();

        virtual void SetSink(IEauSink *pSink);
        virtual void SetPath(const char *path);

        virtual bool Register(const char *name, const char *pass);
        virtual bool SignIn(const char *name, const char *pass);
        virtual bool SignOut();

        virtual bool GetProjects(vector<string> &pids);
        virtual bool GetProjectInfo(const string &pid, pinfo_t &pinfo);
        virtual bool GetProjectUsers(const string &pid, vector<string> &uids);
        virtual bool GetProjectUserInfo(const string &pid, const string &uid, uinfo_t &uinfo);
        virtual bool GetProjectBills(const string &pid, vector<string> &bids);
        virtual bool GetProjectBillInfo(const string &pid, const string &bid, binfo_t &binfo);

        virtual bool AddProject(pinfo_t &pinfo);
        virtual bool AddProjectBill(const string &pid, binfo_t &binfo);
        virtual bool SetProjectBill(const string &pid, const string &bid, string todo_val);
        virtual bool AddProjectUser(const string &pid, uinfo_t &uinfo);
        virtual bool DelProjectUser(const string &pid, const string &uid);

    private:
        bool SyncFromLocal();
        bool SyncIntoLocal();

        // for IJx9Sink
        virtual int OnInput(vmptr_t jx9_vm, void* data);
        virtual int OnOutput(vmptr_t jx9_vm, void* data);

    private:
        IEauSink*       m_pSink;
        string          m_szPath;

        bool            m_bSigned;
        string          m_szName;
        string          m_szPass;
        map<string, proj_t>  m_vProjs;
    };

} // namespace eau

#endif // _EAU_ALL_H_
