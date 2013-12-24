#CheckFunctionExists
#CheckIncludeFile
#CheckIncludeFileCXX
#CheckIncludeFiles
#CheckLibraryExists
#CheckStructHasMember
#CheckSymbolExists
#CheckTypeSize
#CheckVariableExists


include(CheckIncludeFiles)
include(CheckFunctionExists)

check_include_files("dlfcn.h" HAVE_DLFCN_H)
check_include_files("fcntl.h" HAVE_FCNTL_H)
check_function_exists(ftruncate HAVE_FTRUNCATE)
check_function_exists(gettimeofday HAVE_GETTIMEOFDAY)
check_include_files("inttypes.h" HAVE_INTTYPES_H)
check_include_files("limits.h" HAVE_LIMITS_H)
check_include_files("memory.h" HAVE_MEMORY_H)
check_function_exists(memset HAVE_MEMSET)
check_include_files("netinet/in.h" HAVE_NETINET_IN_H)
check_function_exists(socket HAVE_SOCKET)
check_function_exists(srandom HAVE_SRANDOM)
check_include_files("stdint.h" HAVE_STDINT_H)
check_include_files("stdlib.h" HAVE_STDLIB_H)
check_include_files("strings.h" HAVE_STRINGS_H)
check_include_files("string.h" HAVE_STRING_H)
check_function_exists(strtoul HAVE_STRTOUL)
check_include_files("sys/file.h" HAVE_SYS_FILE_H)
check_include_files("sys/ioctl.h" HAVE_SYS_IOCTL_H)
check_include_files("sys/socket.h" HAVE_SYS_SOCKET_H)
check_include_files("sys/stat.h" HAVE_SYS_STAT_H)
check_include_files("sys/time.h" HAVE_SYS_TIME_H)
check_include_files("sys/types.h" HAVE_SYS_TYPES_H)
check_include_files("unistd.h" HAVE_UNISTD_H)
check_function_exists(usleep HAVE_USLEEP)

set(PACKAGE \"libuuid\")
set(PACKAGE_BUGREPORT \"sloowfranklin@gmail.com\")
set(PACKAGE_NAME \"libuuid\")
set(PACKAGE_STRING \"libbuuid\ 1.0.2\")
set(PACKAGE_TARNAME \"libuuid\")
set(PACKAGE_URL \"\")
set(PACKAGE_VERSION \"1.0.2\")

set(STDC_HEADERS 1)
set(VERSION \"1.0.2\")


configure_file(config.h.cmake.in config.h)
include_directories(${PROJECT_BINARY_DIR}/misc/libuuid)
