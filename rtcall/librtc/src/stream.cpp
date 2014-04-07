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

class CMediaStream : public MediaStream {
private:
    talk_base::scoped_refptr<webrtc::MediaStreamInterface> m_stream;

public:
explicit CMediaStream ()
{}

explicit CMediaStream (CMediaStream &stream)
{}

explicit CMediaStream (MediaStreamTrackSequence &tracks)
{}

sequence<MediaStreamTrack> & getAudioTracks ()
{
    sequence<MediaStreamTrack> smt;
    return smt;
}

sequence<MediaStreamTrack> & getVideoTracks ()
{
    sequence<MediaStreamTrack> smt;
    return smt;
}

MediaStreamTrack & getTrackById (DOMString trackId)
{
    MediaStreamTrack st;
    return st;
}

void addTrack (MediaStreamTrack &track)
{}

void removeTrack (MediaStreamTrack &track)
{}

//MediaStream              clone ()
//{}

};

} // namespace xrtc

