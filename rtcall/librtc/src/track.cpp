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

static sequence<SourceInfo> _audio_sources;
static sequence<SourceInfo> _video_sources;

class CMediaStreamTrack : public MediaStreamTrack {
private:
    talk_base::scoped_refptr<webrtc::MediaStreamTrackInterface> m_track;
    talk_base::scoped_refptr<webrtc::MediaSourceInterface> m_source;

    MediaTrackConstraints m_constraints;
    MediaSourceStates m_source_states;
    AllMediaCapabilities *m_capabilities;

public:
bool Init(talk_base::scoped_refptr<webrtc::PeerConnectionFactoryInterface> pc_factory)
{
    if (!pc_factory)    return false;
    if (m_kind == kAudioKind) {
        if (!m_source)
            m_source = pc_factory->CreateAudioSource(NULL);
        m_track = pc_factory->CreateAudioTrack(m_label, (webrtc::AudioSourceInterface *)(m_source.get()));
    }else if (m_kind == kVideoKind) {
        if (!m_source) {
            std::string vname = "";
            sequence<MediaTrackConstraint>::iterator iter;
            for (iter=m_constraints.optional.begin(); iter != m_constraints.optional.end(); iter++) {
                if (iter->first == "sourceId") {
                    sequence<SourceInfo>::iterator iter2;
                    for (iter2=_video_sources.begin(); iter2 != _video_sources.end(); iter2++) {
                        if (iter2->sourceId == iter->second) {
                            vname = iter2->label;
                            break;
                        }
                    }
                    break;
                }
            }

            // if vname empty, select default device
            cricket::VideoCapturer* capturer = OpenVideoCaptureDevice(vname);
            if (capturer)
                m_source = pc_factory->CreateVideoSource(capturer, NULL);
        }
        if (m_source)
            m_track = pc_factory->CreateVideoTrack(m_label, (webrtc::VideoSourceInterface *)(m_source.get()));
    }
    return (m_track != NULL);
}

explicit CMediaStreamTrack(std::string kind, std::string id, std::string label, MediaTrackConstraints *constraints)
{
    m_kind = kind;
    m_id = id;
    m_label = label;

    if (constraints) {
        m_constraints.mandatory = constraints->mandatory;
        m_constraints.optional = constraints->optional;
    }
}

virtual ~CMediaStreamTrack()
{
    reset();
    m_source.release();
    m_track.release();
}

void * getptr()
{
    return m_track.get();
}

MediaTrackConstraints constraints()
{
    return m_constraints;
}

MediaSourceStates states()
{
    return m_source_states;
}

AllMediaCapabilities * capabilities()
{
    return m_capabilities;
}

void applyConstraints(MediaTrackConstraints &constraints)
{
    m_constraints = constraints;
}

//MediaStreamTrack clone()
//{}

void stop()
{}


///
/// for device
static cricket::VideoCapturer* OpenVideoCaptureDevice(std::string name)
{
    talk_base::scoped_ptr<cricket::DeviceManagerInterface> dev_manager(
            cricket::DeviceManagerFactory::Create());
    if (!dev_manager->Init()) {
        return NULL;
    }

    cricket::Device device;
    if(!dev_manager->GetVideoCaptureDevice(name, &device)) {
        return NULL;
    }

    cricket::VideoCapturer* capturer = NULL;
    capturer = dev_manager->CreateVideoCapturer(device);

    // TODO: choose the best format
    return capturer;
}

}; //class CMediaStreamTrack


static bool GetSourceInfos(const std::string kind, sequence<SourceInfo> &sources) {
    talk_base::scoped_ptr<cricket::DeviceManagerInterface> dev_manager(
            cricket::DeviceManagerFactory::Create());
    if (!dev_manager->Init()) {
        return false;
    }

    std::vector<cricket::Device> devs;
    if (kind == kVideoKind) {
        dev_manager->GetVideoCaptureDevices(&devs);
    }else if (kind == kAudioKind) {
        // dev_manager->GetAudioOutputDevices(&devs);
        dev_manager->GetAudioInputDevices(&devs);
    }else {
        return false;
    }

    SourceInfo source;
    source.kind = kind;
    std::vector<cricket::Device>::iterator iter;
    for (iter=devs.begin(); iter != devs.end(); ++iter) {
        source.sourceId = (*iter).id;
        source.label = (*iter).name;
        sources.push_back(source);
    }
    devs.clear();

    return true;
}

sequence<SourceInfo> & VideoStreamTrack::getSourceInfos() {
    _video_sources.clear();
    GetSourceInfos(kVideoKind, _video_sources);
    return _video_sources;
}

sequence<SourceInfo> & AudioStreamTrack::getSourceInfos() {
    _audio_sources.clear();
    GetSourceInfos(kAudioKind, _audio_sources);
    return _audio_sources;
}

static MediaStreamTrack * CreateMediaStreamTrack(talk_base::scoped_refptr<webrtc::PeerConnectionFactoryInterface> pc_factory, 
        std::string kind, std::string id, std::string label, MediaTrackConstraints *constraints) {
    CMediaStreamTrack *track = new CMediaStreamTrack(kind, id, label, constraints);
    if (!track->Init(pc_factory)) {
        delete track;
        track = NULL;
    }
    return track;
}

MediaStreamTrack * CreateAudioStreamTrack(talk_base::scoped_refptr<webrtc::PeerConnectionFactoryInterface> pc_factory, 
        MediaTrackConstraints *constraints) {
    std::string id = "audio_unqieu_id";
    std::string label = "Audio Stream Track";
    return CreateMediaStreamTrack(pc_factory, kAudioKind, id, label, constraints);
}

MediaStreamTrack * CreateVideoStreamTrack(talk_base::scoped_refptr<webrtc::PeerConnectionFactoryInterface> pc_factory,
        MediaTrackConstraints *constraints) {
    std::string id = "video_unqieu_id";
    std::string label = "Video Stream Track";
    return CreateMediaStreamTrack(pc_factory, kVideoKind, id, label, constraints);
}

} // namespace xrtc

