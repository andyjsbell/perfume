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
    compte/compteimpl.cpp
    compte/store.cpp
)

add_library(Yeau STATIC ${Yeau_LIB_SRCS})
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib/) 

#set(CMAKE_INSTALL_PREFIX /tmp/Yeau)
INSTALL(PROGRAMS ${PROJECT_BINARY_DIR}/bin/testYeau DESTINATION bin)
INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/scripts DESTINATION bin)
INSTALL(PROGRAMS ${PROJECT_BINARY_DIR}/lib/libYeau.a DESTINATION lib)
INSTALL(FILES ${PROJECT_SOURCE_DIR}/compte/compte.h DESTINATION inc)
INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/ubase/ DESTINATION inc 
        FILES_MATCHING PATTERN "*.h")

