cmake_minimum_required(VERSION 2.8)

project(Yeau)

# CMAKE_C_FLAGS CMAKE_CXX_FLAGS
add_definitions(-O2 -Wall)
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


set(so_NAME Yeau)
add_library(${so_NAME} SHARED ${Yeau_LIB_SRCS})
set_target_properties(${so_NAME} PROPERTIES CLEAN_DIRECT_OUTPUT 1) 
#set_target_properties(${so_NAME} PROPERTIES VERSION 1.0 SOVERSION 1)

set(a_NAME Yeau_static)
add_library(${a_NAME} STATIC ${Yeau_LIB_SRCS})
set_target_properties(${a_NAME} PROPERTIES OUTPUT_NAME ${so_NAME})
set_target_properties(${a_NAME} PROPERTIES CLEAN_DIRECT_OUTPUT 1) 

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib/) 

#set(CMAKE_INSTALL_PREFIX /tmp/Yeau)
install(TARGETS ${so_NAME} LIBRARY DESTINATION lib)
install(TARGETS ${a_NAME}  ARCHIVE DESTINATION lib)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/scripts DESTINATION bin)
install(FILES ${PROJECT_SOURCE_DIR}/compte/compte.h DESTINATION inc)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/ubase/ DESTINATION inc 
        FILES_MATCHING PATTERN "*.h")

