cmake_minimum_required(VERSION 3.15)

project(BagetScript VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Inclure headers
include_directories(include)

# Fichiers sources
set(SOURCES
    src/main.cpp
    src/lexer.cpp
    src/parser.cpp
    src/interpreter.cpp
)

# Executable
add_executable(bagetscript ${SOURCES})