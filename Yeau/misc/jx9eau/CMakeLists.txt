set(jx9eau_EXEC_SRCS
    jx9eau.cpp
    jx9net.c
)

include_directories(
    ..
    ../libneon/src
)

link_directories(
    ${PROJECT_BINARY_DIR}/lib
)

link_libraries(jx9eau unqlite neon)

add_executable(jx9eau ${jx9eau_EXEC_SRCS})
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

install(TARGETS jx9eau RUNTIME DESTINATION bin)
