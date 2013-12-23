#cmake_minimum_required(VERSION 2.8)
#project(neon)

# CMAKE_C_FLAGS CMAKE_CXX_FLAGS
add_definitions(-O2 -Wall)
#add_definitions(-no-cpp-precomp)
add_definitions(-DHAVE_CONFIG_H -DNE_PRIVATE=extern)

include_directories(
    .
)

link_directories(
)

# for uuid
set(neon_LIB_SRCS
    #src/ne_basic.c     
    #src/ne_props.c     
    #src/ne_locks.c   
    #src/ne_xml.c
    #src/ne_xmlreq.c
    #src/ne_207.c       
    #src/ne_oldacl.c    
    #src/ne_acl3744.c   
    #ne_gnutls.c  
    #ne_openssl.c   
    src/ne_dates.c   
    src/ne_session.c
    src/ne_uri.c
    src/ne_socket.c
    src/ne_utils.c
    src/ne_alloc.c     
    src/ne_i18n.c    
    src/ne_pkcs11.c    
    src/ne_socks.c
    src/ne_auth.c      
    src/ne_sspi.c
    src/ne_md5.c     
    src/ne_redirect.c  
    src/ne_string.c
    src/ne_compress.c  
    src/ne_ntlm.c    
    src/ne_request.c   
    src/ne_stubssl.c
)

# add extra header/function/type check
include(cmake_check.cmake)

set(so_NAME neon)
add_library(${so_NAME} SHARED ${neon_LIB_SRCS})
set_target_properties(${so_NAME} PROPERTIES CLEAN_DIRECT_OUTPUT 1) 
#set_target_properties(${so_NAME} PROPERTIES VERSION 1.0 SOVERSION 1)

set(a_NAME neon_static)
add_library(${a_NAME} STATIC ${neon_LIB_SRCS})
set_target_properties(${a_NAME} PROPERTIES OUTPUT_NAME ${so_NAME})
set_target_properties(${a_NAME} PROPERTIES CLEAN_DIRECT_OUTPUT 1) 

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib/) 

install(TARGETS ${so_NAME} LIBRARY DESTINATION lib)
install(TARGETS ${a_NAME}  ARCHIVE DESTINATION lib)

