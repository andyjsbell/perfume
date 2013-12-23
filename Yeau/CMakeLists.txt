# For Yeau's version number
set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_REV 0)
set(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_REV}")

cmake_minimum_required(VERSION 2.8)

# To declare project
project(Yeau C CXX)


set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang)

add_subdirectory(misc/libneon)
add_subdirectory(misc/libuuid)
add_subdirectory(misc/unqlite)
add_subdirectory(misc/jx9eau)

add_subdirectory(compte)
add_subdirectory(testing)

