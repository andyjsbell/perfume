#ifndef _WEBRTC_H_
#define _WEBRTC_H_

#include "talk/base/common.h"
#include "talk/base/scoped_ptr.h"
#include "talk/base/physicalsocketserver.h"
#include "talk/media/base/mediaengine.h"
#include "talk/app/webrtc/mediastreaminterface.h"
#include "talk/app/webrtc/videosourceinterface.h"
#include "talk/app/webrtc/peerconnection.h"
#include "talk/app/webrtc/peerconnectioninterface.h"

#include "xrtc_std.h"

xrtc::RTCPeerConnection *CreatePeerConnection(talk_base::scoped_refptr<webrtc::PeerConnectionFactoryInterface> pc_factory);

#endif

