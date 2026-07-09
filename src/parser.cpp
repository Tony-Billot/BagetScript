#include "parser.hpp"

#include <cstdlib>
#include <stdexcept>

Parser::Parser(const std::vector<Token>& tokens)
    : tokens(tokens), current(0)
{
}

Token& Parser::peek()
{
    return tokens[current];
}

Token& Parser::previous()
{
    return tokens[current - 1];
}

Token Parser::advance()
{
    if (!isAtEnd())
        current++;

    return previous();
}

bool Parser::isAtEnd()
{
    return peek().type == TokenType::END_OF_FILE;
}

bool Parser::check(TokenType type)
{
    if (isAtEnd())
        return false;

    return peek().type == type;
}

bool Parser::match(TokenType type)
{
    if (!check(type))
        return false;

    advance();
    return true;
}

Token Parser::consume(TokenType type, const std::string& message)
{
    if (check(type))
        return advance();

    throw std::runtime_error(message);
}

std::unique_ptr<Program> Parser::parse()
{
    auto program = std::make_unique<Program>();

    while (!isAtEnd())
        program->statements.push_back(declaration());

    return program;
}

std::unique_ptr<Stmt> Parser::declaration()
{
    if (match(TokenType::FONCTION))
        return functionDeclaration();

    if (match(TokenType::NOMBRE) || match(TokenType::TEXTE) || match(TokenType::BOOLEEN) || match(TokenType::LISTE))
        return variableDeclaration();

    return statement();
}

std::unique_ptr<Stmt> Parser::statement()
{
    if (match(TokenType::SI))
        return ifStatement();

    if (match(TokenType::TANTQUE))
        return whileStatement();

    if (match(TokenType::POUR))
        return forStatement();

    if (match(TokenType::AFFICHER))
        return printStatement();

    if (match(TokenType::LEFT_BRACE))
        return blockStatement();

    return expressionStatement();
}

std::unique_ptr<Stmt> Parser::functionDeclaration()
{
    Token name = consume(TokenType::IDENTIFIER, "Nom de fonction attendu.");
    consume(TokenType::LEFT_PAREN, "'(' attendu apres le nom de fonction.");

    std::vector<std::string> parameters;

    if (!check(TokenType::RIGHT_PAREN))
    {
        do
        {
            Token param = consume(TokenType::IDENTIFIER, "Nom de parametre attendu.");
            parameters.push_back(param.value);
        }
        while (match(TokenType::COMMA));
    }

    consume(TokenType::RIGHT_PAREN, "')' attendu apres les parametres.");
    consume(TokenType::LEFT_BRACE, "'{' attendu pour le corps de la fonction.");

    auto bodyStmt = blockStatement();
    auto* block = dynamic_cast<BlockStmt*>(bodyStmt.get());

    if (!block)
        throw std::runtime_error("Bloc de fonction invalide.");

    std::vector<std::unique_ptr<Stmt>> body = std::move(block->statements);
    return std::make_unique<FunctionStmt>(name.value, std::move(parameters), std::move(body));
}

std::unique_ptr<Stmt> Parser::variableDeclaration()
{
    Token typeToken = previous();
    Token name = consume(TokenType::IDENTIFIER, "Nom de variable attendu.");
    std::unique_ptr<Expr> initializer;

    if (match(TokenType::EQUAL))
        initializer = expression();

    consume(TokenType::SEMICOLON, "';' attendu apres une declaration.");
    return std::make_unique<VarDeclStmt>(typeToken.type, name.value, std::move(initializer));
}

std::unique_ptr<Stmt> Parser::expressionStatement()
{
    auto expr = expression();
    consume(TokenType::SEMICOLON, "';' attendu apres une expression.");
    return std::make_unique<ExpressionStmt>(std::move(expr));
}

std::unique_ptr<Stmt> Parser::printStatement()
{
    consume(TokenType::LEFT_PAREN, "'(' attendu apres 'afficher'.");
    auto expr = expression();
    consume(TokenType::RIGHT_PAREN, "')' attendu apres l'expression de afficher.");
    consume(TokenType::SEMICOLON, "';' attendu apres afficher.");
    return std::make_unique<PrintStmt>(std::move(expr));
}

std::unique_ptr<Stmt> Parser::ifStatement()
{
    consume(TokenType::LEFT_PAREN, "'(' attendu apres 'si'.");
    auto condition = expression();
    consume(TokenType::RIGHT_PAREN, "')' attendu apres la condition.");

    auto thenBranch = statement();
    std::unique_ptr<Stmt> elseBranch;

    if (match(TokenType::SINON))
        elseBranch = statement();

    return std::make_unique<IfStmt>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

std::unique_ptr<Stmt> Parser::whileStatement()
{
    consume(TokenType::LEFT_PAREN, "'(' attendu apres 'tantque'.");
    auto condition = expression();
    consume(TokenType::RIGHT_PAREN, "')' attendu apres la condition.");

    auto body = statement();
    return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
}

std::unique_ptr<Stmt> Parser::forStatement()
{
    consume(TokenType::LEFT_PAREN, "'(' attendu apres 'pour'.");
    Token variable = consume(TokenType::IDENTIFIER, "Variable attendue dans la boucle pour.");

    if (match(TokenType::ALLANT))
    {
        consume(TokenType::DE, "'de' attendu apres 'allant'.");
        auto start = expression();

        if (!(match(TokenType::A) || (check(TokenType::IDENTIFIER) && peek().value == "a" && advance().type == TokenType::IDENTIFIER)))
            throw std::runtime_error("'a' attendu dans la boucle 'allant de ... a ...'.");

        auto end = expression();
        consume(TokenType::RIGHT_PAREN, "')' attendu apres la boucle pour.");

        auto body = statement();
        return std::make_unique<ForRangeStmt>(variable.value, std::move(start), std::move(end), std::move(body));
    }

    if (match(TokenType::DANS))
    {
        auto iterable = expression();
        consume(TokenType::RIGHT_PAREN, "')' attendu apres la boucle pour.");

        auto body = statement();
        return std::make_unique<ForEachStmt>(variable.value, std::move(iterable), std::move(body));
    }

    throw std::runtime_error("Syntaxe invalide pour 'pour'. Utilisez 'allant de ... a ...' ou 'dans'.");
}

std::unique_ptr<Stmt> Parser::blockStatement()
{
    std::vector<std::unique_ptr<Stmt>> statements;

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd())
        statements.push_back(declaration());

    consume(TokenType::RIGHT_BRACE, "'}' attendu pour fermer le bloc.");
    return std::make_unique<BlockStmt>(std::move(statements));
}

