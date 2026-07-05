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
	functions.clear();

	for (const auto& statement : program.statements)
	{
		if (const auto* functionStmt = dynamic_cast<const FunctionStmt*>(statement.get()))
			functions[functionStmt->name] = functionStmt;
	}

	for (const auto& statement : program.statements)
	{
		if (dynamic_cast<const FunctionStmt*>(statement.get()))
			continue;

		executeStmt(*statement);
	}
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
		else if (varDecl->typeToken == TokenType::LISTE)
			value.data = Value::List{};
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
		else if (varDecl->typeToken == TokenType::LISTE)
		{
			if (!std::holds_alternative<Value::List>(value.data))
				throw std::runtime_error("Le type 'liste' attend une liste.");
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

	if (const auto* forRangeStmt = dynamic_cast<const ForRangeStmt*>(&stmt))
	{
		auto previousVariables = variables;

		double start = asNumber(evaluate(*forRangeStmt->start));
		double end = asNumber(evaluate(*forRangeStmt->end));
		double step = start <= end ? 1.0 : -1.0;

		for (double i = start; (step > 0.0) ? (i <= end) : (i >= end); i += step)
		{
			variables[forRangeStmt->variableName] = Value{i};
			executeStmt(*forRangeStmt->body);
		}

		variables = std::move(previousVariables);
		return;
	}

	if (const auto* forEachStmt = dynamic_cast<const ForEachStmt*>(&stmt))
	{
		Value iterableValue = evaluate(*forEachStmt->iterable);
		auto* list = std::get_if<Value::List>(&iterableValue.data);

		if (!list)
			throw std::runtime_error("La boucle 'pour ... dans ...' attend une liste.");

		auto previousVariables = variables;

		for (const auto& item : *list)
		{
			variables[forEachStmt->variableName] = item;
			executeStmt(*forEachStmt->body);
		}

		variables = std::move(previousVariables);
		return;
	}

	if (const auto* functionStmt = dynamic_cast<const FunctionStmt*>(&stmt))
	{
		functions[functionStmt->name] = functionStmt;
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

	if (const auto* listExpr = dynamic_cast<const ListExpr*>(&expr))
	{
		Value::List list;
		list.reserve(listExpr->elements.size());

		for (const auto& elementExpr : listExpr->elements)
			list.push_back(evaluate(*elementExpr));

		return Value{std::move(list)};
	}

	if (const auto* callExpr = dynamic_cast<const CallExpr*>(&expr))
	{
		const auto* calleeVariable = dynamic_cast<const VariableExpr*>(callExpr->callee.get());

		if (!calleeVariable)
			throw std::runtime_error("Appel de fonction invalide.");

		auto functionIt = functions.find(calleeVariable->name);

		if (functionIt == functions.end())
			throw std::runtime_error("Fonction inconnue: " + calleeVariable->name);

		std::vector<Value> arguments;
		arguments.reserve(callExpr->arguments.size());

		for (const auto& argumentExpr : callExpr->arguments)
			arguments.push_back(evaluate(*argumentExpr));

		return callFunction(*functionIt->second, arguments);
	}

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

Value Interpreter::callFunction(const FunctionStmt& function, const std::vector<Value>& arguments)
{
	if (arguments.size() != function.parameters.size())
	{
		throw std::runtime_error(
			"Nombre d'arguments invalide pour '" +
			function.name +
			"' (attendu: " +
			std::to_string(function.parameters.size()) +
			", recu: " +
			std::to_string(arguments.size()) +
			").");
	}

	auto previousVariables = variables;

	for (size_t i = 0; i < function.parameters.size(); ++i)
		variables[function.parameters[i]] = arguments[i];

	for (const auto& stmt : function.body)
		executeStmt(*stmt);

	variables = std::move(previousVariables);

	Value result;
	result.data = false;
	return result;
}

bool Interpreter::isTruthy(const Value& value)
{
	if (auto booleanValue = std::get_if<bool>(&value.data))
		return *booleanValue;

	if (auto number = std::get_if<double>(&value.data))
		return *number != 0.0;

	if (auto text = std::get_if<std::string>(&value.data))
		return !text->empty();

	if (auto list = std::get_if<Value::List>(&value.data))
		return !list->empty();

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

	if (auto list = std::get_if<Value::List>(&value.data))
	{
		std::ostringstream out;
		out << "[";

		for (size_t i = 0; i < list->size(); ++i)
		{
			if (i > 0)
				out << ", ";

			out << valueToString((*list)[i]);
		}

		out << "]";
		return out.str();
	}

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

	if (auto leftList = std::get_if<Value::List>(&left.data))
	{
		const auto& rightList = std::get<Value::List>(right.data);

		if (leftList->size() != rightList.size())
			return false;

		for (size_t i = 0; i < leftList->size(); ++i)
		{
			if (!valuesEqual((*leftList)[i], rightList[i]))
				return false;
		}

		return true;
	}

	return false;
}
