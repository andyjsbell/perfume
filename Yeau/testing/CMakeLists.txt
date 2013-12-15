set(testYeau_EXEC_SRCS
    testYeau.cpp
)

include_directories(
    ${PROJECT_SOURCE_DIR}/compte
    ${PROJECT_SOURCE_DIR}/ubase
)

link_directories(
    ${PROJECT_BINARY_DIR}/lib
)

link_libraries(testYeau Yeau)

add_executable(testYeau ${testYeau_EXEC_SRCS})
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

install(TARGETS testYeau RUNTIME DESTINATION bin)
