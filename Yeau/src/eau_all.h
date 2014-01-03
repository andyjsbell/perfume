#ifndef _EAU_ALL_H_
#define _EAU_ALL_H_

#include "ieau_api.h"
#include "eau_struct.h"

namespace eau
{
    struct uinfo_t {
        string uid;
        string role;
        string stat;
        map<string, string> todo;
    };
    struct binfo_t {
        string bid;
        string name;
        string desc;
        long   cash;
        string creator;
        string stat;
        map<string, string> todo;
    }
    struct proj_t {
        string pid;
        string name;
        string desc;
        map<string, uinfo_t> users;
        map<string, binfo_t> bills;
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

        virtual bool GetProjects();
        virtual bool GetProjectInfo();
        virtual bool GetProjectUsers();
        virtual bool GetProjectBills();

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
