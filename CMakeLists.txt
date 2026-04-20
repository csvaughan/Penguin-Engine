cmake_minimum_required(VERSION 3.16)
project(PenguinEngineWorkspace)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Global compiler optimizations
if(MSVC) 
    add_compile_options(/arch:AVX2)
else()
    add_compile_options(-march=native)
endif()

# Build the Engine first, then the Game
add_subdirectory(Penguin)
add_subdirectory(Game)