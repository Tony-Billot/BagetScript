#include "interpreter.hpp"

#include <cmath>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace
{
double asNumber(const Value& value)
{
	if (auto number = std::get_if<double>(&value.data))
		return *number;

	throw std::runtime_error("Valeur numerique attendue.");
}
}

void Interpreter::execute(const Program& program)
{
	for (const auto& statement : program.statements)
		executeStmt(*statement);
}

void Interpreter::executeStmt(const Stmt& stmt)
{
	if (const auto* expressionStmt = dynamic_cast<const ExpressionStmt*>(&stmt))
	{
		evaluate(*expressionStmt->expr);
		return;
	}

	if (const auto* printStmt = dynamic_cast<const PrintStmt*>(&stmt))
	{
		Value value = evaluate(*printStmt->expr);
		std::cout << valueToString(value) << '\n';
		return;
	}

	if (const auto* varDecl = dynamic_cast<const VarDeclStmt*>(&stmt))
	{
		Value value;

		if (varDecl->initializer)
			value = evaluate(*varDecl->initializer);
		else if (varDecl->typeToken == TokenType::ENTIER)
			value.data = 0.0;
		else if (varDecl->typeToken == TokenType::TEXTE)
			value.data = std::string{};
		else
			value.data = false;

		if (varDecl->typeToken == TokenType::ENTIER)
		{
			if (!std::holds_alternative<double>(value.data))
				throw std::runtime_error("Le type 'entier' attend une valeur numerique.");
		}
		else if (varDecl->typeToken == TokenType::TEXTE)
		{
			if (!std::holds_alternative<std::string>(value.data))
				throw std::runtime_error("Le type 'texte' attend une chaine.");
		}
		else if (varDecl->typeToken == TokenType::BOOLEEN)
		{
			if (!std::holds_alternative<bool>(value.data))
				value.data = isTruthy(value);
		}

		variables[varDecl->name] = value;
		return;
	}

	if (const auto* blockStmt = dynamic_cast<const BlockStmt*>(&stmt))
	{
		for (const auto& innerStmt : blockStmt->statements)
			executeStmt(*innerStmt);

		return;
	}

	if (const auto* ifStmt = dynamic_cast<const IfStmt*>(&stmt))
	{
		if (isTruthy(evaluate(*ifStmt->condition)))
			executeStmt(*ifStmt->thenBranch);
		else if (ifStmt->elseBranch)
			executeStmt(*ifStmt->elseBranch);

		return;
	}

	if (const auto* whileStmt = dynamic_cast<const WhileStmt*>(&stmt))
	{
		while (isTruthy(evaluate(*whileStmt->condition)))
			executeStmt(*whileStmt->body);

		return;
	}

	throw std::runtime_error("Type de statement inconnu.");
}

Value Interpreter::evaluate(const Expr& expr)
{
	if (const auto* literal = dynamic_cast<const LiteralExpr*>(&expr))
		return literal->value;

	if (const auto* variable = dynamic_cast<const VariableExpr*>(&expr))
	{
		auto it = variables.find(variable->name);

		if (it == variables.end())
			throw std::runtime_error("Variable inconnue: " + variable->name);

		return it->second;
	}

	if (const auto* assign = dynamic_cast<const AssignExpr*>(&expr))
	{
		auto it = variables.find(assign->name);

		if (it == variables.end())
			throw std::runtime_error("Variable inconnue: " + assign->name);

		Value value = evaluate(*assign->value);
		it->second = value;
		return value;
	}

	if (const auto* grouping = dynamic_cast<const GroupingExpr*>(&expr))
		return evaluate(*grouping->expr);

	if (const auto* unary = dynamic_cast<const UnaryExpr*>(&expr))
	{
		Value right = evaluate(*unary->right);

		if (unary->op == TokenType::NOT)
			return Value{!isTruthy(right)};

		if (unary->op == TokenType::MINUS)
			return Value{-asNumber(right)};

		throw std::runtime_error("Operateur unaire inconnu.");
	}

	if (const auto* binary = dynamic_cast<const BinaryExpr*>(&expr))
	{
		Value left = evaluate(*binary->left);
		Value right = evaluate(*binary->right);

		switch (binary->op)
		{
			case TokenType::PLUS:
				if (std::holds_alternative<double>(left.data) && std::holds_alternative<double>(right.data))
					return Value{asNumber(left) + asNumber(right)};

				return Value{valueToString(left) + valueToString(right)};

			case TokenType::MINUS:
				return Value{asNumber(left) - asNumber(right)};

			case TokenType::STAR:
				return Value{asNumber(left) * asNumber(right)};

			case TokenType::SLASH:
			{
				double divisor = asNumber(right);

				if (divisor == 0.0)
					throw std::runtime_error("Division par zero.");

				return Value{asNumber(left) / divisor};
			}

			case TokenType::PERCENT:
			{
				double divisor = asNumber(right);

				if (divisor == 0.0)
					throw std::runtime_error("Modulo par zero.");

				return Value{std::fmod(asNumber(left), divisor)};
			}

			case TokenType::GREATER:
				return Value{asNumber(left) > asNumber(right)};

			case TokenType::GREATER_EQUAL:
				return Value{asNumber(left) >= asNumber(right)};

			case TokenType::LESS:
				return Value{asNumber(left) < asNumber(right)};

			case TokenType::LESS_EQUAL:
				return Value{asNumber(left) <= asNumber(right)};

			case TokenType::EQUAL_EQUAL:
				return Value{valuesEqual(left, right)};

			case TokenType::NOT_EQUAL:
				return Value{!valuesEqual(left, right)};

			case TokenType::AND:
				return Value{isTruthy(left) && isTruthy(right)};

			case TokenType::OR:
				return Value{isTruthy(left) || isTruthy(right)};

			default:
				throw std::runtime_error("Operateur binaire inconnu.");
		}
	}

	throw std::runtime_error("Type d'expression inconnu.");
}

bool Interpreter::isTruthy(const Value& value)
{
	if (auto booleanValue = std::get_if<bool>(&value.data))
		return *booleanValue;

	if (auto number = std::get_if<double>(&value.data))
		return *number != 0.0;

	if (auto text = std::get_if<std::string>(&value.data))
		return !text->empty();

	return false;
}

std::string Interpreter::valueToString(const Value& value)
{
	if (auto booleanValue = std::get_if<bool>(&value.data))
		return *booleanValue ? "vrai" : "faux";

	if (auto number = std::get_if<double>(&value.data))
	{
		std::ostringstream out;
		out << *number;
		return out.str();
	}

	if (auto text = std::get_if<std::string>(&value.data))
		return *text;

	return "";
}

bool Interpreter::valuesEqual(const Value& left, const Value& right)
{
	if (left.data.index() != right.data.index())
		return false;

	if (auto leftNumber = std::get_if<double>(&left.data))
		return *leftNumber == std::get<double>(right.data);

	if (auto leftText = std::get_if<std::string>(&left.data))
		return *leftText == std::get<std::string>(right.data);

	if (auto leftBool = std::get_if<bool>(&left.data))
		return *leftBool == std::get<bool>(right.data);

	return false;
}
