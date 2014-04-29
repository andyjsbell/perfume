add_definitions(
    -fno-rtti
    -DPOSIX
    -DOSX
    -DWEBRTC_MAC
)

find_library(FWCORESERVICES CoreServices)
find_library(FWCOREAUDIO CoreAudio)
find_library(FWCOREVIDEO CoreVideo)
find_library(FWQTKIT QTKit)
find_library(FWOPENGL OpenGL)
find_library(FWAUDIOTOOLBOX AudioToolbox)
find_library(FWAPPLICATIONSERVICES ApplicationServices)
find_library(FWFOUNDATION Foundation)
find_library(FWAPPKIT AppKit)
find_library(FWSECURITY Security)
find_library(FWIOKIT IOKit)

find_library(LIBWEBRTC libwebrtc_${CMAKE_BUILD_TYPE}.a third_party/webrtc/trunk/out_mac/${CMAKE_BUILD_TYPE})

set(all_libs
    ${LIBWEBRTC}
    ${FWCORESERVICES}
    ${FWCOREAUDIO}
    ${FWCOREVIDEO}
    ${FWQTKIT}
    ${FWOPENGL}
    ${FWAUDIOTOOLBOX}
    ${FWAPPLICATIONSERVICES}
    ${FWFOUNDATION}
    ${FWAPPKIT}
    ${FWSECURITY}
    ${FWIOKIT}
    -lcrypto
    -lssl
)
