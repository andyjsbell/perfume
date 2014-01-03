#ifndef _EAU_ALL_H_
#define _EAU_ALL_H_

#include "ieau_api.h"
#include <string>
using namespace std;

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
};


#endif // _EAU_ALL_H_
