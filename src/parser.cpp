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
    {
        std::unique_ptr<Stmt> stmt;

        if (match(TokenType::ENTIER) || match(TokenType::TEXTE) || match(TokenType::BOOLEEN))
            stmt = variableDeclaration();
        else
            stmt = statement();

        program->statements.push_back(std::move(stmt));
    }

    return program;
}

std::unique_ptr<Stmt> Parser::statement()
{
    if (match(TokenType::SI))
        return ifStatement();

    if (match(TokenType::TANTQUE))
        return whileStatement();

    if (match(TokenType::AFFICHER))
        return printStatement();

    if (match(TokenType::LEFT_BRACE))
        return blockStatement();

    return expressionStatement();
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

std::unique_ptr<Stmt> Parser::blockStatement()
{
    std::vector<std::unique_ptr<Stmt>> statements;

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd())
    {
        if (match(TokenType::ENTIER) || match(TokenType::TEXTE) || match(TokenType::BOOLEEN))
            statements.push_back(variableDeclaration());
        else
            statements.push_back(statement());
    }

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

        if (!variableExpr)
            throw std::runtime_error("Affectation invalide.");

        std::string name = variableExpr->name;
        return std::make_unique<AssignExpr>(name, std::move(value));
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

std::unique_ptr<Expr> Parser::primary()
{
    if (match(TokenType::NUMBER))
    {
        Value value;
        value.data = std::strtod(previous().value.c_str(), nullptr);
        return std::make_unique<LiteralExpr>(std::move(value));
    }

    if (match(TokenType::STRING))
    {
        Value value;
        value.data = previous().value;
        return std::make_unique<LiteralExpr>(std::move(value));
    }

    if (match(TokenType::IDENTIFIER))
        return std::make_unique<VariableExpr>(previous().value);

    if (match(TokenType::LEFT_PAREN))
    {
        auto expr = expression();
        consume(TokenType::RIGHT_PAREN, "')' attendu apres une expression.");
        return std::make_unique<GroupingExpr>(std::move(expr));
    }

    if (match(TokenType::NOT) || match(TokenType::MINUS))
    {
        TokenType op = previous().type;
        auto right = primary();
        return std::make_unique<UnaryExpr>(op, std::move(right));
    }

    throw std::runtime_error("Expression invalide.");
}