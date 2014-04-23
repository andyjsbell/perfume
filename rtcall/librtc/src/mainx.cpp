#include "xrtc_api.h"
#include "xrtc_std.h"
#include "webrtc.h"
#include "error.h"

talk_base::scoped_refptr<webrtc::PeerConnectionFactoryInterface> _pc_factory = NULL;
ubase::zeroptr<xrtc::MediaStream> _local_stream = NULL;

bool xrtc_init()
{
    _pc_factory = webrtc::CreatePeerConnectionFactory();
    returnb_assert (_pc_factory.get());
    return true;
}

void xrtc_uninit()
{
    _pc_factory = NULL;
}

pc_ptr_t xrtc_new_pc()
{
    ubase::zeroptr<xrtc::RTCPeerConnection> pc = xrtc::CreatePeerConnection(_pc_factory);
    pc->AddRef();
    return (pc_ptr_t)pc.get();
}

void xrtc_del_pc(pc_ptr_t pc)
{
    if (pc) {
        xrtc::RTCPeerConnection *_pc = (xrtc::RTCPeerConnection *) pc;
        delete _pc;
    }
}

long xrtc_media()
{
    xrtc::MediaStreamConstraints constraints;
    constraints.audio = true;
    constraints.video = true;
    GetUserMedia(constraints);
    return UBASE_S_OK;
}

long xrtc_call(pc_ptr_t pc)
{
    returnv_assert (pc, UBASE_E_INVALIDPTR);
    xrtc::RTCPeerConnection *_pc = (xrtc::RTCPeerConnection *) pc;
    xrtc::MediaConstraints constraints;
    _pc->createOffer(constraints);   
    return UBASE_S_OK;
}

long xrtc_answer(pc_ptr_t pc)
{
    return UBASE_S_OK;
}
