#!/bin/bash
# author: peterxu
#

## build config
ROOT=`pwd`
HOST=`uname`
WEBRTC_URL=http://webrtc.googlecode.com/svn/trunk
WEBRTC_REV=5301
OUT_DIR="out"


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

    if [ $HOST != "Darwin" ] ; then
        if [ "#"$JAVA_HOME = "#" ]; then
            echor "[Err] no JAVA_HOME env set!"
            exit 1
        fi
    fi

    if [ $HOST != "Linux" ] && [ $HOST != "Darwin" ] ; then
        echor "[Err] Only support host Linux and MacOSX"
        exit 1
    fi

    if [ "#"$TARGET = "#" ]; then
        TARGET=Linux
    fi

    # Linux, Android, MacOSX, IOS
    if [ $TARGET = "Linux" ]; then
        AR=ar
        CC=gcc
    elif [ $TARGET = "Android" ]; then
        if [ "#"$ANDROID_NDK_HOME = "#" ]; then
            echor "[Err] no ANDROID_NDK_HOME env set!"
            exit 1
        fi
        SYSROOT=$ANDROID_NDK_HOME/toolchains/arm-linux-androideabi-4.6/prebuilt/
        AR=`$ANDROID_NDK_HOME/ndk-which ar`
        CC=`$ANDROID_NDK_HOME/ndk-which gcc` --sysroot=$SYSROOT
    elif [ $TARGET = "MacOSX" ] || [ $TARGET = "IOS" ]; then
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

wrbase() {
  #cd /path/to/webrtc/trunk
  export GYP_DEFINES="build_with_libjingle=1 build_with_chromium=0 libjingle_objc=1"
  export GYP_GENERATORS="ninja"
}

wrios() {
  wrbase
  export GYP_DEFINES="$GYP_DEFINES OS=ios target_arch=armv7"
  export GYP_GENERATOR_FLAGS="$GYP_GENERATOR_FLAGS output_dir=out_ios"
  export GYP_CROSSCOMPILE=1
}

wrsim() {
  wrbase
  export GYP_DEFINES="$GYP_DEFINES OS=ios target_arch=ia32"
  export GYP_GENERATOR_FLAGS="$GYP_GENERATOR_FLAGS output_dir=out_sim"
  export GYP_CROSSCOMPILE=1
}

wrmac() {
  wrbase
  export GYP_DEFINES="$GYP_DEFINES OS=mac target_arch=x64"
  export GYP_GENERATOR_FLAGS="$GYP_GENERATOR_FLAGS output_dir=out_mac"
}


config_webrtc() {
    obj_path=$ROOT/third_party/webrtc
    echog "[+] Getting webrtc from its repo into $obj_path ..."

    mkdir -p $obj_path
    pushd $obj_path
    rm -f .gclient
    gclient config --name=trunk $WEBRTC_URL
    if [ ! -e $obj_path/trunk ]; then
        gclient sync -r $WEBRTC_REV --force >/tmp/svn_webrtc.log 2>&1
        check_err "fail to get webrtc with force from svn!"
    fi

    export GYP_DEFINES="build_with_libjingle=1 build_with_chromium=0"
    if [ $TARGET = "IOS" ]; then
        #wrios
        wrsim
        echo "target_os = ['ios', 'mac']" >> .gclient
    elif [ $TARGET = "MacOSX" ]; then
        wrmac
    elif [ $TARGET = "Android" ]; then
        echo "target_os = ['android', 'unix']" >> .gclient
        export GYP_GENERATOR_FLAGS="$GYP_GENERATOR_FLAGS output_dir=out_android"
        gclient sync --nohooks
        pushd $obj_path/trunk
        source ./build/android/envsetup.sh
        popd
    elif [ $TARGET = "Linux" ]; then
        export GYP_GENERATOR_FLAGS="$GYP_GENERATOR_FLAGS output_dir=out_linux"
    fi

    gclient runhooks --force >/tmp/svn_webrtc.log 2>&1
    check_err "fail to get webrtc with hook from svn!"
    popd
}


build_webrtc() {
    obj_path=$ROOT/third_party/webrtc
    echog "[+] Building webrtc in $obj_path/trunk for $TARGET ..."

    echog "Building webrtc $OUT_DIR/$BUILD_TYPE ..."
    mkdir -p $OUT_DIR/$BUILD_TYPE
    ninja -C $OUT_DIR/$BUILD_TYPE
    ninja -C $OUT_DIR/$BUILD_TYPE -j 1 # build again to avoid failed before
    check_err "fail to build webrtc $TARGET"
    popd
}


make_archive () {
    target=$1
    echog "[+] Generating archive lib$target.a ..."
    if [ $HOST = "Linux" ]; then
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
    if [ $HOST = "Linux" ]; then
        $CC -shared -o lib$target.so -Wl,-whole-archive $thelibs -Wl,-no-whole-archive $ldflags
        #$CC -DTEST_PRIV_API -o /tmp/test_$target -L. -l$target $ldflags 
    else
        libtool -dynamic -arch_only i386 -o lib$target.so ${thelibs[@]:0}
    fi
}


pack_webrtc() {
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
