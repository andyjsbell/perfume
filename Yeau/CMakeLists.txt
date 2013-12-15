cmake_minimum_required(VERSION 2.8)

project(Yeau)

# CMAKE_C_FLAGS CMAKE_CXX_FLAGS
add_definitions(-O2 -Wall)

include_directories(
    ${PROJECT_SOURCE_DIR}/compte
    ${PROJECT_SOURCE_DIR}/ubase
    ${PROJECT_SOURCE_DIR}/misc/unqlite
    ${PROJECT_SOURCE_DIR}/misc/libuuid
)

link_directories(
)


#add_subdirectory(compte)
#add_subdirectory(ubase)
#add_subdirectory(unqlite)
add_subdirectory(testing)

# For Yeau
add_definitions(-DHAVE_PTHREAD_H -DHAS_ATOMICS=1)
set(Yeau_LIB_SRCS
    ubase/atomic.cpp
    ubase/mutex.cpp
    ubase/umisc.cpp
    compte/compte.cpp
    compte/compteimpl.cpp
    compte/store.cpp
    compte/jx9util.cpp
    misc/unqlite/unqlite.c
)

# for uuid
add_definitions(-DHAVE_UNISTD_H)
add_definitions(-DHAVE_STDLIB_H)
add_definitions(-DHAVE_SYS_TIME_H)
set(uuid_LIB_SRCS
    misc/libuuid/clear.c     
    #misc/libuuid/gen_uuid_nt.c  
    #misc/libuuid/tst_uuid.c
    misc/libuuid/compare.c   
    misc/libuuid/isnull.c       
    misc/libuuid/unpack.c
    misc/libuuid/copy.c      
    misc/libuuid/pack.c         
    misc/libuuid/unparse.c
    misc/libuuid/gen_uuid.c  
    misc/libuuid/parse.c        
    misc/libuuid/uuid_time.c
)


set(so_NAME Yeau)
add_library(${so_NAME} SHARED ${Yeau_LIB_SRCS} ${uuid_LIB_SRCS})
set_target_properties(${so_NAME} PROPERTIES CLEAN_DIRECT_OUTPUT 1) 
#set_target_properties(${so_NAME} PROPERTIES VERSION 1.0 SOVERSION 1)

set(a_NAME Yeau_static)
add_library(${a_NAME} STATIC ${Yeau_LIB_SRCS} ${uuid_LIB_SRCS})
set_target_properties(${a_NAME} PROPERTIES OUTPUT_NAME ${so_NAME})
set_target_properties(${a_NAME} PROPERTIES CLEAN_DIRECT_OUTPUT 1) 

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib/) 

#set(CMAKE_INSTALL_PREFIX /tmp/Yeau)
install(TARGETS ${so_NAME} LIBRARY DESTINATION lib)
install(TARGETS ${a_NAME}  ARCHIVE DESTINATION lib)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/scripts DESTINATION bin)

install(FILES ${PROJECT_SOURCE_DIR}/misc/libuuid/uuid.h DESTINATION inc)
install(FILES ${PROJECT_SOURCE_DIR}/compte/compte.h DESTINATION inc)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/ubase/ DESTINATION inc 
        FILES_MATCHING PATTERN "*.h")

