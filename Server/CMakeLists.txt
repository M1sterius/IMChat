cmake_minimum_required(VERSION 3.29)

project(Server)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin/Server/${CMAKE_BUILD_TYPE})

add_executable(Server
    Source/main.cpp
    Source/Server.cpp
)

target_include_directories(Server PRIVATE
    ${CMAKE_SOURCE_DIR}/Common/Include
    ${CMAKE_SOURCE_DIR}/Dependencies
    ${CMAKE_SOURCE_DIR}/Dependencies/boost_asio
    ${CMAKE_SOURCE_DIR}/Dependencies/libpqxx/include
)

message(STATUS "PostgreSQL root: ${PostgreSQL_ROOT}")
message(STATUS "PostgreSQL include: ${PostgreSQL_INCLUDE_DIR}")
message(STATUS "PostgreSQL library: ${PostgreSQL_LIBRARY}")
add_subdirectory(${CMAKE_SOURCE_DIR}/Dependencies/libpqxx ${CMAKE_CURRENT_BINARY_DIR}/libpqxx)

target_link_libraries(Server PRIVATE Common libpqxx::pqxx)
