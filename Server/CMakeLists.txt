cmake_minimum_required(VERSION 3.29)

project(Server)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin/Server/${CMAKE_BUILD_TYPE})

add_executable(Server
    Source/main.cpp
        Source/Server.cpp
        Source/Server.hpp
)

target_include_directories(Server PRIVATE
    ${CMAKE_SOURCE_DIR}/Common/Include
    ${CMAKE_SOURCE_DIR}/Dependencies/boost_asio
)

target_link_libraries(Server PRIVATE Common)
