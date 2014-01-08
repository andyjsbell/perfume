#ifndef _IEAU_API_H_
#define _IEAU_API_H_

#include <vector>
#include <string>
using namespace std;

namespace eau
{
    // user info
    struct uinfo_t {
        string uid;
        string role; // ["owner", "member", "viewer", "observer"]
        string stat; // ["wait", "approve", "refuse", "desperate"]
    };

    // bill info
    struct binfo_t {
        string bid;
        string name;
        string desc;
        long   cash;
        string creator;
        string stat; // ["draft", "wait", "approve", "refuse", "desperate"]
    };

    // proj info
    struct pinfo_t {
        string pid;
        string name;
        string desc;
        string creator;
    };

    class IEauSink {
    public:
        virtual void OnSignedIn() {};
        virtual void OnRegistered() {};

        virtual void OnProjectEvent() {};
        virtual void OnProjectUserEvent() {};
        virtual void OnProjectBillEvent() {};

    protected:
        virtual ~IEauSink() {}
    };

    class IEauApi {
    public:
        virtual void SetSink(IEauSink *pSink) = 0;

#if 1
        virtual bool Register(const char *name, const char *pass) = 0;
        virtual bool SignIn(const char *name, const char *pass) = 0;
        virtual bool SignOut() = 0;

        virtual bool GetProjects(vector<string> &pids) = 0;
        virtual bool GetProjectInfo(const string &pid, pinfo_t &pinfo) = 0;
        virtual bool GetProjectUsers(const string &pid, vector<string> &uids) = 0;
        virtual bool GetProjectUserInfo(const string &pid, const string &uid, uinfo_t &uinfo) = 0;
        virtual bool GetProjectBills(const string &pid, vector<string> &bids) = 0;
        virtual bool GetProjectBillInfo(const string &pid, const string &bid, binfo_t &binfo) = 0;

        virtual bool AddProject(pinfo_t &pinfo) = 0;
        virtual bool AddProjectBill(const string &pid, binfo_t &binfo) = 0;
        virtual bool SetProjectBill(const string &pid, const string &bid, string todo_val) = 0;
        virtual bool AddProjectUser(const string &pid, uinfo_t &uinfo) = 0;
        virtual bool DelProjectUser(const string &pid, const string &uid) = 0;
#endif

    protected:
        virtual ~IEauApi() {}
    };

} // namespace eau

extern "C" void CreateIEauApi(eau::IEauApi ** pp);
extern "C" void DestroyIEauApi(eau::IEauApi ** pp);

#endif // _IEAU_API_H_
