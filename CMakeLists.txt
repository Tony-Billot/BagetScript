cmake_minimum_required(VERSION 3.15)

project(BagetScript VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Inclure les headers dans la nouvelle arborescence
include_directories(
    src/common
    src/lexer
    src/parser
    src/interpreter
)

# Fichiers sources
set(SOURCES
    src/main.cpp
    src/lexer/lexer.cpp
    src/parser/parser.cpp
    src/interpreter/interpreter.cpp
)

# Executable
add_executable(bagetscript ${SOURCES})