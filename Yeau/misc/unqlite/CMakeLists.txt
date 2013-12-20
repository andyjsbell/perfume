#cmake_minimum_required(VERSION 2.8)
#project(unqlite)

# CMAKE_C_FLAGS CMAKE_CXX_FLAGS
add_definitions(-O2 -Wall)

include_directories(
    .
)

link_directories(
)

set(unqlite_LIB_SRCS
    unqlite.c
)

set(a_NAME unqlite)
add_library(${a_NAME} STATIC ${unqlite_LIB_SRCS})
set_target_properties(${a_NAME} PROPERTIES CLEAN_DIRECT_OUTPUT 1) 
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib/) 

install(TARGETS ${a_NAME}  ARCHIVE DESTINATION lib)

