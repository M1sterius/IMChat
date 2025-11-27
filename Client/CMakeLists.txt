cmake_minimum_required(VERSION 3.29)

project(Client)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin/Client/${CMAKE_BUILD_TYPE})

add_executable(Client
    Source/main.cpp
    Source/Client.cpp
)

target_include_directories(Client PRIVATE
    ${CMAKE_SOURCE_DIR}/Common/Include
    ${CMAKE_SOURCE_DIR}/Dependencies
    ${CMAKE_SOURCE_DIR}/Dependencies/boost_asio
)

target_link_libraries(Client PRIVATE Common)
