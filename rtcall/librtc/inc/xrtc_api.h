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

enum video_format {
    UnknownFmt = 0,
    I420Fmt,
    RGB24Fmt,
    RGBA32Fmt,
};

typedef struct _video_frame {
    int width;
    int height;
    int format;
    int size;
    unsigned char *data;
}video_frame_t;

class IRtcRender {
public:
    virtual void OnSize(int width, int height) = 0;
    virtual void OnFrame(const video_frame_t *frame) = 0;
};

class IRtcSink {
public:
    virtual void OnSessionDescription(const std::string &type, const std::string &sdp) = 0;
    virtual void OnIceCandidate(const std::string &candidate, const std::string &sdpMid, int sdpMLineIndex) = 0;
    virtual void OnAddStream() = 0;
    virtual void OnRemoveStream() = 0;
};

class IRtcCenter {
public:
    virtual ~IRtcCenter() {}
    virtual void SetSink(IRtcSink *sink) = 0;

    virtual long GetUserMedia() = 0;
    virtual long CreatePeerConnection() = 0;

    virtual long AddLocalStream() = 0;
    virtual long AddLocalRender(IRtcRender *render) = 0;
    virtual long AddRemoteRender(IRtcRender *render) = 0;

    virtual long SetupCall() = 0;
    virtual long AnswerCall() = 0;

    //> type: offer, pranswer, answer
    virtual long SetLocalSdp(const std::string &type, const std::string &sdp) = 0;
    virtual long SetRemoteSdp(const std::string &type, const std::string &sdp) = 0;
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
 * xrtc_init:                   create peer connection factory
 * xrtc_create:                 create rtc center
 *
 * GetUserMedia:                get local stream(audio/video track)
 * CreatePeerConnection:        create peer connection
 *
 * AddLocalStream:              add local stream into peer connection
 * AddLocalRender:              add render to local stream
 *
 * SetupCall:                   create offer
 * IRtcSink::OnSessionSdp:      SetLocalSdp() of offer;
 *                              =>Send sdp(offer) to remote peer.
 * IRtcSink::OnIceCandidate:    =>send candidate to remote peer
 *
 * ......
 * =>Recv sdp(answer):          SetRemoteSdp() of answer,
 * =>Recv candidate:            AddIceCandidate(),
 *
 * IRtcSink::OnAddStream:       AddRemoteRender()
 *
 */

//> receive one call
/**
 * ---------------> App recv offer from remote peer
 * xrtc_init:                   create peer connection factory
 * xrtc_create:                 create rtc center
 *
 * ......
 * =>Recv sdp(offer):                
 * GetUserMedia:                get local stream(audio/video track)
 * CreatePeerConnection:        create peer connection
 * AddLocalStream:              add local stream into peer connection
 * AddLocalRender:              add render to local stream
 * SetRemoteSdp:                by received offer.
 *
 * AnswerCall:                  create answer
 * IRtcSink::OnSessionSdp:      SetLocalSdp() of answer; 
 *                              =>Send sdp(answer) to remote peer.
 * IRtcSink::OnIceCandidate:    =>Send candidate to remote peer
 *
 * ......
 * =>Recv candidate:            AddIceCandidate(),
 *
 * IRtcSink::OnAddStream:       AddRemoteRender()
 *
 */


#endif // _XRTC_API_H_

