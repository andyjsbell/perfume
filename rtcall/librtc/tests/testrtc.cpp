#include "xrtc_api.h"
#include "error.h"

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
        LOG(m_tag<<" width="<<width<<", height="<<height);
    }

    virtual void OnFrame(const video_frame_t *frame)
    {
        LOG(m_tag<<" length="<<frame->length);
    }
};

class CApplication : public IRtcSink {
private:
    IRtcCenter *m_rtc;
    IRtcRender *m_rrender;
    CRtcRender *m_lrender;

public:
    explicit CApplication(IRtcCenter *rtc) : m_rtc(rtc) {
        m_rrender = new CRtcRender("remote render");
        m_lrender = new CRtcRender("local render");
    }
    virtual ~CApplication() {
        delete m_lrender;
        delete m_rrender;
    }

    virtual void OnSessionDescription(const std::string &type, const std::string &sdp) {
        LOG("type="<<type<<",\nsdp="<<sdp);
    }

    virtual void OnIceCandidate(const std::string &candidate, const std::string &sdpMid, int sdpMLineIndex) {
        LOG("candidate="<<candidate<<",\nsdpMid="<<sdpMid<<",\nsdpMLineIndex"<<sdpMLineIndex);
    }

    //> action: refer to action_t
    virtual void OnRemoteStream(int action) {
        m_rtc->SetRemoteRender(m_rrender, action);
    }

    virtual void OnGetUserMedia(int error, std::string errstr) {
        LOG("error="<<error<<", errstr="<<errstr);
        if (error == 0) {
            m_rtc->AddLocalStream();
            m_rtc->SetLocalRender(m_lrender, ADD_ACTION);
        }
    }

    virtual void OnFailureMesssage(std::string errstr) {
    }
};

void usage() {
    const char *kUsage =
        "h: help\n"
        "m: GetUserMedia\n"
        "c: CreatePeerConnection\n"
        "r: SetLocalRender\n"
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

    bool quit = false;
    do {
        char ch = getchar();
        switch(ch) {
        case 'h': usage(); break;
        case 'm': rtc->GetUserMedia(); break;
        case 'c': rtc->CreatePeerConnection(); break;
        case 's': rtc->SetupCall(); break;
        case 'q': quit=true; break;
        }
    }while(!quit);
    
    xrtc_uninit();
    return 0;
}

