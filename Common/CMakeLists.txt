cmake_minimum_required(VERSION 3.29)

project(Common)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin/Common/${CMAKE_BUILD_TYPE})

add_library(Common STATIC
    Source/Connection.cpp
        Source/Utility.cpp
)

target_include_directories(Common PRIVATE
    ${CMAKE_SOURCE_DIR}/Dependencies/boost_asio
    Include
)
