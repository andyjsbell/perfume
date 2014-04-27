#include "xrtc_api.h"
#include "xrtc_std.h"
#include "webrtc.h"
#include "error.h"

talk_base::scoped_refptr<webrtc::PeerConnectionFactoryInterface> _pc_factory = NULL;

class WebrtcRender : public webrtc::VideoRendererInterface {
private:
    IRtcRender *m_render;
public:
WebrtcRender() {
    m_render = NULL;
}

void SetRender(IRtcRender *render) {
    m_render = render;
}

///> For webrtc::VideoRendererInterface
virtual void SetSize(int width, int height) {
    return_assert(m_render);
    m_render->OnSize(width, height);
}
virtual void RenderFrame(const cricket::VideoFrame* pFrame) {
    return_assert(m_render);
    m_render->OnFrame(NULL);
}

};


class CRtcCenter : public IRtcCenter, 
    public xrtc::NavigatorUserMediaCallback,
    public xrtc::RTCPeerConnectionEventHandler
{
private:
    ubase::zeroptr<xrtc::RTCPeerConnection> m_pc;
    ubase::zeroptr<xrtc::MediaStream> m_local_stream;
    IRtcSink *m_sink;
    WebrtcRender *m_local_render;
    WebrtcRender *m_remote_render;

public:
bool Init() {
    return true;
}

virtual void SetSink(IRtcSink *sink) {
    m_sink = sink;
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
    if (m_pc.get()) {
        m_pc->Put_EventHandler((xrtc::RTCPeerConnectionEventHandler *)this);
    }
    return (m_pc.get() != NULL) ? UBASE_S_OK : UBASE_E_FAIL;
}

virtual long AddLocalStream() {
    returnv_assert (m_pc.get(), UBASE_E_INVALIDPTR);
    xrtc::MediaConstraints constraints;
    m_pc->addStream(m_local_stream, constraints);
    return UBASE_S_OK;
}

long AddRender(sequence<xrtc::MediaStreamPtr> streams, WebrtcRender *render) {
    returnv_assert (!streams.empty(), UBASE_E_FAIL);
    sequence<xrtc::MediaStreamTrackPtr> tracks = streams[0]->getVideoTracks();
    returnv_assert (tracks.empty(), UBASE_E_FAIL);
    
    xrtc::VideoStreamTrack *vtrack = (xrtc::VideoStreamTrack *)tracks[0].get();
    webrtc::VideoTrackInterface *mtrack =(webrtc::VideoTrackInterface *) vtrack->getptr();
    mtrack->AddRenderer((webrtc::VideoRendererInterface *)render);
    return UBASE_S_OK;
}

long RemoveRender(sequence<xrtc::MediaStreamPtr> streams, WebrtcRender *render) {
    returnv_assert (!streams.empty(), UBASE_E_FAIL);
    sequence<xrtc::MediaStreamTrackPtr> tracks = streams[0]->getVideoTracks();
    returnv_assert (tracks.empty(), UBASE_E_FAIL);
    
    xrtc::VideoStreamTrack *vtrack = (xrtc::VideoStreamTrack *)tracks[0].get();
    webrtc::VideoTrackInterface *mtrack =(webrtc::VideoTrackInterface *) vtrack->getptr();
    mtrack->RemoveRenderer((webrtc::VideoRendererInterface *)render);
    return UBASE_S_OK;
}

virtual long AddLocalRender(IRtcRender *render) {
    returnv_assert (m_pc.get(), UBASE_E_INVALIDPTR);
    m_local_render->SetRender(render);
    return AddRender(m_pc->getLocalStreams(), m_local_render);
}

virtual long RemoveLocalRender(IRtcRender *render) {
    returnv_assert (m_pc.get(), UBASE_E_INVALIDPTR);
    long lret = RemoveRender(m_pc->getLocalStreams(), m_local_render);
    m_local_render->SetRender(NULL);
    return lret;
}

virtual long AddRemoteRender(IRtcRender *render) {
    returnv_assert (m_pc.get(), UBASE_E_INVALIDPTR);
    m_remote_render->SetRender(render);
    return AddRender(m_pc->getRemoteStreams(), m_remote_render);
}

virtual long RemoveRemoteRender(IRtcRender *render) {
    returnv_assert (m_pc.get(), UBASE_E_INVALIDPTR);
    long lret = RemoveRender(m_pc->getRemoteStreams(), m_remote_render);
    m_remote_render->SetRender(NULL);
    return lret;
}

virtual long SetupCall() {
    returnv_assert (m_pc.get(), UBASE_E_INVALIDPTR);
    xrtc::MediaConstraints constraints;
    m_pc->createOffer(constraints);   
    return UBASE_S_OK;
}

virtual long AnswerCall() {
    return UBASE_S_OK;
}

virtual long SetLocalSdp(const std::string &type, const std::string &sdp) {
    returnv_assert (m_pc.get(), UBASE_E_INVALIDPTR);
    xrtc::RTCSessionDescription desc;
    desc.type = type;
    desc.sdp = sdp;
    m_pc->setLocalDescription(desc);
    return UBASE_S_OK;
}

virtual long SetRemoteSdp(const std::string & type, const std::string &sdp) {
    returnv_assert (m_pc.get(), UBASE_E_INVALIDPTR);
    xrtc::RTCSessionDescription desc;
    desc.type = type;
    desc.sdp = sdp;
    m_pc->setRemoteDescription(desc);
    return UBASE_S_OK;
}

virtual long AddIceCandidate(const std::string &candidate, const std::string &sdpMid, int sdpMLineIndex) {
    returnv_assert (m_pc.get(), UBASE_E_INVALIDPTR);
    xrtc::RTCIceCandidate ice;
    ice.candidate = candidate;
    ice.sdpMid = sdpMid;
    ice.sdpMLineIndex = sdpMLineIndex;
    m_pc->addIceCandidate(ice);
}

///> For xrtc::NavigatorUserMediaCallback
virtual void SuccessCallback(xrtc::MediaStreamPtr stream)         {
    m_local_stream = stream;
}

virtual void ErrorCallback(xrtc::NavigatorUserMediaError &error)  {
}

virtual void onnegotiationneeded()                  {}

virtual void onicecandidate(xrtc::RTCIceCandidate & ice) {
    m_sink->OnIceCandidate(ice.candidate, ice.sdpMid, ice.sdpMLineIndex);
}
virtual void onsignalingstatechange(int state)      {}
virtual void onaddstream(xrtc::MediaStreamPtr stream) {
    xrtc::MediaConstraints constraints;
    m_pc->addStream(stream, constraints);
    m_sink->OnAddStream();
}
virtual void onremovestream(xrtc::MediaStreamPtr stream) {
    m_pc->removeStream(stream);
    m_sink->OnRemoveStream();
}
virtual void oniceconnectionstatechange(int state)  {
}

virtual void onsuccess(xrtc::RTCSessionDescription &desc) {
    m_sink->OnSessionDescription(desc.type, desc.sdp);
}
virtual void onfailure(const xrtc::DOMString &error)      {}
virtual void onerror()                              {}

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
