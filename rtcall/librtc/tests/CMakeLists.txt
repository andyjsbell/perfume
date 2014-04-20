set(testrtc_EXEC_SRCS
    testrtc.cpp
)

include_directories(
    ${PROJECT_SOURCE_DIR}/inc
)

link_directories(
    ${PROJECT_BINARY_DIR}/lib
)
link_libraries(testrtc rtc ${all_libs} stdc++)

add_executable(testrtc ${testrtc_EXEC_SRCS})
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

install(TARGETS testrtc RUNTIME DESTINATION bin)
