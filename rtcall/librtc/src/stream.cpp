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

    sequence<MediaStreamTrackPtr> m_audio_tracks;
    sequence<MediaStreamTrackPtr> m_video_tracks;

public:
bool Init(talk_base::scoped_refptr<webrtc::PeerConnectionFactoryInterface> pc_factory, 
        const std::string label, 
        talk_base::scoped_refptr<webrtc::MediaStreamInterface> stream) {
    m_stream = stream;
    if (!m_stream) {
        if (pc_factory) {
            m_stream = pc_factory->CreateLocalMediaStream(label);
        }
    }
    return (m_stream != NULL);
}

explicit CMediaStream (std::string id)
{
    m_id = id;
    m_stream = NULL;
}

virtual ~CMediaStream()
{
    m_stream = NULL;
    m_audio_tracks.clear();
    m_video_tracks.clear();
}

void * getptr() 
{
    return m_stream.get();
}

sequence<MediaStreamTrackPtr> & getAudioTracks ()
{
    return m_audio_tracks;
}

sequence<MediaStreamTrackPtr> & getVideoTracks ()
{
    return m_video_tracks;
}

MediaStreamTrackPtr getTrackById (DOMString trackId)
{
    sequence<MediaStreamTrackPtr>::iterator iter;
    for(iter=m_audio_tracks.begin(); iter != m_audio_tracks.end(); iter++) {
        if((*iter)->Get_id() == trackId) {
            return (*iter);
        }
    }

    for(iter=m_video_tracks.begin(); iter != m_video_tracks.end(); iter++) {
        if((*iter)->Get_id() == trackId) {
            return (*iter);
        }
    }

    return NULL;
}

void addTrack (MediaStreamTrackPtr track)
{
    if (m_stream != NULL && track != NULL) {
        if (track->Get_kind() == kAudioKind) {
            m_stream->AddTrack((webrtc::AudioTrackInterface *)track->getptr());
        }else if (track->Get_kind() == kVideoKind) {
            m_stream->AddTrack((webrtc::VideoTrackInterface *)track->getptr());
        }
    }
}

void removeTrack (MediaStreamTrackPtr track)
{
    if (m_stream != NULL && track != NULL) {
        if (track->Get_kind() == kAudioKind) {
            m_stream->RemoveTrack((webrtc::AudioTrackInterface *)track->getptr());
        }else if (track->Get_kind() == kVideoKind) {
            m_stream->RemoveTrack((webrtc::VideoTrackInterface *)track->getptr());
        }
    }
}

//MediaStream              clone ()
//{}

}; //class CMediaStream


MediaStream * CreateMediaStream(
        const std::string label,
        talk_base::scoped_refptr<webrtc::PeerConnectionFactoryInterface> pc_factory, 
        talk_base::scoped_refptr<webrtc::MediaStreamInterface> pstream) {
    std::string id = "stream_unique_id";
    CMediaStream * stream = new CMediaStream(id);
    if (!stream->Init(pc_factory, label, pstream)) {
        delete stream;
        stream = NULL;
    }
    return stream;
}

} // namespace xrtc

