#!/bin/bash
# author: peterxu
#

## build config
ROOT=`pwd`
HOST=`uname`
WEBRTC_URL=http://webrtc.googlecode.com/svn/trunk
WEBRTC_REV=5301
OUT=out

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

    if [ $HOST != "Linux" ] && [ $HOST != "Darwin" ] ; then
        echor "[Err] Only support host Linux and MacOSX"
        exit 1
    fi

    if [ "#$TARGET" = "#" ]; then
        echor "[Err] No set for TARGET"
        exit 1
    fi

    if [ "#$BUILD_TYPE" = "#" ]; then
        BUILD_TYPE=Release
    fi

    if [ $BUILD_TYPE != "Release" ] && [ $BUILD_TYPE != "Debug" ]; then
        echor "[Err] only support build types: Release, Debug"
        exit 1
    fi

    OUT=$OUT_DIR/$BUILD_TYPE
    if [ $TARGET = "UNIX" ]; then
        AR=ar
        CC=gcc
    elif [ $TARGET = "ANDROID" ]; then
        if [ "#$JAVA_HOME" = "#" ]; then
            echor "[Err] no JAVA_HOME env set!"
            exit 1
        fi
        if [ "#$ANDROID_HOME" = "#" ]; then
            echor "[Err] no ANDROID_HOME env set!"
            exit 1
        fi
        if [ "#$ANDROID_NDK_HOME" = "#" ]; then
            echor "[Err] no ANDROID_NDK_HOME env set!"
            exit 1
        fi
        SYSROOT=$ANDROID_NDK_HOME/toolchains/arm-linux-androideabi-4.6/prebuilt/
        AR=`$ANDROID_NDK_HOME/ndk-which ar`
        CC=`$ANDROID_NDK_HOME/ndk-which gcc` --sysroot=$SYSROOT
    elif [ $TARGET = "MAC" ]; then
        :
    elif [ $TARGET = "IOS" ]; then
        OUT=$OUT_DIR/$BUILD_TYPE-iphoneos
    elif [ $TARGET = "IOS-SIM" ]; then
        :
    else
        echor "[Err] only support targets: UNIX, MAC, ANDROID, IOS and IOS-SIM"
        exit 1
    fi

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
        (cd -P $obj_path/trunk && patch -p0 < $ROOT/scripts/webrtc_mac.patch)
    fi

    # ./third_party/webrtc/trunk/talk/app/webrtc/objc/README
    if [ $TARGET = "IOS" ] || [ $TARGET = "IOS-SIM" ] || [ $TARGET = "MAC" ]; then
        echo "target_os = ['ios', 'mac']" >> .gclient
        #gclient sync -r $WEBRTC_REV >/tmp/svn_webrtc.log 2>&1
        gclient runhooks >/tmp/svn_webrtc.log 2>&1
        check_err "fail to get webrtc with hook from svn!"
    elif [ $TARGET = "ANDROID" ]; then
        echo "target_os = ['android', 'unix']" >> .gclient
        gclient sync -r $WEBRTC_REV --nohooks
        check_err "fail to get webrtc with hook from svn!"

        pushd $obj_path/trunk
        source ./build/android/envsetup.sh
        popd
    elif [ $TARGET = "UNIX" ]; then
        gclient runhooks >/tmp/svn_webrtc.log 2>&1
        check_err "fail to get webrtc with hook from svn!"
    fi
    popd
}


build_webrtc() {
    obj_path=$ROOT/third_party/webrtc
    echog "[+] Building webrtc in $obj_path/trunk for $TARGET for $BUILD_TYPE ..."

    pushd $obj_path/trunk
    mkdir -p $OUT
    ninja -C $OUT
    ninja -C $OUT -j 1 # build again to avoid failed before
    check_err "fail to build webrtc $TARGET"
    popd
}


