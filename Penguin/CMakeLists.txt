cmake_minimum_required(VERSION 3.16)
project(Penguin)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Disable Interprocedural Optimization (LTO) for faster iteration
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION OFF CACHE BOOL "Disable LTO" FORCE)

# Path to thirdparty directory
set(TP_PATH "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty")
set(IMGUI_DIR "${TP_PATH}/imgui")

# Build all subdirectories as static to match your engine's linking style
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)

# ==============================================================================
# 1. IMGUI TARGET SETUP
# ==============================================================================
set(IMGUI_SOURCES
    "${IMGUI_DIR}/imgui.cpp"
    "${IMGUI_DIR}/imgui_draw.cpp"
    "${IMGUI_DIR}/imgui_tables.cpp"
    "${IMGUI_DIR}/imgui_widgets.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_sdlrenderer3.cpp"
)

add_library(imgui STATIC ${IMGUI_SOURCES})

target_include_directories(imgui PUBLIC 
    "${IMGUI_DIR}"
    "${IMGUI_DIR}/backends"
)

# ==============================================================================
# 2. EXTERNAL DEPENDENCIES (Subdirectories)
# ==============================================================================
add_subdirectory(${TP_PATH}/SDL3)
add_subdirectory(${TP_PATH}/glm)
add_subdirectory(${TP_PATH}/SDL3_image)
add_subdirectory(${TP_PATH}/SDL3_ttf) 
add_subdirectory(${TP_PATH}/miniaudio) 
add_subdirectory(${TP_PATH}/spdlog)

# Link ImGui to your local static SDL3 target
target_link_libraries(imgui PRIVATE SDL3::SDL3-static)

# ==============================================================================
# 3. ENGINE TARGET SETUP (Penguin)
# ==============================================================================
file(GLOB_RECURSE PENGUIN_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_library(Penguin STATIC ${PENGUIN_SOURCES})

# Compile Definitions
target_compile_definitions(Penguin PRIVATE PGN_ENGINE_EXPORTS)

if(PRODUCTION_BUILD)
    target_compile_definitions(Penguin PRIVATE ENGINE_RESOURCES_PATH="./resources/" PRODUCTION_BUILD=1) 
else()
    target_compile_definitions(Penguin PRIVATE ENGINE_RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources/" PRODUCTION_BUILD=0)
endif()

# Configuration-specific definitions
target_compile_definitions(Penguin PUBLIC 
    $<$<CONFIG:Debug>:PGN_DEBUG>
    $<$<CONFIG:Release>:PGN_RELEASE>
)

# Include Directories for the Engine
target_include_directories(Penguin PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${TP_PATH}"
    "${TP_PATH}/stb"
    "${TP_PATH}/SDL3/include"
    "${TP_PATH}/SDL3_image/include"
    "${TP_PATH}/SDL3_ttf/include"
    "${TP_PATH}/spdlog/include"
    "${IMGUI_DIR}"
    "${IMGUI_DIR}/backends"
)

# Link Libraries for the Engine
target_link_libraries(Penguin PRIVATE
    glm
    miniaudio
    SDL3::SDL3-static              
    SDL3_image::SDL3_image-static  
    SDL3_ttf::SDL3_ttf-static
    imgui  
)

target_link_libraries(Penguin PUBLIC spdlog)