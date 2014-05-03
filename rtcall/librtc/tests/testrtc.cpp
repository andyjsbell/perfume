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

    virtual void OnSize(int width, int height)
    {
        LOGI(m_tag<<" width="<<width<<", height="<<height);
    }

    virtual void OnFrame(const video_frame_t *frame)
    {
        LOGI(m_tag<<" length="<<frame->length);
    }
};

class CApplication : public IRtcSink, public PeerConnectionClientObserver {
private:
    IRtcCenter *m_rtc;
    PeerConnectionClient *m_client;
    IRtcRender *m_rrender;
    CRtcRender *m_lrender;

public:
    explicit CApplication(IRtcCenter *rtc) : m_rtc(rtc) {
        m_rrender = new CRtcRender("remote render");
        m_lrender = new CRtcRender("local render");
    }
    void SetClient(PeerConnectionClient *client) {m_client=client;}
    virtual ~CApplication() {
        delete m_lrender;
        delete m_rrender;
    }

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
        if (error == 0) {
            m_rtc->AddLocalStream();
            m_rtc->SetLocalRender(m_lrender, ADD_ACTION);
        }
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

enum {
    ON_NONE,
    ON_INIT,
    ON_UNINIT,
    ON_LOGIN,
    ON_CREATE_PC,
    ON_GET_MEDIA,
    ON_SETUP_CALL,
};

class CEventServer : public talk_base::PhysicalSocketServer {
public:
    CEventServer(talk_base::Thread* thread)
        : thread_(thread), init_(false) {}
    virtual ~CEventServer() {}
    bool Init() {
        if (init_) return true;
        xrtc_init();
        xrtc_create(rtc_);

        app_ = new CApplication(rtc_);
        rtc_->SetSink((IRtcSink *)app_);

        client_ = new PeerConnectionClient();
        client_->RegisterObserver((PeerConnectionClientObserver*)app_);
        app_->SetClient(client_);
        init_ = true;
    }
    void Uninit() {
        init_ = false;
        rtc_->Close();
        delete client_;
        delete app_;
        xrtc_destroy(rtc_);
        xrtc_uninit();
    }
    void PostMsg(int msg) { ubase::ScopedLock lock(mtx_); msgs_.push(msg);}

    // Override so that we can also pump the GTK message loop.
    virtual bool Wait(int cms, bool process_io) {
        do {
            ubase::ScopedLock lock(mtx_);
            if (msgs_.empty()) break;

            int &msg = msgs_.front();
            switch(msg) {
            case ON_INIT:
                if (!init_) Init(); break;
            case ON_UNINIT:
                if (init_) Uninit(); break;
            case ON_LOGIN:
                if (init_) client_->OnMessage(NULL);
                break;
            case ON_CREATE_PC:
                if (init_) rtc_->CreatePeerConnection();
                break;
            case ON_GET_MEDIA: {
                talk_base::AutoThread auto_thread;
                if(init_) rtc_->GetUserMedia();           
                break;
            }
            case ON_SETUP_CALL:
                if(init_) rtc_->SetupCall();
                break;
            }
            msgs_.pop();
        }while(false);

        return talk_base::PhysicalSocketServer::Wait(100, process_io);
    }

protected:
public:
    talk_base::Thread* thread_;
    PeerConnectionClient* client_;
    IRtcCenter* rtc_;
    CApplication* app_;

protected:
    bool init_;
    ubase::Mutex mtx_;
    std::queue<int> msgs_;
};

void usage() {
    const char *kUsage =
        "h: help\n"
        "i: init\n"
        "c: CreatePeerConnection\n"
        "g: GetUserMedia\n"
        "l: login\n"
        "s: SetupCall\n" 
        "q: quit\n"
    ;
    std::cout<<kUsage<<std::endl;
}

int main(int argc, char *argv[]) {

    talk_base::Thread* thread = new talk_base::Thread();
    CEventServer* event = new CEventServer(thread);
    thread->set_socketserver(event);
    thread->Start();

    bool quit = false;
    do {
        printf(">");
        char ch = getchar();
        switch(ch) {
        case 'h': usage(); break;
        case 'i': event->PostMsg(ON_INIT); break;
        case 'c': event->PostMsg(ON_CREATE_PC); break;
        case 'g': event->PostMsg(ON_GET_MEDIA); break;
        //case 'g': event->rtc_->GetUserMedia(); break;
        case 'l': {
            LOGD("login server ...");
            //std::cout<<"IP: "; std::cin>>kServIp;
            //std::cout<<"Port: "; std::cin>>kServPort;
            std::cout<<"Peer Name: "; std::cin>>kPeerName;
            event->PostMsg(ON_LOGIN);
            break;
        }
        case 's': event->PostMsg(ON_SETUP_CALL); break;
        case 'q': quit=true; break;
        }
    }while(!quit);

    LOGD("quit thread");
    if (thread) {
        thread->Stop();
        thread->Quit();
        thread->set_socketserver(NULL);
    }
    LOGD("uninit event");
    event->PostMsg(ON_UNINIT);
    return 0;
}

