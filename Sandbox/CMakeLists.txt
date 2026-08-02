project(Sandbox)

option(PRODUCTION_BUILD "Make this a production build" OFF)

# 1. Gather Sandbox Source
file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(Sandbox ${GAME_SOURCES})

# Path Logic (Resources)
if(PRODUCTION_BUILD)
    target_compile_definitions(Sandbox PUBLIC ASSET_PATH="./assets/" PRODUCTION_BUILD=1) 
else()
    target_compile_definitions(Sandbox PUBLIC ASSET_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/" PRODUCTION_BUILD=0)
endif()

# Windows specific tweaks
if(MSVC)
    target_compile_definitions(Sandbox PUBLIC _CRT_SECURE_NO_WARNINGS)
    set_target_properties(Sandbox PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>$<$<CONFIG:Debug>:Debug>")
endif()

set(TP_PATH "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty")

add_subdirectory(${TP_PATH}/tmxlite)
add_subdirectory(${TP_PATH}/json)

# 2. Update Include Directories
target_include_directories(Sandbox BEFORE PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/")

# 3. Link Libraries (This populates the target with all thirdparty include paths)
target_link_libraries(Sandbox PRIVATE 
    nlohmann_json::nlohmann_json
    Penguin
    tmxlite
)

# 4. Precompile Headers (MUST BE AT THE BOTTOM)
# Since Sandbox links to Penguin and thirdparty libs, these are all safely available via angle brackets.
target_precompile_headers(Sandbox PRIVATE
    <pgpch.h>
    <nlohmann/json.hpp>
    <tmxlite/Map.hpp>
    <tmxlite/TileLayer.hpp>
)