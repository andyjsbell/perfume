#!/bin/sh
# author: peterxu
#

## build config
ROOT=`pwd`
PLATFORM=`uname`
BUILDTYPE=Release
WEBRTC_URL=http://webrtc.googlecode.com/svn/trunk
WEBRTC_REV=5805
AR=ar
CC=gcc


echox() {
    if [ $# -ge 2 ]; then
        color=$1; shift; echo -e "\033[${color}m$*\033[00m"; 
    fi
}
echor() { echox 31 "$*"; }
echog() { echox 32 "$*"; }
echoy() { echox 33 "$*"; }
echob() { echox 34 "$*"; }
check_err() {
    if [ $? != 0 ]; then
        echor "[*] Error and exit!!!, reason="$1
        exit 1
    fi
}


detect_env() {
    if [ $PLATFORM != "Linux" ] && [ $PLATFORM != "Darwin" ] ; then
        exit 1
    fi

    which git >/dev/null
    check_err "git is required and was not found in the path!"

    which gclient >/dev/null
    check_err "'gclient' is required and was not found in the path!"

    which cmake >/dev/null
    check_err "'cmake' is required and was not found."
}


config_webrtc() {
    obj_path=$ROOT/third_party/webrtc
    echoy "Getting webrtc from its repo into $obj_path ..."
    mkdir -p $obj_path
    if [ ! -e $obj_path/trunk ]; then
        cd $obj_path
        gclient config --name=trunk $WEBRTC_URL
        gclient sync -r $WEBRTC_REV --force >/tmp/svn_webrtc.log 2>&1
        check_err "fail to gclient sync for webrtc!"
    fi
}


build_jingle() {
    obj_path=$ROOT/third_party/webrtc

    echoy "Do gyp_chromium on libjingle_all.gyp from $obj_path ..."
    cd $obj_path
    python trunk/webrtc/build/gyp_chromium --depth=trunk trunk/talk/libjingle.gyp
    check_err "fail to gyp_chromium for libjingle_all.gyp"

    # for jingle
    targets="libjingle libjingle_p2p libjingle_media libjingle_peerconnection libjingle_sound libjingle_xmpphelp peerconnection_server "

    # for webrtc misc
    targets=$targets"bitrate_controller remote_bitrate_estimator rtp_rtcp udp_transport webrtc_utility system_wrappers "
    targets=$targets"libjpeg libsrtp protobuf_full_do_not_use protobuf_lite protoc libyuv "

    # for webrtc audio
    targets=$targets"resampler signal_processing vad CNG "
    targets=$targets"G711 G722 NetEq NetEq4 PCM16B iLBC iSAC iSACFix opus "
    targets=$targets"audio_coding_module audio_conference_mixer audio_device audio_processing "
    targets=$targets"voice_engine_core media_file paced_sender audio_processing_sse2 webrtc_opus "

    # for webrtc video
    targets=$targets"common_video video_capture_module webrtc_vp8 video_coding_utility video_processing video_processing_sse2 "
    targets=$targets"webrtc_video_coding video_engine_core video_render_module webrtc_i420 "
    targets=$targets"gen_asm_offsets_vp8 gen_asm_offsets_vp9 gen_asm_offsets_vpx_scale "
    targets=$targets"libvpx libvpx_asm_offsets libvpx_asm_offsets_vp9 libvpx_asm_offsets_vpx_scale "

    cd $obj_path/trunk
    for target in $targets; do
        if [ $PLATFORM = "Darwin" ]; then
            xcodebuild -arch i386 -sdk macosx10.7 -configuration $BUILDTYPE -project talk/libjingle_all.xcodeproj -target=$target GCC_ENABLE_CPP_RTTI=YES
        else
            make BUILDTYPE=$BUILDTYPE $target 2>>/tmp/jingle_build.log 1>&2
        fi
        check_err "fail to build [$target]"
    done
}


make_archive () {
    target=$1
    if [ $PLATFORM = "Linux" ]; then
        rm -rf tmpobj; mkdir tmpobj; cd tmpobj
        for lib in $thelibs; do
            lib=../$lib
            if [ ! -e $lib ]; then
                echor "Can not find $lib!"; continue
            fi
            #echo "Processing $k ..."
            #ar t $k | xargs ar qc lib$target.a
            $AR x $lib
            $AR t $lib | xargs $AR qc lib$target.a
        done
        echo "Adding symbol table to archive."
        ar sv lib$target.a
        mv lib$target.a ../
        cd -
        rm -rf tmpobj
    else
        libtool -static -arch_only i386 -o lib$target.a ${thelibs[@]:0}
    fi
}


make_so () {
    target=$1
    if [ $PLATFORM = "Linux" ]; then
        echo "Generate lib$target.so ..."
        $CC -shared -o lib$target.so -Wl,-whole-archive $thelibs -Wl,-no-whole-archive $ldflags
        #$CC -DTEST_PRIV_API -o /tmp/test_$target -L. -l$target $ldflags 
    else
        libtool -dynamic -arch_only i386 -o lib$target.so ${thelibs[@]:0}
    fi
}


build_webrtc() {
    if [ $PLATFORM = "Darwin" ]; then
        local_root=$ROOT/third_party/libjingle/trunk/xcodebuild/
    else
        local_root=$ROOT/third_party/libjingle/trunk/out/
    fi

    local_root=$local_root/$BUILDTYPE
    mkdir -p $local_root
    cd $local_root
    target=webrtc_$BUILDTYPE
    rm -f lib$target.a
    if [ $PLATFORM = "Darwin" ]; then
        thelibs=`find . -name "lib*.a" -print`
    else
        thelibs=`find obj.target/ -name lib*.a`
    fi
    make_archive $target
}



detect_env
config_webrtc
#build_jingle
#build_webrtc

exit 0
