#ifndef _IEAU_API_H_
#define _IEAU_API_H_

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

class IEauApi{
public:
    virtual void SetSink(IEauSink *pSink) = 0;

    virtual bool SignIn(const char *name, const char *pass) = 0;
    virtual bool SignOut() = 0;
    virtual bool Register(const char *name, const char *pass) = 0;
    
    virtual bool GetProjects() = 0;
    virtual bool GetProjectInfo() = 0;
    virtual bool GetProjectUsers() = 0;
    virtual bool GetProjectBills() = 0;

    virtual bool CreateProject() = 0;
    virtual bool AddProjectBill() = 0;
    virtual bool AddProjectUser() = 0;

pritected:
    virtual ~IEauApi() {}
};

extern "C" void CreateIEauApi(IEauApi ** pp);
extern "C" void DesctroyIEauApi(IEauApi ** pp);

#endif // _IEAU_API_H_
