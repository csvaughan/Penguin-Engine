cmake_minimum_required(VERSION 3.28)
project(PenguinEngineWorkspace)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_LINKER_TYPE LLD)

# 2. Heavy Optimizations Restricted to Release Only
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL TRUE)

# 3. Global Compiler & Build-Speed Options
if(MSVC) 
    add_compile_options(
        /arch:AVX2
        /MP                  
        /Zc:inline           
        /permissive-         
    )
    add_link_options($<$<CONFIG:Debug>:/INCREMENTAL>)
else()
    # Add -Winvalid-pch here to diagnose Linux PCH issues
    add_compile_options(-march=native -Winvalid-pch)
endif()

# Build the Engine first, then the Game
add_subdirectory(Penguin)
add_subdirectory(Sandbox)