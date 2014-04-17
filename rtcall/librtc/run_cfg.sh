#!/bin/sh
# author: peterxu
#

## build config
ROOT=`pwd`
HOST_OS=`uname`
WEBRTC_URL=http://webrtc.googlecode.com/svn/trunk
WEBRTC_REV=5301


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
    which git >/dev/null
    check_err "git is required and was not found in the path!"

    which gclient >/dev/null
    check_err "'gclient' is required and was not found in the path!"

    which ninja >/dev/null
    check_err "'ninja' is required and was not found in the path!"

    if [ $HOST_OS != "Darwin" ] ; then
        if [ "#"$JAVA_HOME = "#" ]; then
            echor "[Err] no JAVA_HOME env set!"
            exit 1
        fi
    fi

    if [ $HOST_OS != "Linux" ] && [ $HOST_OS != "Darwin" ] ; then
        echor "[Err] Only support host Linux and MacOSX"
        exit 1
    fi

    if [ "#"$TARGET_OS = "#" ]; then
        TARGET_OS=Linux
    fi

    # Linux, Android, MacOSX, IOS
    if [ $TARGET_OS = "Linux" ]; then
        AR=ar
        CC=gcc
    elif [ $TARGET_OS = "Android" ]; then
        if [ "#"$ANDROID_NDK_HOME = "#" ]; then
            echor "[Err] no ANDROID_NDK_HOME env set!"
            exit 1
        fi
        SYSROOT=$ANDROID_NDK_HOME/toolchains/arm-linux-androideabi-4.6/prebuilt/
        AR=`$ANDROID_NDK_HOME/ndk-which ar`
        CC=`$ANDROID_NDK_HOME/ndk-which gcc` --sysroot=$SYSROOT
    elif [ $TARGET_OS = "MacOSX" ] || [ $TARGET_OS = "IOS" ]; then
        :
    else
        echor "[Err] only support targets: Linux, MacOSX, Android and IOS"
        exit 1
    fi

    if [ "#"$BUILD_TYPE = "#" ]; then
        BUILD_TYPE=Release
    fi

    if [ $BUILD_TYPE != "Release" ] && [ $BUILD_TYPE = "Debug" ]; then
        echor "[Err] only support build types: Release, Debug"
        exit 1
    fi
}


config_webrtc() {
    obj_path=$ROOT/third_party/webrtc
    echog "[+] Getting webrtc from its repo into $obj_path ..."
    mkdir -p $obj_path
    if [ ! -e $obj_path/trunk ]; then
        cd $obj_path
        gclient config --name=trunk $WEBRTC_URL
        gclient sync -r $WEBRTC_REV --force >/tmp/svn_webrtc.log 2>&1
        check_err "fail to gclient sync for webrtc!"
    fi
}

build_webrtc_mac() {
    export GYP_DEFINES="build_with_libjingle=1 build_with_chromium=0 libjingle_objc=1"
    export GYP_GENERATORS="ninja"

    if [ $TARGET_OS = "IOS" ]; then
        outdir=out_sim
        export GYP_DEFINES="$GYP_DEFINES OS=ios target_arch=ia32"
        export GYP_CROSSCOMPILE=1
    elif [ $TARGET_OS = "MacOSX" ]; then
        outdir=out_mac
        export GYP_DEFINES="$GYP_DEFINES OS=mac target_arch=x64"
        export GYP_CROSSCOMPILE=0
    else
        return;
    fi
    #export GYP_GENERATOR_FLAGS="$GYP_GENERATOR_FLAGS output_dir=$outdir"

    obj_path=$ROOT/third_party/webrtc
    pushd $obj_path/trunk
    mkdir -p $outdir/$BUILD_TYPE
    ninja -C $outdir/$BUILD_TYPE
    check_err "fail to build webrtc $outdir"
    popd
}

