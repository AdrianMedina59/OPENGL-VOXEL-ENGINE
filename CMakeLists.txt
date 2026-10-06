cmake_minimum_required(VERSION 3.20)
project(VoxelEngine LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

include(FetchContent)

# Build fetched dependencies as static libraries so no DLLs need to be shipped.
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# ---------------------------------------------------------------------------
# GLFW: downloaded and built from source by default so every compiler gets a
# matching build (a system package built for MinGW breaks MSVC, etc.).
# ---------------------------------------------------------------------------
option(VOXEL_USE_SYSTEM_GLFW "Use an installed GLFW instead of fetching it" OFF)
if(VOXEL_USE_SYSTEM_GLFW)
    find_package(glfw3 3.3 REQUIRED)
else()
    set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG        3.4
        GIT_SHALLOW    TRUE)
    FetchContent_MakeAvailable(glfw)
endif()

# ---------------------------------------------------------------------------
# libnoise (qknight's CMake fork)
# ---------------------------------------------------------------------------
set(BUILD_LIBNOISE_UTILS         OFF CACHE BOOL "" FORCE)
set(BUILD_LIBNOISE_EXAMPLES      OFF CACHE BOOL "" FORCE)
set(BUILD_LIBNOISE_DOCUMENTATION OFF CACHE BOOL "" FORCE)
# libnoise asks for an ancient cmake_minimum_required that CMake 4 rejects.
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
FetchContent_Declare(libnoise
    GIT_REPOSITORY https://github.com/qknight/libnoise.git
    GIT_TAG        9ce0737b55812f7de907e86dc633724524e3a8e8)
FetchContent_MakeAvailable(libnoise)
# The upstream target only exposes its headers privately.
target_include_directories(noise-static SYSTEM INTERFACE ${libnoise_SOURCE_DIR}/src)

# ---------------------------------------------------------------------------
# Vendored libraries (third_party/)
# ---------------------------------------------------------------------------
set(THIRD_PARTY ${CMAKE_CURRENT_SOURCE_DIR}/third_party)

find_package(OpenGL REQUIRED)

add_library(glad STATIC ${THIRD_PARTY}/glad/src/glad.c)
target_include_directories(glad SYSTEM PUBLIC ${THIRD_PARTY}/glad/include)
target_link_libraries(glad PUBLIC ${CMAKE_DL_LIBS})

add_library(glm INTERFACE)
target_include_directories(glm SYSTEM INTERFACE ${THIRD_PARTY}/glm)

add_library(stb INTERFACE)
target_include_directories(stb SYSTEM INTERFACE ${THIRD_PARTY}/stb)

add_library(imgui STATIC
    ${THIRD_PARTY}/imgui/imgui.cpp
    ${THIRD_PARTY}/imgui/imgui_demo.cpp
    ${THIRD_PARTY}/imgui/imgui_draw.cpp
    ${THIRD_PARTY}/imgui/imgui_tables.cpp
    ${THIRD_PARTY}/imgui/imgui_widgets.cpp
    ${THIRD_PARTY}/imgui/imgui_impl_glfw.cpp
    ${THIRD_PARTY}/imgui/imgui_impl_opengl3.cpp)
target_include_directories(imgui SYSTEM PUBLIC ${THIRD_PARTY}/imgui)
target_link_libraries(imgui PUBLIC glfw OpenGL::GL)

# ---------------------------------------------------------------------------
# Engine executable
# ---------------------------------------------------------------------------
add_executable(VoxelEngine
    src/main.cpp
    src/setup.cpp
    src/input.cpp
    src/Shader.cpp
    src/World.cpp
    src/Chunk.cpp)
target_include_directories(VoxelEngine PRIVATE src)
target_link_libraries(VoxelEngine PRIVATE glad glm stb imgui glfw noise-static OpenGL::GL)

if(MSVC)
    target_compile_options(VoxelEngine PRIVATE /W3 /utf-8)
    target_compile_definitions(VoxelEngine PRIVATE _CRT_SECURE_NO_WARNINGS)
else()
    target_compile_options(VoxelEngine PRIVATE -Wall)
endif()

# Shaders and textures are loaded with relative paths ("shaders/...",
# "textures/..."), so copy them next to the executable after every build.
add_custom_command(TARGET VoxelEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/assets $<TARGET_FILE_DIR:VoxelEngine>
    COMMENT "Copying assets next to the executable")

# Run from the executable's folder when launched from Visual Studio.
set_target_properties(VoxelEngine PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:VoxelEngine>)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT VoxelEngine)
