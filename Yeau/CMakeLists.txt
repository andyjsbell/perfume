cmake_minimum_required(VERSION 2.8)

project(Yeau)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -O2")  
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -O2") 

#add_definitions(-g -Wall)
add_definitions(-DHAVE_PTHREAD_H -DHAS_ATOMICS=1)

include_directories(
    ${PROJECT_SOURCE_DIR}/compte
    ${PROJECT_SOURCE_DIR}/ubase
    ${PROJECT_SOURCE_DIR}/unqlite
)

link_directories(
)


#add_subdirectory(compte)
#add_subdirectory(ubase)
#add_subdirectory(unqlite)
add_subdirectory(testing)

set(Yeau_LIB_SRCS
    unqlite/unqlite.c
    ubase/atomic.cpp
    ubase/mutex.cpp
    compte/compte.cpp
    compte/store.cpp
)

add_library(Yeau STATIC ${Yeau_LIB_SRCS})
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib/) 

#set(CMAKE_INSTALL_PREFIX /Yeau)
#install(DIRECTORY DESTINATION . PATTERN "*.*" EXCLUDE)