std::unique_ptr<Expr> Parser::expression()
{
    auto expr = comparison();

    if (match(TokenType::EQUAL))
    {
        auto value = expression();
        auto* variableExpr = dynamic_cast<VariableExpr*>(expr.get());

        if (variableExpr)
        {
            std::string name = variableExpr->name;
            return std::make_unique<AssignExpr>(name, std::move(value));
        }

        auto* indexExpr = dynamic_cast<IndexExpr*>(expr.get());

        if (indexExpr)
        {
            auto object = std::move(indexExpr->object);
            auto index = std::move(indexExpr->index);
            return std::make_unique<IndexAssignExpr>(std::move(object), std::move(index), std::move(value));
        }

        throw std::runtime_error("Affectation invalide.");
    }

    return expr;
}

std::unique_ptr<Expr> Parser::comparison()
{
    auto expr = term();

    while (
        match(TokenType::EQUAL_EQUAL) ||
        match(TokenType::NOT_EQUAL) ||
        match(TokenType::LESS) ||
        match(TokenType::LESS_EQUAL) ||
        match(TokenType::GREATER) ||
        match(TokenType::GREATER_EQUAL) ||
        match(TokenType::AND) ||
        match(TokenType::OR))
    {
        TokenType op = previous().type;
        auto right = term();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::term()
{
    auto expr = factor();

    while (match(TokenType::PLUS) || match(TokenType::MINUS))
    {
        TokenType op = previous().type;
        auto right = factor();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> Parser::factor()
{
    auto expr = primary();

    while (match(TokenType::STAR) || match(TokenType::SLASH) || match(TokenType::PERCENT))
    {
        TokenType op = previous().type;
        auto right = primary();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::vector<std::unique_ptr<Expr>> Parser::parseArguments()
{
    std::vector<std::unique_ptr<Expr>> arguments;

    if (check(TokenType::RIGHT_PAREN))
        return arguments;

    do
    {
        arguments.push_back(expression());
    }
    while (match(TokenType::COMMA));

    return arguments;
}

std::unique_ptr<Expr> Parser::primary()
{
    std::unique_ptr<Expr> expr;

    if (match(TokenType::NUMBER))
    {
        Value value;
        value.data = std::strtod(previous().value.c_str(), nullptr);
        expr = std::make_unique<LiteralExpr>(std::move(value));
    }
    else if (match(TokenType::STRING))
    {
        Value value;
        value.data = previous().value;
        expr = std::make_unique<LiteralExpr>(std::move(value));
    }
    else if (match(TokenType::IDENTIFIER))
    {
        expr = std::make_unique<VariableExpr>(previous().value);
    }
    else if (match(TokenType::LEFT_PAREN))
    {
        auto innerExpr = expression();
        consume(TokenType::RIGHT_PAREN, "')' attendu apres une expression.");
        expr = std::make_unique<GroupingExpr>(std::move(innerExpr));
    }
    else if (match(TokenType::LEFT_BRACKET))
    {
        std::vector<std::unique_ptr<Expr>> elements;

        if (!check(TokenType::RIGHT_BRACKET))
        {
            do
            {
                elements.push_back(expression());
            }
            while (match(TokenType::COMMA));
        }

        consume(TokenType::RIGHT_BRACKET, "']' attendu apres les elements de liste.");
        expr = std::make_unique<ListExpr>(std::move(elements));
    }
    else if (match(TokenType::NOT) || match(TokenType::MINUS))
    {
        TokenType op = previous().type;
        auto right = primary();
        expr = std::make_unique<UnaryExpr>(op, std::move(right));
    }
    else
    {
        throw std::runtime_error("Expression invalide.");
    }

    while (true)
    {
        if (match(TokenType::LEFT_PAREN))
        {
            auto arguments = parseArguments();
            consume(TokenType::RIGHT_PAREN, "')' attendu apres les arguments.");
            expr = std::make_unique<CallExpr>(std::move(expr), std::move(arguments));
            continue;
        }

        if (match(TokenType::LEFT_BRACKET))
        {
            auto index = expression();
            consume(TokenType::RIGHT_BRACKET, "']' attendu apres l'index.");
            expr = std::make_unique<IndexExpr>(std::move(expr), std::move(index));
            continue;
        }

        if (match(TokenType::DOT))
        {
            Token methodName = consume(TokenType::IDENTIFIER, "Nom de methode attendu apres '.'.");
            consume(TokenType::LEFT_PAREN, "'(' attendu apres le nom de methode.");
            auto arguments = parseArguments();
            consume(TokenType::RIGHT_PAREN, "')' attendu apres les arguments de methode.");
            expr = std::make_unique<MethodCallExpr>(std::move(expr), methodName.value, std::move(arguments));
            continue;
        }

        break;
    }

    return expr;
}