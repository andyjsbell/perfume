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

class CMediaStreamTrack : public MediaStreamTrack {
private:
    talk_base::scoped_refptr<webrtc::MediaStreamTrackInterface> m_track;
    talk_base::scoped_refptr<webrtc::MediaSourceInterface> m_source;

public:
bool Init(talk_base::scoped_refptr<webrtc::PeerConnectionFactoryInterface> pc_factory, DOMString vid)
{
    if (!pc_factory)    return false;
    if (m_kind == kAudioKind) {
        if (!m_source)
            m_source = pc_factory->CreateAudioSource(NULL);
        m_track = pc_factory->CreateAudioTrack(m_label, (webrtc::AudioSourceInterface *)(m_source.get()));
    }else if (m_kind == kVideoKind) {
        if (!m_source) {
            cricket::VideoCapturer* capturer = OpenVideoCaptureDevice(vid);
            if (capturer)
                m_source = pc_factory->CreateVideoSource(capturer, NULL);
        }
        if (m_source)
            m_track = pc_factory->CreateVideoTrack(m_label, (webrtc::VideoSourceInterface *)(m_source.get()));
    }
    return m_track != NULL;
}

explicit CMediaStreamTrack() 
{}

virtual ~CMediaStreamTrack()
{
    reset();
    m_source.release();
    m_track.release();
}

sequence<SourceInfo> & getSourceInfos()
{
    sequence<SourceInfo> ssi;
    return ssi;
}

MediaTrackConstraints constraints()
{
    MediaTrackConstraints cons;
    return cons;
}

MediaSourceStates states()
{
    MediaSourceStates stats;
    return stats;    
}

AllMediaCapabilities capabilities()
{
    AllMediaCapabilities caps;
    return caps;
}

void applyConstraints(MediaTrackConstraints &constraints)
{}

//MediaStreamTrack clone()
//{}

void stop()
{}

static cricket::VideoCapturer* OpenVideoCaptureDevice(DOMString dev_id) 
{
    talk_base::scoped_ptr<cricket::DeviceManagerInterface> dev_manager(
            cricket::DeviceManagerFactory::Create());
    if (!dev_manager->Init()) {
        return NULL;
    }

    std::vector<cricket::Device> devs;
    if (!dev_manager->GetVideoCaptureDevices(&devs)) {
        return NULL;
    }

    cricket::VideoCapturer* capturer = NULL;
    std::vector<cricket::Device>::iterator dev_it = devs.begin();
    for (; dev_it != devs.end(); ++dev_it) {
        std::string key = (*dev_it).id;
        if (!dev_id.empty() && dev_id != key) {
            continue;
        }
        capturer = dev_manager->CreateVideoCapturer(*dev_it);
        if (capturer != NULL)
            break;
    }

    // TODO: choose the best format

    return capturer;
}

static std::map<std::string, std::string> GetVideoDevices()
{
    std::string key;
    std::string val;
    std::map<std::string, std::string> devices;

    talk_base::scoped_ptr<cricket::DeviceManagerInterface> dev_manager(
            cricket::DeviceManagerFactory::Create());
    if (!dev_manager->Init()) {
        return devices;
    }

    std::vector<cricket::Device> devs;
    if (!dev_manager->GetVideoCaptureDevices(&devs)) {
        return devices;
    }

    std::vector<cricket::Device>::iterator dev_it = devs.begin();
    for (; dev_it != devs.end(); ++dev_it) {
        key = (*dev_it).id;
        val = (*dev_it).name;
        devices[key] = val;

#if 0
        std::string msg("Capture device [id = ");
        msg += key;
        msg += ", name = ";
        msg += (val + "]");
        printf("video devices: %s\n", msg.c_str());
#endif
    }

    return devices;
}

static std::vector<std::string> GetAudioDevices(bool bInput)
{
    std::vector<std::string> devices;
    std::vector<cricket::Device> devicelist;

    static bool initTried = false;
    static talk_base::scoped_ptr<cricket::DeviceManagerInterface> devmgr(cricket::DeviceManagerFactory::Create());

    if(false == initTried) {
        if(false == devmgr->Init()) {
            return devices;
        }
        initTried = true;
    }

    if(bInput)
        devmgr->GetAudioInputDevices(&devicelist);
    else
        devmgr->GetAudioOutputDevices(&devicelist);

    for(size_t i=0; i<devicelist.size(); i++) {
        devices.push_back(devicelist[i].name);
    }
    devicelist.clear();

    return devices;
}

};

} // namespace xrtc