make_archive () {
    target=$1
    echog "[-] Generating archive lib$target.a ..."
    if [ $TARGET = "UNIX" ] || [ $TARGET = "ANDROID" ]; then
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
        $AR sv lib$target.a
        mv lib$target.a ../
        cd -
        rm -rf tmpobj
    elif [ $TARGET = "IOS" ]; then
        libtool -static -arch_only armv7 -o lib$target.a ${thelibs[@]:0}
    elif [ $TARGET = "IOS-SIM" ]; then
        libtool -static -arch_only i386 -o lib$target.a ${thelibs[@]:0}
    elif [ $TARGET = "MAC" ]; then
        libtool -static -arch_only x86_64 -o lib$target.a ${thelibs[@]:0}
    fi
}


make_so () {
    target=$1
    echog "[-] Generate shared lib$target.so ..."
    if [ $TARGET = "UNIX" ] || [ $TARGET = "ANDROID" ]; then
        $CC -shared -o lib$target.so -Wl,-whole-archive $thelibs -Wl,-no-whole-archive $ldflags
        #$CC -DTEST_PRIV_API -o /tmp/test_$target -L. -l$target $ldflags 
    elif [ $TARGET = "MAC" ]; then
        libtool -dynamic -arch_only x86_64 -o lib$target.so ${thelibs[@]:0}
    fi
}


pack_webrtc() {
    obj_path=$ROOT/third_party/webrtc
    echog "[+] To package webrtc libs $OUT ..."
    local_root=$obj_path/trunk/$OUT
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

test_webrtc() {
    obj_path=$ROOT/third_party
    echog "[+] To test webrtc of $TARGET ..."
    if [ $TARGET = "IOS-SIM" ]; then
        if [ ! -e $obj_path/iphonesim ]; then
            git clone https://github.com/hborders/iphonesim.git
        fi

        echog "[-] building iphonesim tool..."
        pushd $obj_path/iphonesim
        xcodebuild
        popd

        echog "[-] lanuching AppRTCDemo.app ..."
        if [ -f $obj_path/iphonesim/build/Release/iphonesim ]; then
            $obj_path/iphonesim/build/Release/iphonesim launch $obj_path/webrtc/trunk/$OUT/AppRTCDemo.app &
        fi
    fi
}

clean_webrtc() {
    obj_path=$ROOT/third_party/webrtc
    local_root=$obj_path/trunk/$OUT
    echog "[+] To clean webrtc of $OUT ..."
    if [ -e $local_root ]; then
        rm -rf $local_root
    fi
}

build_librtc() {
    pushd $ROOT
    mkdir bld
    pushd bld
    if [ $TARGET = "MAC" ]; then
        cmake -D MAC=1 -D CMAKE_BUILD_TYPE=$BUILD_TYPE -G Xcode ..
        xcodebuild -target ubase_static -target rtc_static -target testrtc -configuration $BUILD_TYPE
    elif [ $TARGET = "IOS" ]; then
        cmake -D TARGET=$TARGET -D IOS=1 -D CMAKE_BUILD_TYPE=$BUILD_TYPE -G Xcode ..
        xcodebuild -target ubase_static -target rtc_static -target testrtc -configuration $BUILD_TYPE
    elif [ $TARGET = "IOS-SIM" ]; then
        cmake -D TARGET=$TARGET -D IOS=1 -D CMAKE_BUILD_TYPE=$BUILD_TYPE -G Xcode ..
        xcodebuild -target ubase_static -target rtc_static -target testrtc -configuration $BUILD_TYPE
    elif [ $TARGET = "UNIX" ]; then
        cmake -D CMAKE_BUILD_TYPE=$BUILD_TYPE UNIX=1 ..
        make
    fi
    popd
    popd
}

main() {
    if [ $# -lt 1 ]; then 
        echor "usage: $0 build|clean|librtc"
        exit 1
    fi

    detect_env
    if [ $1 = "clean" ]; then
        clean_webrtc
    elif [ $1 = "build" ]; then
        config_webrtc
        build_webrtc
        pack_webrtc
        test_webrtc
    elif [ $1 = "librtc" ]; then
        build_librtc
    else
        echor "usage: $0 build|clean"
    fi
}

main $*
exit 0
