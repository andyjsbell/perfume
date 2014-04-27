#include "observer.h"
#include "peer.h"

namespace xrtc {

bool CRTCPeerConnectionObserver::Init(ubase::zeroptr<CRTCPeerConnection> pc, 
        talk_base::scoped_refptr<webrtc::PeerConnectionInterface> conn) 
{
    m_pc = pc;
    m_conn = conn;
    return ((m_pc.get() != NULL) && (m_conn.get() != NULL));
}

CRTCPeerConnectionObserver::CRTCPeerConnectionObserver() 
{
    m_pc = NULL;
    m_conn = NULL;
}

CRTCPeerConnectionObserver::~CRTCPeerConnectionObserver()
{
    m_pc = NULL;
    m_conn = NULL;
}

///
/// for webrtc::PeerConnectionObserver
void CRTCPeerConnectionObserver::OnError() 
{
    event_process0(onerror);
}

// Triggered when the SignalingState changed.
void CRTCPeerConnectionObserver::OnSignalingChange(
        webrtc::PeerConnectionInterface::SignalingState new_state) 
{
    int state = (int)new_state;
    event_process1(onsignalingstatechange, state);
    m_pc->Put_signalingState((xrtc::RTCSignalingState)state);
}

// Triggered when SignalingState or IceState have changed.
// TODO(bemasc): Remove once callers transition to OnSignalingChange.
void CRTCPeerConnectionObserver::OnStateChange(webrtc::PeerConnectionObserver::StateType state_changed) 
{
}

// Triggered when media is received on a new stream from remote peer.
void CRTCPeerConnectionObserver::OnAddStream(webrtc::MediaStreamInterface* stream) 
{
    if (!stream)    return;
    MediaStreamPtr mstream = CreateMediaStream("remote_stream", NULL, stream);
    m_pc->m_remote_streams.push_back(mstream);
    event_process1(onaddstream, mstream);
}

// Triggered when a remote peer close a stream.
void CRTCPeerConnectionObserver::OnRemoveStream(webrtc::MediaStreamInterface* stream) 
{
    if (!stream)    return;
    MediaStreamPtr mstream = CreateMediaStream("remote_stream", NULL, stream);
    event_process1(onremovestream, mstream);
    m_pc->m_remote_streams.clear(); //TODO: only support one remote stream
}

// Triggered when a remote peer open a data channel.
// TODO(perkj): Make pure virtual.
void CRTCPeerConnectionObserver::OnDataChannel(webrtc::DataChannelInterface* data_channel) 
{}

// Triggered when renegotation is needed, for example the ICE has restarted.
void CRTCPeerConnectionObserver::OnRenegotiationNeeded() 
{
    event_process0(onnegotiationneeded);
}

// Called any time the IceConnectionState changes
void CRTCPeerConnectionObserver::OnIceConnectionChange(
        webrtc::PeerConnectionInterface::IceConnectionState new_state) 
{
    int state = (int)new_state;
    m_pc->Put_iceConnectionState((RTCIceConnectionState)state);
    event_process1(oniceconnectionstatechange, state);
}

// Called any time the IceGatheringState changes
void CRTCPeerConnectionObserver::OnIceGatheringChange(
        webrtc::PeerConnectionInterface::IceGatheringState new_state) 
{
    int state = (int)new_state;
    m_pc->Put_iceGatheringState((RTCIceGatheringState)state);
}

// New Ice candidate have been found.
void CRTCPeerConnectionObserver::OnIceCandidate(const webrtc::IceCandidateInterface* candidate) 
{
    if (!candidate)   return;
    std::string sdp;
    if (!candidate->ToString(&sdp)) {
        return;
    }

    RTCIceCandidate ice;
    ice.candidate = sdp;
    ice.sdpMid = candidate->sdp_mid();
    ice.sdpMLineIndex = candidate->sdp_mline_index();
    event_process1(onicecandidate, ice);
}

// TODO(bemasc): Remove this once callers transition to OnIceGatheringChange.
// All Ice candidates have been found.
void CRTCPeerConnectionObserver::OnIceComplete() {}

///
/// for webrtc::CreateSessionDescriptionObserver
void CRTCPeerConnectionObserver::OnSuccess(webrtc::SessionDescriptionInterface* desc) 
{
    if (!desc)  return;
    RTCSessionDescription rtcDesc;
    rtcDesc.type = desc->type();

    std::string sdp;
    desc->ToString(&sdp);
    rtcDesc.sdp = sdp;

#if 1
    const webrtc::SessionDescriptionInterface* ldesc = m_conn->local_description();
    if (ldesc) {
        ldesc->ToString(&sdp);
        RTCSessionDescription desp;
        desp.sdp = sdp;
        desp.type = ldesc->type();
        m_pc->Put_localDescription(desp);
    }

    const webrtc::SessionDescriptionInterface* rdesc = m_conn->local_description();
    if (rdesc) {
        rdesc->ToString(&sdp);
        RTCSessionDescription desp;
        desp.sdp = sdp;
        desp.type = rdesc->type();
        m_pc->Put_remoteDescription(desp);
    }
#endif

    event_process1(onsuccess, rtcDesc);
}

void CRTCPeerConnectionObserver::OnFailure(const std::string& error)
{
    event_process1(onfailure, error);
}

}
