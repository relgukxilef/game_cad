cmake_minimum_required(VERSION 3.5)

project(GameCAD CXX)

enable_testing()

add_subdirectory(source)
target_include_directories(GameCAD PUBLIC include)

include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.9
)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(DOXYGEN_FILE windows.x64.bin.zip)
    set(DOXYGEN_PATH doxygen)
else()
    set(DOXYGEN_FILE linux.bin.tar.gz)
    # For some reason the path inside the archive is different between platforms
    set(DOXYGEN_PATH doxygen-1.15.0/bin/doxygen)
endif()

FetchContent_Declare(
    doxygen
    URL
    "https://github.com/doxygen/doxygen/releases/download/Release_1_15_0/doxygen-1.15.0.${DOXYGEN_FILE}"
    DOWNLOAD_EXTRACT_TIMESTAMP true
)

FetchContent_MakeAvailable(imgui glfw doxygen)

FetchContent_GetProperties(doxygen SOURCE_DIR doxygen_dir)

add_library(
    imgui STATIC 
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(
    imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(imgui PUBLIC glfw)

add_subdirectory(samples)

add_custom_target(
    documentation ALL
    ${doxygen_dir}/${DOXYGEN_PATH} 
    ${CMAKE_CURRENT_SOURCE_DIR}/documentation/Doxyfile
)
