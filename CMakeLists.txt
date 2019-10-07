cmake_minimum_required(VERSION 3.0.0)

project(LowLatencyPrivateComparison)

set(CMAKE_PALISADE_DIR "/home/$ENV{USER}/palisade-release")

set(CMAKE_CXX_FLAGS "-O2 -Wall -std=c++17 -fopenmp -pthread")
set(CMAKE_LIB_PALISADE "${CMAKE_PALISADE_DIR}/bin/lib")
set(CMAKE_THIRDPARTY_LIB_PALISADE "${CMAKE_PALISADE_DIR}/third-party/lib")

include_directories(${CMAKE_PALISADE_DIR}/src ${CMAKE_PALISADE_DIR}/third-party/include ./include ${CMAKE_PALISADE_DIR}/src/pke/lib ${CMAKE_PALISADE_DIR}/src/core/lib /usr/local/include)
link_directories(${CMAKE_LIB_PALISADE} ${CMAKE_THIRDPARTY_LIB_PALISADE})

add_subdirectory(demo)
