cmake_minimum_required(VERSION 3.29)

project(Server)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin/Server/${CMAKE_BUILD_TYPE})
set(DEPENDENCIES_DIR ${CMAKE_SOURCE_DIR}/Dependencies)
set(PLATFORM_LINK_LIBRARIES)

add_executable(Server
    Source/main.cpp
    Source/Server.cpp
)

target_include_directories(Server PRIVATE
    ${CMAKE_SOURCE_DIR}/Common/Include
    ${DEPENDENCIES_DIR}
    ${DEPENDENCIES_DIR}/boost_asio
    ${DEPENDENCIES_DIR}/libpqxx/include
    ${DEPENDENCIES_DIR}/fmt/include
)

add_subdirectory(${DEPENDENCIES_DIR}/libpqxx ${CMAKE_CURRENT_BINARY_DIR}/libpqxx)

target_link_directories(Server PRIVATE $ENV{PostgreSQL_LIBRARY})

target_link_libraries(Server PRIVATE Common libpqxx::pqxx libpq)
