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

class CRTCPeerConnection : public RTCPeerConnection {
public:
bool Init(talk_base::scoped_refptr<webrtc::PeerConnectionFactoryInterface> pc_factory)
{
}
 
explicit CRTCPeerConnection ()
{}

virtual ~CRTCPeerConnection ()
{}

void setParams (RTCConfiguration &configuration, MediaConstraints &constraints)
{}

void createOffer (RTCSdpCallback &callback)
{}

void createOffer (RTCSdpCallback &callback, MediaConstraints &constraints)
{}

void createAnswer (RTCSdpCallback &callback)
{}

void createAnswer (RTCSdpCallback &callback, MediaConstraints &constraints)
{}

void setLocalDescription (RTCSessionDescription &description, RTCVoidCallback &callback)
{}

void setRemoteDescription (RTCSessionDescription &description, RTCVoidCallback &callback)
{}

void updateIce (RTCConfiguration &configuration)
{}

void updateIce (RTCConfiguration &configuration, MediaConstraints &constraints)
{}

void addIceCandidate (RTCIceCandidate &candidate, RTCVoidCallback &callback)
{}

sequence<MediaStream> & getLocalStreams ()
{
    sequence<MediaStream> sms;
    return sms;
}

sequence<MediaStream> & getRemoteStreams ()
{
    sequence<MediaStream> sms;
    return sms;
}

MediaStream & getStreamById (DOMString streamId)
{
    MediaStream ms;
    return ms;
}

void addStream (MediaStream &stream)
{}

void addStream (MediaStream &stream, MediaConstraints &constraints)
{}

void removeStream (MediaStream &stream)
{}

void close ()
{}

};

RTCPeerConnection *CreatePeerConnection(talk_base::scoped_refptr<webrtc::PeerConnectionFactoryInterface> pc_factory) {
    CRTCPeerConnection * pc = new CRTCPeerConnection();
    if (!pc || !pc->Init(pc_factory)) {
        delete pc;
        pc = NULL;
    }
    return pc;
}


} //namespace xrtc
