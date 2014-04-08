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

    MediaStreamTrack m_null_track;
    sequence<MediaStreamTrack> m_audio_tracks;
    sequence<MediaStreamTrack> m_video_tracks;
    

public:
bool Init(talk_base::scoped_refptr<webrtc::PeerConnectionFactoryInterface> pc_factory, const std::string label) {
    if (!pc_factory)    return false;
    talk_base::scoped_refptr<webrtc::MediaStreamInterface> stream = pc_factory->CreateLocalMediaStream(label);
    m_stream = stream;
    return (m_stream != NULL);
}

explicit CMediaStream ()
{}

explicit CMediaStream (CMediaStream &stream)
{}

explicit CMediaStream (MediaStreamTrackSequence &tracks)
{}

virtual ~CMediaStream()
{
    m_stream = NULL;
}

void * getptr() 
{
    return m_stream.get();
}

sequence<MediaStreamTrack> & getAudioTracks ()
{
    return m_audio_tracks;
}

sequence<MediaStreamTrack> & getVideoTracks ()
{
    return m_video_tracks;
}

MediaStreamTrack & getTrackById (DOMString trackId)
{
    sequence<MediaStreamTrack>::iterator iter;
    for(iter=m_audio_tracks.begin(); iter != m_audio_tracks.end(); iter++) {
        if(iter->Get_id() == trackId) {
            return (*iter);
        }
    }

    for(iter=m_video_tracks.begin(); iter != m_video_tracks.end(); iter++) {
        if(iter->Get_id() == trackId) {
            return (*iter);
        }
    }

    return m_null_track;
}

void addTrack (MediaStreamTrack &track)
{
    if (track.Get_kind() == kAudioKind) {
        m_stream->AddTrack((webrtc::AudioTrackInterface *)track.getptr());
    }else if (track.Get_kind() == kVideoKind) {
        m_stream->AddTrack((webrtc::VideoTrackInterface *)track.getptr());
    }
}

void removeTrack (MediaStreamTrack &track)
{
    if (track.Get_kind() == kAudioKind) {
        m_stream->RemoveTrack((webrtc::AudioTrackInterface *)track.getptr());
    }else if (track.Get_kind() == kVideoKind) {
        m_stream->RemoveTrack((webrtc::VideoTrackInterface *)track.getptr());
    }
}

//MediaStream              clone ()
//{}

}; //class CMediaStream


MediaStream * CreateMediaStream(talk_base::scoped_refptr<webrtc::PeerConnectionFactoryInterface> pc_factory) {
    CMediaStream * stream = new CMediaStream();
    if (!stream)    return NULL;
    if (!stream->Init(pc_factory, kStreamLabel)) {
        delete stream;
        stream = NULL;
    }
    return stream;
}

} // namespace xrtc

