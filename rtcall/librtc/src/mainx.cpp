#include "xrtc_api.h"
#include "xrtc_std.h"
#include "webrtc.h"

static talk_base::scoped_refptr<webrtc::PeerConnectionFactoryInterface> _pc_factory = NULL;

bool xrtc_init()
{
    _pc_factory = webrtc::CreatePeerConnectionFactory();
    if (_pc_factory == NULL) {
        return false;
    }
    return true;
}

void xrtc_uninit()
{
    _pc_factory = NULL;
}

pc_ptr_t xrtc_pc_create()
{
    return (pc_ptr_t) CreatePeerConnection(_pc_factory);
}

void xrtc_pc_destroy(pc_ptr_t pc)
{
    if (pc) {
        xrtc::RTCPeerConnection *_pc = (xrtc::RTCPeerConnection *) pc;
        delete _pc;
    }
}

