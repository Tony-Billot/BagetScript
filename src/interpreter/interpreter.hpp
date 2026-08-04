#pragma once

#include <string>
#include <unordered_map>

#include "ast.hpp"

class Interpreter
{
public:
	void execute(const Program& program);

private:
	std::unordered_map<std::string, Value> variables;
	std::unordered_map<std::string, const FunctionStmt*> functions;

	void executeStmt(const Stmt& stmt);
	Value evaluate(const Expr& expr);
    Value callFunction(const FunctionStmt& function, const std::vector<Value>& arguments);

	static bool isTruthy(const Value& value);
	static std::string valueToString(const Value& value);
	static bool valuesEqual(const Value& left, const Value& right);
};
