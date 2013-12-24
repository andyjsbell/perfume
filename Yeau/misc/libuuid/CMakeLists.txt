#cmake_minimum_required(VERSION 2.8)
#project(uuid)

# CMAKE_C_FLAGS CMAKE_CXX_FLAGS
add_definitions(-DHAVE_CONFIG_H)

include_directories(
    .
)

link_directories(
)

set(uuid_LIB_SRCS
    clear.c     
    compare.c   
    isnull.c       
    unpack.c
    copy.c      
    pack.c         
    unparse.c
    gen_uuid.c  
    parse.c        
    uuid_time.c
    #gen_uuid_nt.c  
    #tst_uuid.c
)

include(cmake_check.cmake)

set(so_NAME uuid)
add_library(${so_NAME} SHARED ${uuid_LIB_SRCS})
set_target_properties(${so_NAME} PROPERTIES CLEAN_DIRECT_OUTPUT 1) 
#set_target_properties(${so_NAME} PROPERTIES VERSION 1.0 SOVERSION 1)

set(a_NAME uuid_static)
add_library(${a_NAME} STATIC ${uuid_LIB_SRCS})
set_target_properties(${a_NAME} PROPERTIES OUTPUT_NAME ${so_NAME})
set_target_properties(${a_NAME} PROPERTIES CLEAN_DIRECT_OUTPUT 1) 

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib/) 

install(TARGETS ${so_NAME} LIBRARY DESTINATION lib)
install(TARGETS ${a_NAME}  ARCHIVE DESTINATION lib)

