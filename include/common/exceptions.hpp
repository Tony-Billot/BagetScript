#pragma once
#include <stdexcept>
#include <string>

class BagetScriptError : public std::runtime_error
{
public:
    BagetScriptError(const std::string& message, int line, int column) : std::runtime_error(message), line(line), column(column) {}

    int line;
    int column;
};

class LexicalError : public BagetScriptError { using BagetScriptError::BagetScriptError; };
class SyntaxError  : public BagetScriptError { using BagetScriptError::BagetScriptError; };