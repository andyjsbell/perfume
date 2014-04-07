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

namespace xrtc {

RTCPeerConnection::RTCPeerConnection (RTCConfiguration &configuration)
{}

RTCPeerConnection::RTCPeerConnection (RTCConfiguration &configuration, MediaConstraints &constraints)
{}

void RTCPeerConnection::createOffer (RTCSdpCallback &callback)
{}

void RTCPeerConnection::createOffer (RTCSdpCallback &callback, MediaConstraints &constraints)
{}

void RTCPeerConnection::createAnswer (RTCSdpCallback &callback)
{}

void RTCPeerConnection::createAnswer (RTCSdpCallback &callback, MediaConstraints &constraints)
{}

void RTCPeerConnection::setLocalDescription (RTCSessionDescription &description, RTCVoidCallback &callback)
{}

void RTCPeerConnection::setRemoteDescription (RTCSessionDescription &description, RTCVoidCallback &callback)
{}

void RTCPeerConnection::updateIce (RTCConfiguration &configuration)
{}

void RTCPeerConnection::updateIce (RTCConfiguration &configuration, MediaConstraints &constraints)
{}

void RTCPeerConnection::addIceCandidate (RTCIceCandidate &candidate, RTCVoidCallback &callback)
{}

sequence<MediaStream> & RTCPeerConnection::getLocalStreams ()
{
    sequence<MediaStream> sms;
    return sms;
}

sequence<MediaStream> & RTCPeerConnection::getRemoteStreams ()
{
    sequence<MediaStream> sms;
    return sms;
}

MediaStream & RTCPeerConnection::getStreamById (DOMString streamId)
{
    MediaStream ms;
    return ms;
}

void RTCPeerConnection::addStream (MediaStream &stream)
{}

void RTCPeerConnection::addStream (MediaStream &stream, MediaConstraints &constraints)
{}

void RTCPeerConnection::removeStream (MediaStream &stream)
{}

void RTCPeerConnection::close ()
{}

} //namespace xrtc

