cmake_minimum_required(VERSION 3.27)
project(aca VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
set(CMAKE_COLOR_MAKEFILE OFF)

find_package(OpenMP)
if(OPENMP_FOUND)
    message("OpenMP loaded")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

include_directories(inc)
link_directories(lib)
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/conworld.cpp
    src/guiworld.cpp
    src/rules.cpp
    src/lexer.cpp
    src/token.cpp
    src/parse.cpp
)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
target_link_libraries(${PROJECT_NAME} libraylib.a winmm gdi32)