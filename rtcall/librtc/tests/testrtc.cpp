#include "xrtc_api.h"
#include "error.h"
#include "pclient.h"
#include "mutex.h"

static std::string kServIp = "127.0.0.1";
static int kServPort = 8888;
static std::string kPeerName = "peer";

class CRtcRender : public IRtcRender {
private:
    std::string m_tag;

public:
    explicit CRtcRender(std::string tag) {
        m_tag = "[" + tag + "]";
    }
    virtual ~CRtcRender() {}
    virtual void OnSize(int width, int height) {
        LOGI(m_tag<<" width="<<width<<", height="<<height);
    }
    virtual void OnFrame(const video_frame_t *frame) {
        LOGI(m_tag<<" length="<<frame->length);
    }
};

class CApplication : public IRtcSink, public PeerConnectionClientObserver {
private:
    IRtcCenter *m_rtc;
    PeerConnectionClient *m_client;
    IRtcRender *m_rrender;

public:
    explicit CApplication(IRtcCenter *rtc) : m_rtc(rtc) {
        m_rrender = new CRtcRender("remote render");
    }
    virtual ~CApplication() {
        delete m_rrender;
    }
    void SetClient(PeerConnectionClient *client) {m_client=client;}

    virtual void OnSessionDescription(const std::string &type, const std::string &sdp) {
        LOGI("type="<<type<<",\nsdp="<<sdp);
    }
    virtual void OnIceCandidate(const std::string &candidate, const std::string &sdpMid, int sdpMLineIndex) {
        LOGI("candidate="<<candidate<<",\nsdpMid="<<sdpMid<<",\nsdpMLineIndex"<<sdpMLineIndex);
    }
    //> action: refer to action_t
    virtual void OnRemoteStream(int action) {
        m_rtc->SetRemoteRender(m_rrender, action);
    }
    virtual void OnGetUserMedia(int error, std::string errstr) {
        LOGI("error="<<error<<", errstr="<<errstr);
    }
    virtual void OnFailureMesssage(std::string errstr) {
        LOGI("msg="<<errstr);
    }
    virtual void OnSignedIn() {
        LOGD("ok");
    }
    virtual void OnDisconnected() {
        LOGD("ok");
    }
    virtual void OnPeerConnected(int id, const std::string& name) {
        LOGD("id="<<id<<", name="<<name);
    }
    virtual void OnPeerDisconnected(int peer_id) {
        LOGD("peer_id="<<peer_id);
    }
    virtual void OnMessageFromPeer(int peer_id, const std::string& message) {
        LOGD("peer_id="<<peer_id<<", message="<<message);
    }
    virtual void OnMessageSent(int err) {
        LOGD("err="<<err);
    }
    virtual void OnServerConnectionFailure() {
        LOGD("ok");
    }
};

class CustomThread {
public:
    CustomThread() : quit_(false) {}
    virtual ~CustomThread() {quit_=true;}
    void Start() {
        pthread_t tid;
        quit_ = false;
        pthread_create(&tid, NULL, CustomThread::Entry, (void *)this);
    }
    void Stop() { quit_ = true; usleep(1000);}
    virtual bool Wait(int cms, bool process_io) = 0;

protected:
    static void * Entry(void *param) {
        CustomThread *thiz = (CustomThread *)param;
        if (!thiz) return NULL;
        thiz->Run();
        return NULL;
    }
    void Run() {
        do { Wait(100, false); }while(!quit_); 
    }
    bool quit_;
};

enum {
    ON_NONE,
    ON_LOGIN,
};
class CEventServer : public CustomThread {
public:
    CEventServer() : init_(false) {}
    virtual ~CEventServer() {Uninit();}
    bool Init() {
        if (init_) return true;
        app_ = new CApplication(rtc_);
        rtc_->SetSink((IRtcSink *)app_);

        client_ = new PeerConnectionClient();
        client_->RegisterObserver((PeerConnectionClientObserver*)app_);
        app_->SetClient(client_);
        init_ = true;
    }
    void Uninit() {
        if (!init_) return;
        init_ = false;
        rtc_->Close();
        delete client_;
        delete app_;
    }
    void PostMsg(int msg) { ubase::ScopedLock lock(mtx_); msgs_.push(msg);}
    void SetRtc(IRtcCenter *rtc) {rtc_ = rtc;}

    // Override so that we can also pump the GTK message loop.
    virtual bool Wait(int cms, bool process_io) {
        do {
            ubase::ScopedLock lock(mtx_);
            if (msgs_.empty()) break;
            int &msg = msgs_.front();
            switch(msg) {
            case ON_LOGIN:
                client_->OnMessage(NULL);
                break;
            }
            msgs_.pop();
        }while(false);
        usleep(100*1000);
    }

protected:
    PeerConnectionClient* client_;
    IRtcCenter* rtc_;
    CApplication* app_;

    bool init_;
    ubase::Mutex mtx_;
    std::queue<int> msgs_;
};

void usage() {
    const char *kUsage =
        "h: help\n"
        "c: CreatePeerConnection\n"
        "g: GetUserMedia\n"
        "l: login\n"
        "s: SetupCall\n" 
        "q: quit\n"
    ;
    std::cout<<kUsage<<std::endl;
}

int main(int argc, char *argv[]) {
    xrtc_init();
    IRtcCenter *rtc = NULL;
    xrtc_create(rtc);

    CEventServer* event = new CEventServer();
    event->SetRtc(rtc);
    event->Init();
    event->Start();

    IRtcRender *lrender = new CRtcRender("local render");

    bool quit = false;
    do {
        printf(">");
        char ch = getchar();
        switch(ch) {
        case 'h': usage(); break;
        case 'c': 
            rtc->CreatePeerConnection();
            break;
        case 'g': 
            rtc->GetUserMedia();
            break;
        case 'l': {
            LOGD("login server ...");
            //std::cout<<"IP: "; std::cin>>kServIp;
            //std::cout<<"Port: "; std::cin>>kServPort;
            std::cout<<"Peer Name: "; std::cin>>kPeerName;
            event->PostMsg(ON_LOGIN);
            break;
        }
        case 's': 
            rtc->AddLocalStream();
            rtc->SetLocalRender(lrender, ADD_ACTION);
            rtc->SetupCall();
            break;
        case 'q': quit=true; break;
        }
    }while(!quit);

    xrtc_destroy(rtc);
    xrtc_uninit();
    return 0;
}

