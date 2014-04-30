#include "xrtc_api.h"
#include "error.h"
#include "pclient.h"

static std::string kServIp = "127.0.0.1";
static int kServPort = 8888;
static std::string kPeerName = "peer";

class CustomSocketServer : public talk_base::PhysicalSocketServer {
 public:
  enum {
    ON_NONE,
    ON_MSG,
  };

  CustomSocketServer(talk_base::Thread* thread)
      : thread_(thread), client_(NULL), msg_(ON_NONE) {}
  virtual ~CustomSocketServer() {}

  void set_client(PeerConnectionClient* client) { client_ = client; }

  // Override so that we can also pump the GTK message loop.
  virtual bool Wait(int cms, bool process_io) {
    switch(msg_) {
      case ON_MSG:
        client_->OnMessage(NULL);
        msg_ = ON_NONE;
        break;
    }

    if (client_ != NULL && !client_->is_connected()) {
      //LOGD("loop, cms="<<cms);
      //thread_->Quit();
      usleep(1000*500);
    }
    return talk_base::PhysicalSocketServer::Wait(0/*cms == -1 ? 1 : cms*/,
                                                 process_io);
  }

  void post_msg(int msg) { msg_ = msg;}

 protected:
  talk_base::Thread* thread_;
  PeerConnectionClient* client_;
  bool msg_;
};

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

void usage() {
    const char *kUsage =
        "h: help\n"
        "c: CreatePeerConnection\n"
        "g: GetUserMedia\n"
        "i: init network\n"
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

    CApplication app(rtc);
    rtc->SetSink((IRtcSink *)&app);

    PeerConnectionClient client;
    client.RegisterObserver((PeerConnectionClientObserver*) &app);
    app.SetClient(&client);


    talk_base::Thread* thread = NULL;
    CustomSocketServer* socket_server = NULL;

    bool quit = false;
    do {
        printf(">");
        char ch = getchar();
        switch(ch) {
        case 'h': usage(); break;
        case 'c': rtc->CreatePeerConnection(); break;
        case 'g': rtc->GetUserMedia(); break;
        case 'i': {
            //talk_base::AutoThread auto_thread;
            //thread = talk_base::Thread::Current();
            thread = new talk_base::Thread();
            socket_server = new CustomSocketServer(thread);
            thread->set_socketserver(socket_server);
            socket_server->set_client(&client);
            thread->Start();
            break;
        }
        case 'l':
            LOGD("login server");
            //std::cout<<"IP: "; std::cin>>kServIp;
            //std::cout<<"Port: "; std::cin>>kServPort;
            std::cout<<"Peer Name: "; std::cin>>kPeerName;
            client.Connect(kServIp, kServPort, kPeerName);
            socket_server->post_msg(CustomSocketServer::ON_MSG);
            break;
        case 's': 
            LOGD("setup call");
            rtc->SetupCall(); 
            break;
        case 'q': quit=true; break;
        }
    }while(!quit);

    if (thread) {
        thread->Stop();
        thread->Quit();
        thread->set_socketserver(NULL);
    }
    xrtc_uninit();
    return 0;
}

