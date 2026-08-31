cmake_minimum_required(VERSION 3.15)

project(BagetScript VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)

include_directories(include)

set(SOURCES
    src/main.cpp
    src/repl/repl.cpp
    src/lexer/lexer.cpp
    src/types/number.cpp
    src/types/text.cpp
)

add_executable(bagetscript ${SOURCES})