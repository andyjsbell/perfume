#include "xrtc_api.h"
#include "xrtc_std.h"
#include "webrtc.h"
#include "error.h"

talk_base::scoped_refptr<webrtc::PeerConnectionFactoryInterface> _pc_factory = NULL;


class CRtcCenter : public IRtcCenter, public xrtc::NavigatorUserMediaCallback {
private:
    ubase::zeroptr<xrtc::RTCPeerConnection> m_pc;
    ubase::zeroptr<xrtc::MediaStream> m_local_stream;

    ICallSink *m_call_sink;
    IAnswerSink *m_answer_sink;

public:
bool Init() {
    return true;
}

virtual long GetUserMedia() {
    returnv_assert (_pc_factory.get(), UBASE_E_INVALIDPTR);
    xrtc::MediaStreamConstraints constraints;
    constraints.audio = true;
    constraints.video = true;
    xrtc::GetUserMedia(constraints, (xrtc::NavigatorUserMediaCallback *)this);
    return UBASE_S_OK;
}

virtual long CreatePeerConnection() {
    returnv_assert (_pc_factory.get(), UBASE_E_INVALIDPTR);
    m_pc = xrtc::CreatePeerConnection(_pc_factory);
}

virtual long  SetupCall(ICallSink *sink) {
    returnv_assert (m_pc.get(), UBASE_E_INVALIDPTR);
    xrtc::MediaConstraints constraints;
    m_pc->createOffer(constraints);   
    m_call_sink = sink;
    return UBASE_S_OK;
}

virtual long AddRender(IRenderSink *render) {

}

virtual long AnswerCall(IAnswerSink * sink) {
    m_answer_sink = sink;
}

virtual void SuccessCallback(xrtc::MediaStreamPtr stream)         {
    m_local_stream = stream;
}

virtual void ErrorCallback(xrtc::NavigatorUserMediaError &error)  {
}

};


//
//>======================================================

bool xrtc_init()
{
    _pc_factory = webrtc::CreatePeerConnectionFactory();
    returnb_assert (_pc_factory.get());
    return true;
}

void xrtc_uninit()
{
    _pc_factory = NULL;
}

bool xrtc_create(IRtcCenter * &rtc)
{
    rtc = NULL;
    returnv_assert (_pc_factory.get(), UBASE_E_FAIL);

    CRtcCenter *pRtc = new CRtcCenter();
    if (!pRtc->Init()) {
        delete pRtc;
        pRtc = NULL;
    }
    rtc = (IRtcCenter *)pRtc;
    return (rtc != NULL);
}

void xrtc_destroy(IRtcCenter *rtc)
{
    return_assert(rtc);
    delete rtc;
}
