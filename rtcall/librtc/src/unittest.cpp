#include "xrtc_api.h"

#ifdef _TEST

int main(int argc, char *argv[]) {
    xrtc_init();

    pc_ptr_t pc = xrtc_new_pc();
    xrtc_del_pc(pc);
    
    xrtc_uninit();
    return 0;
}

#endif

