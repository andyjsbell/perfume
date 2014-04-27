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

#include "refcount.h"
#include "zeroptr.h"

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
    DOMString errstr;
};

class EventTarget : public ubase::RefCount {
public:
    virtual ~EventTarget() {}
    virtual void * getptr()         {return NULL;}
};

#define readonly_attribute(t, n)        protected: virtual void Put_##n(t value) { m_##n = value; } public: virtual t Get_##n() { return m_##n; } protected: t m_##n;
#define readwrite_attribute(t, n)       public:    virtual void Put_##n(t value) { m_##n = value; } public: virtual t Get_##n() { return m_##n; } protected: t m_##n;
#define eventhandler_attribute(c)       public: virtual void Put_EventHandler(c##EventHandler *handler) { m_pEventHandler = handler; } \
                                        protected: virtual c##EventHandler * Get_EventHandler() {return m_pEventHandler; } \
                                        protected: c##EventHandler *m_pEventHandler;
#define event_process0(f)               if(m_pEventHandler) { m_pEventHandler->f(); }
#define event_process1(f, a)            if(m_pEventHandler) { m_pEventHandler->f((a)); }
#define event_process2(f, a, b)         if(m_pEventHandler) { m_pEventHandler->f((a), (b)); }


enum media_t {
    XRTC_UNKNOWN,
    XRTC_AUDIO,
    XRTC_VIDEO,
};
const DOMString kUnknownKind = "unknown";
const DOMString kAudioKind = "audio";
const DOMString kVideoKind = "video";
//const DOMString kStreamLabel = "stream";


///
/// ==============================
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
    virtual void onmute()       {}
    virtual void onunmute()     {}
    virtual void onstarted()    {}
    virtual void onended()      {}
    virtual void onoverconstrained()    {}
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
    virtual ~MediaStreamTrack() {}
    void reset() {
        m_kind = kUnknownKind;
        m_id = "";
        m_label = "Unknown Track";
        m_enabled = false;
        m_muted = false;
        m_readonly = false;
        m_remote = false;
        m_readyState = TRACK_ENDED;
        m_pEventHandler = NULL;
    }

    virtual MediaTrackConstraints   constraints ()      = 0;
    virtual MediaSourceStates       states ()           = 0;
    virtual AllMediaCapabilities *  capabilities ()     = 0;
    virtual void                    applyConstraints (MediaTrackConstraints &constraints) {}
    //virtual MediaStreamTrack        clone () {}
    virtual void                    stop () {}
};
typedef ubase::zeroptr<MediaStreamTrack> MediaStreamTrackPtr;
//typedef sequence<MediaStreamTrackPtr> MediaStreamTrackSequence;

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
    virtual void onended()          {}
    virtual void onaddtrack()       {}
    virtual void onremovetrack()    {}
};

class MediaStream : public EventTarget {
    readonly_attribute (DOMString,    id);
    readonly_attribute (boolean,      ended);

    eventhandler_attribute (MediaStream);

public:
    explicit MediaStream () {reset();}
    virtual ~MediaStream () {}
    void reset() {
        m_id = "";
        m_ended = false;
        m_pEventHandler = NULL;
    }

    virtual sequence<MediaStreamTrackPtr> & getAudioTracks ()           = 0;
    virtual sequence<MediaStreamTrackPtr> & getVideoTracks ()           = 0;
    virtual MediaStreamTrackPtr      getTrackById (DOMString trackId)   = 0;
    virtual void                     addTrack (MediaStreamTrackPtr track) {}
    virtual void                     removeTrack (MediaStreamTrackPtr track) {}
    //virtual MediaStream              clone () {}
};
typedef ubase::zeroptr<MediaStream> MediaStreamPtr;


///
/// ==============================
struct NavigatorUserMediaError : public DOMError {
    DOMString constraintName;
};

class NavigatorUserMediaCallback {
public:
    virtual void SuccessCallback(MediaStreamPtr stream)         {};
    virtual void ErrorCallback(NavigatorUserMediaError &error)  {};
};

class NavigatorUserMedia {
public:
    static void getUserMedia (const MediaStreamConstraints & constraints, NavigatorUserMediaCallback *callback);
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

//
//> return EVENT_OK it will continue with defalut action
//> else stop at this callback
class RTCPeerConnectionEventHandler {
public:
    virtual void onnegotiationneeded()                  {}
    virtual void onicecandidate(RTCIceCandidate & ice)  {}
    virtual void onsignalingstatechange(int state)      {}
    virtual void onaddstream(MediaStreamPtr)            {}
    virtual void onremovestream(MediaStreamPtr)         {}
    virtual void oniceconnectionstatechange(int state)  {}

    virtual void onsuccess(RTCSessionDescription &desc) {}
    virtual void onfailure(const DOMString &error)      {}
    virtual void onerror()                              {}
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
    virtual ~RTCPeerConnection () {}
    void reset() {
        //m_localDescription = "";
        //m_remoteDescription = "";
        m_signalingState = SIGNALING_STABLE;
        m_iceGatheringState = ICE_NEW;
        m_iceConnectionState = CONN_NEW;
        m_pEventHandler = NULL;
    }

    virtual void setParams (const RTCConfiguration & configuration, const MediaConstraints & constraints) {}
    virtual void createOffer (const MediaConstraints & constraints) = 0;
    virtual void createAnswer (const MediaConstraints & constraints) = 0;
    virtual void setLocalDescription (const RTCSessionDescription & description) = 0;
    virtual void setRemoteDescription (const RTCSessionDescription & description) = 0;

    virtual void updateIce (const RTCConfiguration & configuration, const MediaConstraints & constraints) {}
    virtual void addIceCandidate (const RTCIceCandidate & candidate) = 0;

    virtual sequence<MediaStreamPtr> & getLocalStreams ()               = 0;
    virtual sequence<MediaStreamPtr> & getRemoteStreams ()              = 0;
    virtual MediaStreamPtr        getStreamById (DOMString streamId)    = 0;

    virtual void addStream (MediaStreamPtr stream, const MediaConstraints & constraints) = 0;
    virtual void removeStream (MediaStreamPtr stream) = 0;
    virtual void close () = 0;
};
typedef ubase::zeroptr<RTCPeerConnection> RTCPeerConnectionPtr;

} // namespace xrtc


#endif // _XRTC_STD_H_

