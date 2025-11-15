cmake_minimum_required(VERSION 4.0)

project(Common)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin/Common/${CMAKE_BUILD_TYPE})

add_library(Common STATIC
    Source/Common.cpp
)

target_include_directories(Common PRIVATE
    Include
)
