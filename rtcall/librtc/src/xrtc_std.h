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

#ifndef _XRTC_STD_H_
#define _XRTC_STD_H_

#include <string>
#include <vector>
#include <map>


namespace xrtc {


typedef bool boolean;
typedef std::string DOMString;
#ifdef sequence
#undef sequence
#endif
#define sequence std::vector

typedef sequence<DOMString> CapabilityList;
typedef std::map<DOMString, DOMString> MediaTrackConstraintSet;
typedef std::pair<DOMString, DOMString> MediaTrackConstraint;
typedef std::map<DOMString, DOMString> MediaConstraints;


struct MediaTrackConstraints {
    MediaTrackConstraintSet mandatory;
    sequence<MediaTrackConstraint> optional;
};

struct MediaStreamConstraints {
    boolean video;
    MediaTrackConstraints videoConstraints;
    boolean audio;
    MediaTrackConstraints audioConstraints;
};

enum MediaStreamTrackState {
    NEW,            //"new",
    LIVE,           //"live",
    ENDED,          //"ended"
};

struct DOMError {
    int errno;
};

class EventTarget {};

#define readonly_attribute(t, n)        public: t Get_##n() { return m_##n; }; private: t m_##n;
#define readwrite_attribute(t, n)       public: void Put_##n(t value) { m_##n = value; }; readonly_attribute(t, n);
#define eventhandler_attribute(c, n)    public: void Put_EventHandler(c##EventHandler *handler) { m_p##n = handler; }; private: c##EventHandler *m_p##n;



///
/// ==============================
enum VideoFacingModeEnum {
    USER,           //"user",
    ENVIRONMENT,    //"environment",
    LEFT,           //"left",
    RIGHT,          //"right"
};

enum SourceTypeEnum {
    NONE,           //"none",
    CAMERA,         //"camera",
    MICROPHONE,     //"microphone"
};

struct SourceInfo {
    DOMString sourceId;
    DOMString kind;
    DOMString label;
};

struct MediaSourceStates {
    SourceTypeEnum      sourceType;
    DOMString           sourceId;
    unsigned long       width;
    unsigned long       height;
    float               frameRate;
    float               aspectRatio;
    VideoFacingModeEnum facingMode;
    unsigned long       volume;
};

struct CapabilityRange {
    float     max;
    float     min;
    boolean supported;
};

struct AllMediaCapabilities{
    DOMString mediaType; // "none", "audio", "video", or "text"
};

struct AllVideoCapabilities : public AllMediaCapabilities {
    CapabilityList  sourceType;
    CapabilityList  sourceId;
    CapabilityRange width;
    CapabilityRange height;
    CapabilityRange frameRate;
    CapabilityRange aspectRatio;
    CapabilityList  facingMode;
};

struct AllAudioCapabilities : public AllMediaCapabilities {
    CapabilityRange volume;
};

class MediaStreamTrackEventHandler {
public:
    virtual void onmute()       {};
    virtual void onunmute()     {};
    virtual void onstarted()    {};
    virtual void onended()      {};
    virtual void onoverconstrained()    {};
};

class MediaStreamTrack : public EventTarget {
    readonly_attribute (DOMString,             kind);
    readonly_attribute (DOMString,             id);
    readonly_attribute (DOMString,             label);
    readwrite_attribute (boolean,              enabled);
    readonly_attribute (boolean,               muted);
    readonly_attribute (boolean,               readonly);
    readonly_attribute (boolean,               remote);
    readonly_attribute (MediaStreamTrackState, readyState);

    eventhandler_attribute (MediaStreamTrack, EventHandler);

public:
    static sequence<SourceInfo> & getSourceInfos();

    MediaTrackConstraints   constraints ();
    MediaSourceStates       states ();
    AllMediaCapabilities &  capabilities ();
    void                    applyConstraints (MediaTrackConstraints constraints);
    MediaStreamTrack        clone ();
    void                    stop ();
};
typedef sequence<MediaStreamTrack> MediaStreamTrackSequence;


///
/// ==============================
class MediaStreamEventHandler {
public:
    virtual void onended()         {};
    virtual void onaddtrack()       {};
    virtual void onremovetrack()    {};
};

class MediaStream : public EventTarget {
    readonly_attribute (DOMString,    id);
    readonly_attribute (boolean,      ended);

    eventhandler_attribute (MediaStream, EventHandler);

public:
    MediaStream ();
    MediaStream (MediaStream &stream);
    MediaStream (MediaStreamTrackSequence &tracks);

