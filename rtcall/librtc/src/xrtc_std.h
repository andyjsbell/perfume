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

class EventTarget {
public:
    virtual ~EventTarget() {}
    virtual void * getptr()         {return NULL;}
};

#define readonly_attribute(t, n)        public: t Get_##n() { return m_##n; }; protected: t m_##n;
#define readwrite_attribute(t, n)       public: void Put_##n(t value) { m_##n = value; }; readonly_attribute(t, n);
#define eventhandler_attribute(c)       public: void Put_EventHandler(c##EventHandler *handler) { m_pEventHandler = handler; }; protected: c##EventHandler *m_pEventHandler;
#define event_process0(f)               event_t revent = EVENT_OK; if(m_pEventHandler) { revent = m_pEventHandler->f(); }
#define event_process1(f, a)            event_t revent = EVENT_OK; if(m_pEventHandler) { revent = m_pEventHandler->f((a)); }
#define event_process2(f, a, b)         event_t revent = EVENT_OK; if(m_pEventHandler) { revent = m_pEventHandler->f((a), (b)); }
enum event_t {
    EVENT_OK = 0,
    EVENT_DECLINE = 1,
};



///
/// ==============================
const DOMString kStreamLabel = "stream";

const DOMString kNullKind = "null";
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
    virtual event_t onmute()       {return EVENT_OK;}
    virtual event_t onunmute()     {return EVENT_OK;}
    virtual event_t onstarted()    {return EVENT_OK;}
    virtual event_t onended()      {return EVENT_OK;}
    virtual event_t onoverconstrained()    {return EVENT_OK;}
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

    eventhandler_attribute (MediaStreamTrack);

public:
    explicit MediaStreamTrack() {reset();}
    virtual ~MediaStreamTrack();
    void reset() {
        m_kind = kNullKind;
        m_id = "";
        m_label = "Unknown Track";
        m_enabled = false;
        m_muted = false;
        m_readonly = false;
        m_remote = false;
        m_readyState = TRACK_ENDED;
        m_pEventHandler = NULL;
    }

    virtual MediaTrackConstraints   constraints ();
    virtual MediaSourceStates       states ();
    virtual AllMediaCapabilities *  capabilities ();
    virtual void                    applyConstraints (MediaTrackConstraints &constraints);
    //virtual MediaStreamTrack        clone ();
    virtual void                    stop ();
};
typedef sequence<MediaStreamTrack> MediaStreamTrackSequence;

class VideoStreamTrack : public MediaStreamTrack {
public:
    static sequence<SourceInfo> & getSourceInfos();
};

class AudioStreamTrack : public MediaStreamTrack {
public:
    static sequence<SourceInfo> & getSourceInfos();
};


///
/// ==============================
class MediaStreamEventHandler {
public:
    virtual event_t onended()          {return EVENT_OK;}
    virtual event_t onaddtrack()       {return EVENT_OK;}
    virtual event_t onremovetrack()    {return EVENT_OK;}
};

class MediaStream : public EventTarget {
    readonly_attribute (DOMString,    id);
    readonly_attribute (boolean,      ended);

    eventhandler_attribute (MediaStream);

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
const DOMString kRTCSdpType[] = {
    "offer",
    "pranswer",
    "answer",
};

struct RTCSessionDescription {
    DOMString type; // kRTCSdpType
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
const DOMString kDefaultIceServer = "stun:stun.l.google.com:19302";

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
    virtual event_t onnegotiationneeded()                  {return EVENT_OK;}
    virtual event_t onicecandidate()                       {return EVENT_OK;}
    virtual event_t onsignalingstatechange(int state)      {return EVENT_OK;}
    virtual event_t onaddstream()                          {return EVENT_OK;}
    virtual event_t onremovestream()                       {return EVENT_OK;}
    virtual event_t oniceconnectionstatechange(int state)  {return EVENT_OK;}

    virtual event_t onsuccess(RTCSessionDescription &desc) {return EVENT_OK;}
    virtual event_t onfailure(const DOMString &error)      {return EVENT_OK;}
    virtual event_t onerror()                              {return EVENT_OK;}
};

class RTCPeerConnection : public EventTarget  {
    readonly_attribute (RTCSessionDescription,  localDescription);
    readonly_attribute (RTCSessionDescription,  remoteDescription);
    readonly_attribute (RTCSignalingState,      signalingState);
    readonly_attribute (RTCIceGatheringState,   iceGatheringState);
    readonly_attribute (RTCIceConnectionState,  iceConnectionState);

    eventhandler_attribute (RTCPeerConnection);

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

    virtual void                  setParams (RTCConfiguration *configuration, MediaConstraints *constraints);   ///@future

    virtual void                  createOffer (MediaConstraints *constraints);
    virtual void                  createAnswer (RTCSessionDescription &offer, MediaConstraints *constraints);
    virtual void                  setLocalDescription (RTCSessionDescription &description);
    virtual void                  setRemoteDescription (RTCSessionDescription &description);

    virtual void                  updateIce (RTCConfiguration *configuration, MediaConstraints *constraints);   ///@future
    virtual void                  addIceCandidate (RTCIceCandidate &candidate); ///@future

    virtual sequence<MediaStream> & getLocalStreams ();
    virtual sequence<MediaStream> & getRemoteStreams ();
    virtual MediaStream           & getStreamById (DOMString streamId);
    virtual void                  addStream (MediaStream &stream, MediaConstraints *constraints);   ///@future
    virtual void                  removeStream (MediaStream &stream);   ///@future
    virtual void                  close ();
};

} // namespace xrtc


#endif // _XRTC_STD_H_

