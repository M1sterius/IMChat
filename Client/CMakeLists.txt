cmake_minimum_required(VERSION 3.29)

project(Client)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin/Client/${CMAKE_BUILD_TYPE})
set(DEPENDENCIES_DIR ${CMAKE_SOURCE_DIR}/Dependencies)

add_executable(Client
    ${DEPENDENCIES_DIR}/imgui/imgui.cpp
    ${DEPENDENCIES_DIR}/imgui/imgui_draw.cpp
    ${DEPENDENCIES_DIR}/imgui/imgui_impl_glfw.cpp
    ${DEPENDENCIES_DIR}/imgui/imgui_impl_opengl3.cpp
    ${DEPENDENCIES_DIR}/imgui/imgui_tables.cpp
    ${DEPENDENCIES_DIR}/imgui/imgui_widgets.cpp
    ${DEPENDENCIES_DIR}/glad/src/glad.c

    Source/main.cpp
    Source/Client.cpp
    Source/ClientUI.cpp
)

target_include_directories(Client PRIVATE
    ${DEPENDENCIES_DIR}
    ${CMAKE_SOURCE_DIR}/Common/Include
    ${DEPENDENCIES_DIR}/boost_asio
    ${DEPENDENCIES_DIR}/glfw/include
    ${DEPENDENCIES_DIR}/glad/include
)

add_subdirectory(${DEPENDENCIES_DIR}/glfw ${CMAKE_CURRENT_BINARY_DIR}/glfw)

target_link_libraries(Client PRIVATE Common glfw)
