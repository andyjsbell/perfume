#ifndef _IEAU_API_H_
#define _IEAU_API_H_

namespace eau
{
    struct user_base_t {
        string uid;
        string role;
        string stat;
    };
    struct bill_base_t {
        string bid;
        string name;
        string desc;
        long   cash;
        string creator;
        string stat;
    };
    struct proj_base_t {
        string pid;
        string name;
        string desc;
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

        virtual bool SignIn(const char *name, const char *pass) = 0;
        virtual bool SignOut() = 0;
        virtual bool Register(const char *name, const char *pass) = 0;

        virtual bool GetProjects(vector<string> &pids) = 0;

        virtual string GetProjectName(const string &pid) = 0;
        virtual string GetProjectDesc(const string &pid) = 0;
        virtual string GetProjectCreator(const string &pid) = 0;

        virtual bool GetProjectUsers(const string &pid, vector<string> &uids) = 0;
        virtual bool GetProjectUserRole()

        virtual bool GetProjectInfo(const string &pid, proj_t &proj) = 0;
        virtual bool UpdateProject(proj_t &proj) = 0;

        virtual bool GetProjectUsers() = 0;
        virtual bool GetProjectBills() = 0;

        virtual bool CreateProject(proj_t &proj) = 0;
        virtual bool AddProjectBill() = 0;
        virtual bool AddProjectUser() = 0;

    protected:
        virtual ~IEauApi() {}
    };

} // namespace eau

extern "C" void CreateIEauApi(eau::IEauApi ** pp);
extern "C" void DesctroyIEauApi(eau::IEauApi ** pp);

#endif // _IEAU_API_H_