    sequence<MediaStreamTrack> & getAudioTracks ();
    sequence<MediaStreamTrack> & getVideoTracks ();
    MediaStreamTrack         & getTrackById (DOMString &trackId);
    void                     addTrack (MediaStreamTrack &track);
    void                     removeTrack (MediaStreamTrack &track);
    MediaStream              clone ();
};


///
/// ==============================
struct NavigatorUserMediaError : public DOMError {
    DOMString constraintName;
};

class NavigatorUserMediaCallback {
public:
    virtual void SuccessCallback(MediaStream &stream)           {};
    virtual void ErrorCallback(NavigatorUserMediaError error)   {};
};

class NavigatorUserMedia {
public:
    void getUserMedia (MediaStreamConstraints &constraints, NavigatorUserMediaCallback &callback);
};


///
/// ==============================
enum RTCSdpType {
    OFFER,      //"offer",
    PRANSWER,   //"pranswer",
    ANSWER,     //"answer"
};

struct RTCSessionDescriptionInit {
    RTCSdpType type;
    DOMString  sdp;
};

class RTCSessionDescription {
    readwrite_attribute (RTCSdpType,    type);
    readwrite_attribute (DOMString,     sdp);

public:
    RTCSessionDescription();
    RTCSessionDescription(RTCSessionDescriptionInit descriptionInitDict);
};

class RTCIceCandidateInit {
    DOMString      candidate;
    DOMString      sdpMid;
    unsigned short sdpMLineIndex;
};

class RTCIceCandidate {
    readwrite_attribute (DOMString,      candidate);
    readwrite_attribute (DOMString,      sdpMid);
    readwrite_attribute (unsigned short, sdpMLineIndex);

public:
    RTCIceCandidate();
    RTCIceCandidate(RTCIceCandidateInit candidateInitDict);
};

class RTCIceServer {
    sequence<DOMString>     urls;
    DOMString               username;
    DOMString               credential;
};

struct RTCConfiguration {
    sequence<RTCIceServer> iceServers;
};

struct RTCSdpError : public DOMError {
    long sdpLineNumber;
};


///
/// ==============================
enum RTCSignalingState {
    STABLE,                 //"stable",
    HAVE_LOCAL_OFFER,       //"have-local-offer",
    HAVE_REMOTE_OFFER,      //"have-remote-offer",
    HAVE_LOCAL_PRANSWER,    //"have-local-pranswer",
    HAVE_REMOTE_PRANSWER,   //"have-remote-pranswer",
    SS_CLOSED,              //"closed"
};

enum RTCIceGatheringState {
    IGS_ICE,        //"new",
    GATHERING,      //"gathering",
    COMPLETE,       //"complete"
};

enum RTCIceConnectionState {
    ICS_NEW,        //"new",
    CHECKING,       //"checking",
    CONNECTED,      //"connected",
    COMPLETED,      //"completed",
    FAILED,         //"failed",
    DISCONNECTED,   //"disconnected",
    ICS_CLOSED,     //"closed"
};

class RTCPeerConnectionEventHandler {
public:
    virtual void onnegotiationneeded();
    virtual void onicecandidate();
    virtual void onsignalingstatechange();
    virtual void onaddstream();
    virtual void onremovestream();
    virtual void oniceconnectionstatechange();
};

class RTCSdpCallback {
public:
    virtual void SuccessCallback (RTCSessionDescription sdp) {}
    virtual void FailureCallback (DOMError error) {}
};

class RTCVoidCallback {
public:
    virtual void SuccessCallback () {}
    virtual void FailureCallback (DOMError error) {}
};

class RTCPeerConnection : public EventTarget  {
    readonly_attribute (RTCSessionDescription,  localDescription);
    readonly_attribute (RTCSessionDescription,  remoteDescription);
    readonly_attribute (RTCSignalingState,      signalingState);
    readonly_attribute (RTCIceGatheringState,   iceGatheringState);
    readonly_attribute (RTCIceConnectionState,  iceConnectionState);

    eventhandler_attribute (RTCPeerConnection, EventHandler);

public:
    RTCPeerConnection (RTCConfiguration configuration);
    RTCPeerConnection (RTCConfiguration configuration, MediaConstraints constraints);

    void                  createOffer (RTCSdpCallback &callback);
    void                  createOffer (RTCSdpCallback &callback, MediaConstraints constraints);
    void                  createAnswer (RTCSdpCallback &callback);
    void                  createAnswer (RTCSdpCallback &callback, MediaConstraints constraints);

    void                  setLocalDescription (RTCSessionDescription description, RTCVoidCallback &callback);
    void                  setRemoteDescription (RTCSessionDescription description, RTCVoidCallback &callback);

    void                  updateIce (RTCConfiguration configuration);
    void                  updateIce (RTCConfiguration configuration, MediaConstraints constraints);
    void                  addIceCandidate (RTCIceCandidate candidate, RTCVoidCallback &callback);

    sequence<MediaStream> getLocalStreams ();
    sequence<MediaStream> getRemoteStreams ();
    MediaStream           getStreamById (DOMString streamId);
    void                  addStream (MediaStream stream);
    void                  addStream (MediaStream stream, MediaConstraints constraints);
    void                  removeStream (MediaStream stream);
    void                  close ();
};


} // namespace xrtc


#endif // _XRTC_STD_H_

