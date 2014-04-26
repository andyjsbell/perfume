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

enum video_render {
    LocalRender,
    RemoteRender,
};

typedef struct video_frame {
    int width;
    int height;
    int format;
    int size;
    unsigned char *data;
}video_frame_t;

class IRenderSink {
public:
    virtual void OnSize(int width, int height) = 0;
    virtual void OnFrame(const video_frame_t *frame) = 0;
};

class ICallSink {
public:
    virtual void OnOffer(const std::string &offer) = 0;
};

class IAnswerSink {
public:
    virtual void OnAnswer(const std::string &answer) = 0;
};


class IRtcCenter {
public:
    virtual ~IRtcCenter() {}

    virtual long GetUserMedia() = 0;
    virtual long CreatePeerConnection() = 0;
    virtual long SetupCall(ICallSink *sink) = 0;
    virtual long AddRender(IRenderSink *render) = 0;
    virtual long AnswerCall(IAnswerSink * sink) = 0;
};

extern "C" {
bool        xrtc_init();
void        xrtc_uninit();
bool        xrtc_create(IRtcCenter * &rtc);
void        xrtc_destroy(IRtcCenter * rtc);
}

//> setup one call
/**
 * xrtc_init:       create peer connection factory
 * xrtc_media:      get local stream(audio/video track)
 * xrtc_new_pc:     create peer connection
 * xrtc_call:       create offer
 * ---------------> App send offer to remote peer.
 *  ......
 * ---------------> App recv answer from remote peer.
 *
 */

//> receive one call
/**
 * ---------------> App recv offer from remote peer
 * xrtc_init:       To create peer connection factory
 * xrtc_new_pc:     To create peer connection
 * xrtc_media:      Get local stream(audio/video track)
 * xrtc_answer:     Set offer and callback of IAnswerSink::Answer()
 * ---------------> App send answer to remote peer.
 *
 */


#endif // _XRTC_API_H_

