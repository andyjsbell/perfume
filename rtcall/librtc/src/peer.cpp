/**
 * The MIT License (MIT)
 * 
 * Copyright (c) 2014 PeterXu uskee521@gmail.com
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "xrtc_std.h"
#include "webrtc.h"

namespace xrtc {

class DummySetSessionDescriptionObserver : public webrtc::SetSessionDescriptionObserver {
public:
    static DummySetSessionDescriptionObserver* Create() {
        return new talk_base::RefCountedObject<DummySetSessionDescriptionObserver>();
    }
    virtual void OnSuccess() {
    }
    virtual void OnFailure(const std::string& error) {
    }

protected:
    DummySetSessionDescriptionObserver() {}
    ~DummySetSessionDescriptionObserver() {}
};

class CRTCPeerConnection : public RTCPeerConnection, 
    public webrtc::PeerConnectionObserver,
    public webrtc::CreateSessionDescriptionObserver {
private:
    talk_base::scoped_refptr<webrtc::PeerConnectionInterface> m_conn;

    sequence<MediaStreamPtr> m_local_streams;
    sequence<MediaStreamPtr> m_remote_streams;

public:
bool Init(talk_base::scoped_refptr<webrtc::PeerConnectionFactoryInterface> pc_factory)
{
    webrtc::PeerConnectionInterface::IceServers servers;
    webrtc::PeerConnectionInterface::IceServer server;
    server.uri = kDefaultIceServer;
    servers.push_back(server);

    m_conn = pc_factory->CreatePeerConnection(servers, NULL, NULL, this);
    if (m_conn == NULL) {
        return false;
    }
    return true;
}
 
explicit CRTCPeerConnection ()
{
    m_conn = NULL;
}

virtual ~CRTCPeerConnection ()
{
    m_conn = NULL;
}

void * getptr() 
{
    return m_conn.get();
}

void setParams (RTCConfiguration *configuration, MediaConstraints *constraints)
{}

void createOffer (MediaConstraints *constraints)
{
    m_conn->CreateOffer(this, NULL);
}

void createAnswer (RTCSessionDescription &offer, MediaConstraints *constraints)
{
    m_conn->CreateAnswer(this, NULL);
}

void setLocalDescription (RTCSessionDescription &description)
{
    webrtc::SessionDescriptionInterface* desp(webrtc::CreateSessionDescription(description.type, description.sdp));
    if (desp) {
        m_conn->SetLocalDescription(DummySetSessionDescriptionObserver::Create(), desp);
    }
}

void setRemoteDescription (RTCSessionDescription &description)
{
    webrtc::SessionDescriptionInterface* desp(webrtc::CreateSessionDescription(description.type, description.sdp));
    if (desp) {
        m_conn->SetRemoteDescription(DummySetSessionDescriptionObserver::Create(), desp);
    }
}

void updateIce (RTCConfiguration *configuration, MediaConstraints *constraints)
{
}

void addIceCandidate (RTCIceCandidate &candidate)
{
}

sequence<MediaStreamPtr> & getLocalStreams ()
{
    return m_local_streams;
}

sequence<MediaStreamPtr> & getRemoteStreams ()
{
    return m_remote_streams;
}

MediaStreamPtr getStreamById (DOMString streamId)
{
    return NULL;
}

void addStream (MediaStreamPtr stream, MediaConstraints *constraints)
{}

void removeStream (MediaStreamPtr stream)
{}

void close ()
{}


///
/// for webrtc::PeerConnectionObserver
virtual void OnError() {
    event_process0(onerror);
}

// Triggered when the SignalingState changed.
virtual void OnSignalingChange(
        webrtc::PeerConnectionInterface::SignalingState new_state) {
    int state = (int)new_state;
    event_process1(onsignalingstatechange, state);
    m_signalingState = (RTCSignalingState)state;
}

// Triggered when SignalingState or IceState have changed.
// TODO(bemasc): Remove once callers transition to OnSignalingChange.
virtual void OnStateChange(webrtc::PeerConnectionObserver::StateType state_changed) {
}

// Triggered when media is received on a new stream from remote peer.
virtual void OnAddStream(webrtc::MediaStreamInterface* stream) {
    if (!stream)    return;
    event_process0(onaddstream);
    if (revent == EVENT_OK) {
        webrtc::MediaConstraintsInterface* constraints = NULL;
        m_conn->AddStream(stream, constraints);
    }
}

// Triggered when a remote peer close a stream.
virtual void OnRemoveStream(webrtc::MediaStreamInterface* stream) {
    if (!stream)    return;
    event_process0(onremovestream);
    if (revent == EVENT_OK)
        m_conn->RemoveStream(stream);
}

// Triggered when a remote peer open a data channel.
// TODO(perkj): Make pure virtual.
virtual void OnDataChannel(webrtc::DataChannelInterface* data_channel) {}

// Triggered when renegotation is needed, for example the ICE has restarted.
virtual void OnRenegotiationNeeded() {
    event_process0(onnegotiationneeded);
}

// Called any time the IceConnectionState changes
virtual void OnIceConnectionChange(
        webrtc::PeerConnectionInterface::IceConnectionState new_state) {
    int state = (int)new_state;
    m_iceConnectionState = (RTCIceConnectionState)state;
    event_process1(oniceconnectionstatechange, state);
}

// Called any time the IceGatheringState changes
virtual void OnIceGatheringChange(
        webrtc::PeerConnectionInterface::IceGatheringState new_state) {
    int state = (int)new_state;
    m_iceGatheringState = (RTCIceGatheringState)state;
}

// New Ice candidate have been found.
virtual void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) {
    if (!candidate)   return;
    event_process0(onicecandidate);
    if (revent == EVENT_OK)
        m_conn->AddIceCandidate(candidate);
}

// TODO(bemasc): Remove this once callers transition to OnIceGatheringChange.
// All Ice candidates have been found.
virtual void OnIceComplete() {}


///
/// for webrtc::CreateSessionDescriptionObserver
virtual void OnSuccess(webrtc::SessionDescriptionInterface* desc) {
    if (!desc)  return;
    RTCSessionDescription rtcDesc;
    rtcDesc.type = desc->type();

    std::string sdp;
    desc->ToString(&sdp);
    rtcDesc.sdp = sdp;

    const webrtc::SessionDescriptionInterface* ldesc = m_conn->local_description();
    if (ldesc) {
        ldesc->ToString(&sdp);
        m_localDescription.sdp = sdp;
        m_localDescription.type = ldesc->type();
    }

    const webrtc::SessionDescriptionInterface* rdesc = m_conn->local_description();
    if (rdesc) {
        rdesc->ToString(&sdp);
        m_remoteDescription.sdp = sdp;
        m_remoteDescription.type = rdesc->type();
    }

    event_process1(onsuccess, rtcDesc);
}

virtual void OnFailure(const std::string& error)
{
    event_process1(onfailure, error);
}

}; // class CRTCPeerConnection


RTCPeerConnection *CreatePeerConnection(talk_base::scoped_refptr<webrtc::PeerConnectionFactoryInterface> pc_factory) {
    talk_base::scoped_refptr<CRTCPeerConnection> pc = new talk_base::RefCountedObject<CRTCPeerConnection>();
    if (!pc.get() || !pc->Init(pc_factory)) {
        pc = NULL;
    }else {
        pc->AddRef();
    }
    return pc.get();
}


} //namespace xrtc
