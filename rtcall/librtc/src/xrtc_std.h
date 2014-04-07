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

struct DOMError {
    int errno;
};

class EventTarget {};

#define readonly_attribute(t, n)        public: t Get_##n() { return m_##n; }; protected: t m_##n;
#define readwrite_attribute(t, n)       public: void Put_##n(t value) { m_##n = value; }; readonly_attribute(t, n);
#define eventhandler_attribute(c, n)    public: void Put_EventHandler(c##EventHandler *handler) { m_p##n = handler; }; protected: c##EventHandler *m_p##n;



///
/// ==============================
const DOMString kMediaKind = "none";
const DOMString kAudioKind = "audio";
const DOMString kVideoKind = "video";

enum MediaStreamTrackState {
    TRACK_NEW,            //"new",
    TRACK_LIVE,           //"live",
    TRACK_ENDED,          //"ended"
};

enum VideoFacingModeEnum {
    FACING_USER,           //"user",
    FACING_ENVIRONMENT,    //"environment",
    FACING_LEFT,           //"left",
    FACING_RIGHT,          //"right"
};

enum SourceTypeEnum {
    SOURCE_NONE,           //"none",
    SOURCE_CAMERA,         //"camera",
    SOURCE_MICROPHONE,     //"microphone"
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
    explicit MediaStreamTrack() {reset();}
    virtual ~MediaStreamTrack();
    void reset() {
        m_kind = kMediaKind;
        m_id = "";
        m_label = "";
        m_enabled = false;
        m_muted = false;
        m_readonly = false;
        m_remote = false;
        m_readyState = TRACK_ENDED;
        m_pEventHandler = NULL;
    }

    static sequence<SourceInfo> & getSourceInfos();

    virtual MediaTrackConstraints   constraints ();
    virtual MediaSourceStates       states ();
    virtual AllMediaCapabilities    capabilities ();
    virtual void                    applyConstraints (MediaTrackConstraints &constraints);
    //virtual MediaStreamTrack        clone ();
    virtual void                    stop ();
};
typedef sequence<MediaStreamTrack> MediaStreamTrackSequence;


///
/// ==============================
class MediaStreamEventHandler {
public:
    virtual void onended()          {};
    virtual void onaddtrack()       {};
    virtual void onremovetrack()    {};
};

class MediaStream : public EventTarget {
    readonly_attribute (DOMString,    id);
    readonly_attribute (boolean,      ended);

    eventhandler_attribute (MediaStream, EventHandler);

public:
    explicit MediaStream () {reset();}
    virtual ~MediaStream ();
    void reset() {
        m_id = "";
        m_ended = false;
        m_pEventHandler = NULL;
    }

    virtual sequence<MediaStreamTrack> & getAudioTracks ();
    virtual sequence<MediaStreamTrack> & getVideoTracks ();
    virtual MediaStreamTrack         & getTrackById (DOMString trackId);
    virtual void                     addTrack (MediaStreamTrack &track);
    virtual void                     removeTrack (MediaStreamTrack &track);
    //virtual MediaStream              clone ();
};


///
/// ==============================
struct NavigatorUserMediaError : public DOMError {
    DOMString constraintName;
};

class NavigatorUserMediaCallback {
public:
    virtual void SuccessCallback(MediaStream &stream)           {};
    virtual void ErrorCallback(NavigatorUserMediaError &error)  {};
};

class NavigatorUserMedia {
public:
    static void getUserMedia (MediaStreamConstraints &constraints, NavigatorUserMediaCallback &callback);
};


///
/// ==============================
enum RTCSdpType {
    SDP_OFFER,      //"offer",
    SDP_PRANSWER,   //"pranswer",
    SDP_ANSWER,     //"answer"
};

struct RTCSessionDescription {
    RTCSdpType type;
    DOMString sdp;
};

struct RTCIceCandidate {
    DOMString      candidate;
    DOMString      sdpMid;
    unsigned short sdpMLineIndex;
};

