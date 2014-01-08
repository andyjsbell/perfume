# CMAKE_C_FLAGS CMAKE_CXX_FLAGS
add_definitions(-O2 -Wall)

include_directories(
    ${PROJECT_SOURCE_DIR}/ubase
    ${PROJECT_SOURCE_DIR}/misc/
    ${PROJECT_SOURCE_DIR}/misc/jx9eau
    ${PROJECT_SOURCE_DIR}/misc/libneon/src
)

# For Yeau
add_definitions(-DHAVE_PTHREAD_H -DHAS_ATOMICS=1)
set(Yeau_LIB_SRCS
    ../ubase/atomic.cpp
    ../ubase/mutex.cpp
    ../ubase/umisc.cpp
    eau_all.cpp  
    ieau_impl.cpp  
    jx9_api.cpp
    ../misc/jx9eau/jx9net.c
)

link_directories(
    ${PROJECT_BINARY_DIR}/lib
)
link_libraries(Yeau unqlite uuid neon pthread)


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
install(FILES ${PROJECT_SOURCE_DIR}/src/ieau_api.h DESTINATION inc)

