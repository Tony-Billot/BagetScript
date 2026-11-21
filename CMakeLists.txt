cmake_minimum_required(VERSION 3.15)

project(BagetScript VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/utf-8)
else()
    add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

include_directories(include)

set(SOURCES
    src/main.cpp
    src/repl/repl.cpp
    src/lexer/lexer.cpp
    src/parser/parser.cpp
    src/interpreter/interpreter.cpp
    src/types/number.cpp
    src/types/text.cpp
    src/types/boolean.cpp
    src/debug/ast_printer.cpp
)

add_executable(bagetscript ${SOURCES})

enable_testing()

add_executable(bagetscript_tests
    tests/unit_tests.cpp
    src/lexer/lexer.cpp
    src/parser/parser.cpp
    src/interpreter/interpreter.cpp
)

target_include_directories(bagetscript_tests PRIVATE include)

add_test(NAME bagetscript_unit_tests COMMAND bagetscript_tests)

if(MSVC)
    add_custom_command(TARGET bagetscript_tests POST_BUILD
        COMMAND cmd.exe /d /c "chcp 65001>nul && \"$<TARGET_FILE:bagetscript_tests>\""
        COMMENT "Exécution des tests unitaires BagetScript"
    )
else()
    add_custom_command(TARGET bagetscript_tests POST_BUILD
        COMMAND $<TARGET_FILE:bagetscript_tests>
        COMMENT "Exécution des tests unitaires BagetScript"
    )
endif()