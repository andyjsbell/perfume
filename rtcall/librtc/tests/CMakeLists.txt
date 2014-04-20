set(xrtc_EXEC_SRCS
    testrtc.cpp
)

include_directories(
    ${PROJECT_SOURCE_DIR}/src
)

link_directories(
    ${PROJECT_BINARY_DIR}/lib
)

link_libraries(xrtc rtc ${all_libs})

add_executable(xrtc ${xrtc_EXEC_SRCS})
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

install(TARGETS xrtc RUNTIME DESTINATION bin)
