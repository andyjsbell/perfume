set(jx9eau_EXEC_SRCS
    jx9eau.cpp
    ../unqlite/unqlite.c
)

include_directories(
    ${PROJECT_SOURCE_DIR}/misc/unqlite
)

link_directories(
    ${PROJECT_BINARY_DIR}/lib
)

#link_libraries(jx9eau Yeau)

add_executable(jx9eau ${jx9eau_EXEC_SRCS})
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

install(TARGETS jx9eau RUNTIME DESTINATION bin)
