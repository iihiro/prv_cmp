cmake_minimum_required(VERSION 3.0.0)

set(project_name LowLatencyPrivateComparison)
project(${project_name})

set(CMAKE_PALISADE_DIR "/home/$ENV{USER}/palisade-release")

find_package(Threads REQUIRED)
set(CMAKE_CXX_FLAGS "-O3 -std=c++17 -openmp -pthread -Wall -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -std=c++17 -openmp -pthread -Wall")

set(CMAKE_LIB_PALISADE "${CMAKE_PALISADE_DIR}/bin/lib")
set(CMAKE_THIRDPARTY_LIB_PALISADE "${CMAKE_PALISADE_DIR}/third-party/lib")

include_directories(${CMAKE_PALISADE_DIR}/src ${CMAKE_PALISADE_DIR}/third-party/include
                    ${CMAKE_PALISADE_DIR}/src/pke/lib ${CMAKE_PALISADE_DIR}/src/core/lib
                    ${PROJECT_SOURCE_DIR}/prvc ${PROJECT_SOURCE_DIR}/stdsc)
link_directories(${CMAKE_LIB_PALISADE} ${CMAKE_THIRDPARTY_LIB_PALISADE})

set(COMMON_LIBS stdsc prvc_share PALISADEpke PALISADEcore ntl)

add_subdirectory(stdsc)
add_subdirectory(prvc)
add_subdirectory(demo)
