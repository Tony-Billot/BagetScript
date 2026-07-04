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

	void executeStmt(const Stmt& stmt);
	Value evaluate(const Expr& expr);

	static bool isTruthy(const Value& value);
	static std::string valueToString(const Value& value);
	static bool valuesEqual(const Value& left, const Value& right);
};
