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

#ifndef _XRTC_API_H_
#define _XRTC_API_H_

#include <string>

enum action_t {
    ADD_ACTION,
    REMOVE_ACTION,
};

enum color_t {
    UnknownFmt = 0,
    I420Fmt,
    RGB24Fmt,
    ARGB32Fmt,
};

enum rotation_t{
  ROTATION_0 = 0,
  ROTATION_90 = 90,
  ROTATION_180 = 180,
  ROTATION_270 = 270
};

typedef struct _video_frame {
    int width;
    int height;
    int color;    // refer to color_t
    int rotation; // refer to rotation_t
    unsigned long timestamp;
    int length; // length of video frame
    int size;   // size of data buffer
    unsigned char *data;
}video_frame_t;

class IRtcRender {
public:
    virtual ~IRtcRender() {}

    virtual void OnSize(int width, int height) = 0;
    virtual void OnFrame(const video_frame_t *frame) = 0;
};

class IRtcSink {
public:
    virtual ~IRtcSink() {}

    virtual void OnSessionDescription(const std::string &type, const std::string &sdp) = 0;
    virtual void OnIceCandidate(const std::string &candidate, const std::string &sdpMid, int sdpMLineIndex) = 0;
    //> action: refer to action_t
    virtual void OnRemoteStream(int action) = 0;
    virtual void OnGetUserMedia(int error, std::string errstr) = 0;
    virtual void OnFailureMesssage(std::string errstr) = 0;
};

class IRtcCenter {
public:
    virtual ~IRtcCenter() {}
    virtual void SetSink(IRtcSink *sink) = 0;

    virtual long GetUserMedia() = 0;
    virtual long CreatePeerConnection() = 0;
    virtual long AddLocalStream() = 0;

    //> action: refer to action_t
    virtual long SetLocalRender(IRtcRender *render, int action) = 0;
    virtual long SetRemoteRender(IRtcRender *render, int action) = 0;

    virtual long SetupCall() = 0;
    virtual long AnswerCall() = 0;
    virtual void Close() = 0;

    //> type: offer, pranswer, answer
    virtual long SetLocalDescription(const std::string &type, const std::string &sdp) = 0;
    virtual long SetRemoteDescription(const std::string &type, const std::string &sdp) = 0;
    virtual long AddIceCandidate(const std::string &candidate, const std::string &sdpMid, int sdpMLineIndex) = 0;
};

extern "C" {
bool        xrtc_init();
void        xrtc_uninit();
bool        xrtc_create(IRtcCenter * &rtc);
void        xrtc_destroy(IRtcCenter * rtc);
}

//> setup one call
/**
 * xrtc_init():                 create peer connection factory
 * xrtc_create():               create rtc center
 *                              call SetSink()
 *
 * GetUserMedia():              get local stream(audio/video track)
 * CreatePeerConnection():      create peer connection
 * AddLocalStream():            add local stream into peer connection
 * SetLocalRender(ADD):         add render to local stream
 *
 * SetupCall():                         create offer
 * IRtcSink::OnSessionDescription():    call SetLocalDescription() of offer
 *                                      app=>Send sdp(offer) to remote peer
 * IRtcSink::OnIceCandidate:            app=>send candidate to remote peer
 *
 * ......
 * =>Recv sdp(answer) from SIP:         call SetRemoteDescription() of answer
 * =>Recv candidate from SIP:           call AddIceCandidate()
 * IRtcSink::OnRemotetream():           call SetRemoteRender(ADD)
 *
 */

//> receive one call
/**
 * xrtc_init():                 create peer connection factory
 * xrtc_create():               create rtc center
 *                              call SetSink()
 *
 * ......
 * App=>Recv sdp(offer) from SIP:                
 * GetUserMedia():              get local stream(audio/video track)
 * CreatePeerConnection():      create peer connection
 * AddLocalStream():            add local stream into peer connection
 * SetLocalRender(ADD):         add render to local stream
 * SetRemoteDescription():      with received offer.
 *
 * AnswerCall():                        create answer
 * IRtcSink::OnSessionDescription():    call SetLocalDescription() of answer; 
 *                                      app=>Send sdp(answer) to remote peer over SIP.
 * IRtcSink::OnIceCandidate():          app=>Send candidate to remote peer over SIP.
 *
 * ......
 * app=>Recv candidate from SIP:        call AddIceCandidate(),
 * IRtcSink::OnRemoteStream(ADD):       call SetRemoteRender(ADD)
 *
 */

//> close one call
/**
 * Close():                 
 * SetLocalRender(REMOVE):
 * SetRemoteRender(REMOVE):
 * xrtc_destroy():
 * xrtc_uninit():
 *
 */

#endif // _XRTC_API_H_

