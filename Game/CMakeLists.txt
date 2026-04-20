project(Game)

option(PRODUCTION_BUILD "Make this a production build" OFF)

# 1. Gather Game Source (Ensure this path is correct!)
file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(Game ${GAME_SOURCES})

# Path Logic (Resources)
if(PRODUCTION_BUILD)
    target_compile_definitions(Game PUBLIC RESOURCES_PATH="./resources/" PRODUCTION_BUILD=1) 
else()
    target_compile_definitions(Game PUBLIC RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources/" PRODUCTION_BUILD=0)
endif()

# Windows specific tweaks
if(MSVC)
    target_compile_definitions(Game PUBLIC _CRT_SECURE_NO_WARNINGS)
    set_target_properties(Game PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>$<$<CONFIG:Debug>:Debug>")
endif()

set(TP_PATH "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty")

add_subdirectory(${TP_PATH}/tmxlite)
add_subdirectory(${TP_PATH}/json)

# 2. Update Include Directories
target_include_directories(Game BEFORE PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/")

target_link_libraries(Game PRIVATE 
    nlohmann_json::nlohmann_json
    Penguin
    tmxlite
)