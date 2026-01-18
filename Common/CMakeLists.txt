cmake_minimum_required(VERSION 3.29)

project(Common)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin/Common/${CMAKE_BUILD_TYPE})
set(DEPENDENCIES_DIR ${CMAKE_SOURCE_DIR}/Dependencies)
set(PLATFORM_LINK_LIBRARIES)

add_library(Common STATIC
    Source/Connection.cpp
    Source/Utility.cpp
    Source/Message.cpp
)

target_include_directories(Common PRIVATE
    ${DEPENDENCIES_DIR}
    ${DEPENDENCIES_DIR}/boost_asio
    ${DEPENDENCIES_DIR}/fmt/include
    Include
)

if (WIN32)
    list(APPEND PLATFORM_LINK_LIBRARIES ws2_32 iphlpapi)
elseif (UNIX)
    list(APPEND PLATFORM_LINK_LIBRARIES pthread)
endif ()

add_subdirectory(${DEPENDENCIES_DIR}/fmt ${CMAKE_CURRENT_BINARY_DIR}/fmt)

target_link_libraries(Common PUBLIC ${PLATFORM_LINK_LIBRARIES} fmt::fmt)
