#ifndef _EAU_ALL_H_
#define _EAU_ALL_H_

#include "ieau_api.h"
#include "eau_struct.h"

namespace eau
{
    struct user_t : public uinfo_t {
        map<string, string> todo;
    };
    struct bill_t : public binfo_t {
        map<string, string> todo;
    };
    struct proj_t : public pinfo_t {
        map<string, user_t> users;
        map<string, bill_t> bills;
    };

    class CEauApi : public IEauApi
    {
    public:
        explicit CEauApi();
        virtual ~CEauApi();

        virtual void SetSink(IEauSink *pSink);

        virtual bool Register(const char *name, const char *pass);
        virtual bool SignIn(const char *name, const char *pass);
        virtual bool SignOut();

        virtual bool GetProjects(vector<string> &pids);
        virtual bool GetProjectInfo(const string &pid, pinfo_t &pinfo);
        virtual bool GetProjectUsers(const string &pid, vector<string> &uids);
        virtual bool GetProjectUserInfo(const string &pid, const string &uid, uinfo_t &uinfo);
        virtual bool GetProjectBills(const string &pid, vector<string> &bids);
        virtual bool GetProjectBillInfo(const string &pid, const string &bid, binfo_t &binfo);

        virtual bool CreateProject();
        virtual bool AddProjectBill();
        virtual bool AddProjectUser();

    private:
        IEauSink*       m_pSink;

        bool            m_bSigned;
        string          m_szName;
        string          m_szPass;
        map<string, proj_t>  m_vProjs;
    };

} // namespace eau

#endif // _EAU_ALL_H_
