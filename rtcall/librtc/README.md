librtc
======

1. cmake for ios
    http://code.google.com/p/ios-cmake/
    cmake -DCMAKE_TOOLCHAIN_FILE=ios-cmake

    Then, in CMakeLists.txt:
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -arch armv7")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -arch armv7")