struct RTCIceServer {
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
    SIGNALING_STABLE,       //"stable",
    HAVE_LOCAL_OFFER,       //"have-local-offer",
    HAVE_REMOTE_OFFER,      //"have-remote-offer",
    HAVE_LOCAL_PRANSWER,    //"have-local-pranswer",
    HAVE_REMOTE_PRANSWER,   //"have-remote-pranswer",
    SIGNALING_CLOSED,       //"closed"
};

enum RTCIceGatheringState {
    ICE_NEW,            //"new",
    ICE_GATHERING,      //"gathering",
    ICE_COMPLETE,       //"complete"
};

enum RTCIceConnectionState {
    CONN_NEW,       //"new",
    CHECKING,       //"checking",
    CONNECTED,      //"connected",
    COMPLETED,      //"completed",
    FAILED,         //"failed",
    DISCONNECTED,   //"disconnected",
    CONN_CLOSED,    //"closed"
};

class RTCPeerConnectionEventHandler {
public:
    virtual void onnegotiationneeded()      {}
    virtual void onicecandidate()           {}
    virtual void onsignalingstatechange()   {}
    virtual void onaddstream()              {}
    virtual void onremovestream()           {}
    virtual void oniceconnectionstatechange()   {}
};

class RTCSdpCallback {
public:
    virtual void SuccessCallback (RTCSessionDescription &sdp) {}
    virtual void FailureCallback (DOMError &error) {}
};

class RTCVoidCallback {
public:
    virtual void SuccessCallback () {}
    virtual void FailureCallback (DOMError &error) {}
};

class RTCPeerConnection : public EventTarget  {
    readonly_attribute (RTCSessionDescription,  localDescription);
    readonly_attribute (RTCSessionDescription,  remoteDescription);
    readonly_attribute (RTCSignalingState,      signalingState);
    readonly_attribute (RTCIceGatheringState,   iceGatheringState);
    readonly_attribute (RTCIceConnectionState,  iceConnectionState);

    eventhandler_attribute (RTCPeerConnection, EventHandler);

public:
    explicit RTCPeerConnection () {reset();}
    virtual ~RTCPeerConnection ();
    void reset() {
        //m_localDescription = "";
        //m_remoteDescription = "";
        m_signalingState = SIGNALING_STABLE;
        m_iceGatheringState = ICE_NEW;
        m_iceConnectionState = CONN_NEW;
        m_pEventHandler = NULL;
    }

    virtual void                  setParams (RTCConfiguration &configuration, MediaConstraints &constraints);
    virtual void                  createOffer (RTCSdpCallback &callback);
    virtual void                  createOffer (RTCSdpCallback &callback, MediaConstraints &constraints);
    virtual void                  createAnswer (RTCSdpCallback &callback);
    virtual void                  createAnswer (RTCSdpCallback &callback, MediaConstraints &constraints);

    virtual void                  setLocalDescription (RTCSessionDescription &description, RTCVoidCallback &callback);
    virtual void                  setRemoteDescription (RTCSessionDescription &description, RTCVoidCallback &callback);

    virtual void                  updateIce (RTCConfiguration &configuration);
    virtual void                  updateIce (RTCConfiguration &configuration, MediaConstraints &constraints);
    virtual void                  addIceCandidate (RTCIceCandidate &candidate, RTCVoidCallback &callback);

    virtual sequence<MediaStream> & getLocalStreams ();
    virtual sequence<MediaStream> & getRemoteStreams ();
    virtual MediaStream           & getStreamById (DOMString streamId);
    virtual void                  addStream (MediaStream &stream);
    virtual void                  addStream (MediaStream &stream, MediaConstraints &constraints);
    virtual void                  removeStream (MediaStream &stream);
    virtual void                  close ();
};

} // namespace xrtc


#endif // _XRTC_STD_H_