build_webrtc_nix() {
    obj_path=$ROOT/third_party/webrtc
    pushd $obj_path
    if [ $TARGET_OS = "Android" ]; then
        echo "target_os = ['android', 'unix']" >> .gclient
        gclient sync -r $WEBRTC_REV --nohooks >/tmp/svn_webrtc.log 2>&1
    fi
    popd

    pushd $obj_path/trunk
    if [ $TARGET_OS = "Android" ]; then
        outdir=out_android
        source ./build/android/envsetup.sh
    else
        outdir=out_linux
    fi
    mkdir -p $outdir/$BUILD_TYPE
    ninja -C $outdir/$BUILD_TYPE
    ninja -C $outdir/$BUILD_TYPE -j 1 # build again to avoid failed before
    check_err "fail to build webrtc $outdir!"
    popd
}

build_webrtc() {
    obj_path=$ROOT/third_party/webrtc
    if [ ! -e $obj_path/trunk ]; then
        return
    fi

    echog "[+] Building webrtc_nix in $obj_path/trunk for $TARGET_OS ..."
    if [ $TARGET_OS == "Android" ] || [ $TARGET_OS == "Linux" ]; then
        build_webrtc_nix
    elif [ $TARGET_OS == "IOS" ] || [ $TARGET_OS == "MacOSX" ]; then
        build_webrtc_mac
    fi
}


make_archive () {
    target=$1
    echog "[+] Generating archive lib$target.a ..."
    if [ $HOST_OS = "Linux" ]; then
        rm -rf tmpobj; mkdir tmpobj; cd tmpobj
        $AR x ../libyuv.a
        for lib in $thelibs; do
            lib=../$lib
            if [ ! -e $lib ]; then
                echor "Can not find $lib!"; continue
            fi
            #echo "Processing $lib ..."
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
    echog "[+] Generate shared lib$target.so ..."
    if [ $HOST_OS = "Linux" ]; then
        $CC -shared -o lib$target.so -Wl,-whole-archive $thelibs -Wl,-no-whole-archive $ldflags
        #$CC -DTEST_PRIV_API -o /tmp/test_$target -L. -l$target $ldflags 
    else
        libtool -dynamic -arch_only i386 -o lib$target.so ${thelibs[@]:0}
    fi
}


pack_webrtc() {
    # for jingle
    targets="libjingle libjingle_p2p libjingle_media libjingle_peerconnection libjingle_sound libjingle_xmpphelp peerconnection_server "
    # for webrtc misc
    targets=$targets"bitrate_controller remote_bitrate_estimator rtp_rtcp udp_transport webrtc_utility system_wrappers "
    targets=$targets"libjpeg libsrtp protobuf_full_do_not_use protobuf_lite protoc libyuv "
    # for webrtc audio
    targets=$targets"resampler signal_processing vad CNG G711 G722 NetEq NetEq4 PCM16B iLBC iSAC iSACFix opus "
    targets=$targets"audio_coding_module audio_conference_mixer audio_device audio_processing "
    targets=$targets"voice_engine_core media_file paced_sender audio_processing_sse2 webrtc_opus "
    # for webrtc video
    targets=$targets"common_video video_capture_module webrtc_vp8 video_coding_utility video_processing video_processing_sse2 "
    targets=$targets"webrtc_video_coding video_engine_core video_render_module webrtc_i420 "
    targets=$targets"gen_asm_offsets_vp8 gen_asm_offsets_vp9 gen_asm_offsets_vpx_scale "
    targets=$targets"libvpx libvpx_asm_offsets libvpx_asm_offsets_vp9 libvpx_asm_offsets_vpx_scale "

    local_root=$ROOT/third_party/webrtc/trunk/out/
    local_root=$local_root/$BUILD_TYPE
    if [ -e $local_root ]; then
        cd $local_root
        target=webrtc_$BUILD_TYPE
        rm -f lib$target.a
        excludes="testing\|unittest\|test\|Test\|protobuf_full_do_not_use"
        thelibs=`find . -name "lib*.a" -print | grep -v "$excludes"`
        make_archive $target
        check_err "fail to gen archive .a"
        #make_so $target
        check_err "fail to gen shared .so"
    else
        echor "fail to pack webrtc"
    fi
}



detect_env
config_webrtc
build_webrtc
pack_webrtc

exit 0
