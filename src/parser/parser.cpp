#include "parser/parser.hpp"
#include "ast/expressions/identifier_expression.hpp"
#include "ast/expressions/number_expression.hpp"
#include "ast/expressions/text_expression.hpp"
#include "ast/expressions/boolean_expression.hpp"
#include "common/exceptions.hpp"
#include <memory>
#include "ast/expressions/binary_expression.hpp"
#include "ast/expressions/unary_expression.hpp"

Parser::Parser(std::vector<Token> tokens)
{
    this->tokens = tokens;
}

bool Parser::has_more_tokens() const
{
    return tokens[position].type != TokenType::FIN_DE_FICHIER;
}

Token Parser::current_token()
{
    return tokens[position];
}

Token Parser::consume(TokenType expected)
{
    if (current_token().type != expected)
    {
        throw SyntaxError(token_type_to_string(expected) + " attendu", current_token().line, current_token().column);
    }

    return tokens[position++];
}


Declaration Parser::parse_declaration()
{
    Declaration declaration;
    declaration.type = consume(current_token().type).type;
    declaration.name = consume(TokenType::IDENTIFIANT).valeur;
    consume(TokenType::A_POUR_VALEUR);
    declaration.value = parse_expression();
    consume(TokenType::POINT_VIRGULE);
    return declaration;
}

PrintStatement Parser::parse_print_statement()
{
    PrintStatement statement;

    consume(TokenType::AFFICHER);
    consume(TokenType::PARENTHESE_GAUCHE);
    statement.expression = parse_expression();
    consume(TokenType::PARENTHESE_DROITE);
    consume(TokenType::POINT_VIRGULE);

    return statement;
}

Assignment Parser::parse_assignment()
{
    Assignment assignment;

    assignment.name = consume(TokenType::IDENTIFIANT).valeur;
    consume(TokenType::A_POUR_VALEUR);
    assignment.value = parse_expression();
    consume(TokenType::POINT_VIRGULE);
    return assignment;
}

std::unique_ptr<Statement> Parser::parse_statement()
{
    if (is_current(TokenType::AFFICHER))
    {
        return std::make_unique<PrintStatement>(parse_print_statement());
    }

    if (is_current(TokenType::SI))
    {
        return std::make_unique<IfStatement>(parse_if_statement());
    }

    if (is_current(TokenType::TYPE_NOMBRE) || is_current(TokenType::TYPE_TEXTE) || is_current(TokenType::TYPE_BOOLEEN))
    {
        return std::make_unique<Declaration>(parse_declaration());
    }

    if (is_current(TokenType::FONCTION))
    {
        return parse_function();
    }

    if (is_current(TokenType::RETOURNER))
    {
        return parse_return();
    }

    if (is_current(TokenType::IDENTIFIANT))
    {
        if (has_assignment_syntax())
        {
            return std::make_unique<Assignment>(parse_assignment());
        }

        if (has_function_call_syntax())
        {
            auto call = std::make_unique<FunctionCallStatement>();
            call->call = parse_function_call();
            return call;
        }
    }

    Token token = current_token();
    throw SyntaxError(
        "instruction attendue",
        token.line,
        token.column);
}

std::vector<std::unique_ptr<Statement>> Parser::parse_block()
{
    consume(TokenType::ACCOLADE_GAUCHE);
    std::vector<std::unique_ptr<Statement>> statements;

    while (!is_current(TokenType::ACCOLADE_DROITE))
    {
        statements.push_back(parse_statement());
    }

    consume(TokenType::ACCOLADE_DROITE);
    return statements;
}

IfStatement Parser::parse_if_statement()
{
    IfStatement statement;

    consume(TokenType::SI);
    consume(TokenType::PARENTHESE_GAUCHE);
    statement.condition = parse_expression();
    consume(TokenType::PARENTHESE_DROITE);
    statement.statements = parse_block();

    while (is_current(TokenType::SINON))
    {
        consume(TokenType::SINON);

        if (is_current(TokenType::SI))
        {
            consume(TokenType::SI);
            consume(TokenType::PARENTHESE_GAUCHE);

            ElseIfBranch branch;
            branch.condition = parse_expression();
            consume(TokenType::PARENTHESE_DROITE);
            branch.statements = parse_block();
            statement.else_if_branches.push_back(std::move(branch));
        }
        else
        {
            statement.else_statements = parse_block();
            break;
        }
    }

    return statement;
}

std::unique_ptr<Expression> Parser::operate(std::unique_ptr<Expression> left, Token operator_token, std::unique_ptr<Expression> right)
{
    auto operation = std::make_unique<BinaryExpression>();
    operation->left = std::move(left);
    operation->operation = operator_token.type;
    operation->right = std::move(right);
    operation->line = operator_token.line;
    operation->column = operator_token.column;
    return operation;
}

std::unique_ptr<Expression> Parser::parse_expression()
{
    return parse_logical_or();
}

std::unique_ptr<Expression> Parser::parse_logical_or()
{
    auto result = parse_logical_and();

    while (is_current(TokenType::OU))
    {
        Token operator_token = consume(TokenType::OU);
        auto right_side = parse_logical_and();
        result = operate(std::move(result), operator_token, std::move(right_side));
    }

    return result;
}

std::unique_ptr<Expression> Parser::parse_logical_and()
{
    auto result = parse_equality();

    while (is_current(TokenType::ET))
    {
        Token operator_token = consume(TokenType::ET);
        auto right_side = parse_equality();
        result = operate(std::move(result), operator_token, std::move(right_side));
    }

    return result;
}

std::unique_ptr<Expression> Parser::parse_equality()
{
    auto result = parse_addition();

    while (is_current(TokenType::EST_EGAL_A)
        || is_current(TokenType::EST_INFERIEUR_A)
        || is_current(TokenType::EST_SUPERIEUR_A))
    {
        Token operator_token = consume(current_token().type);
        auto right_side = parse_addition();
        result = operate(std::move(result), operator_token, std::move(right_side));
    }

    return result;
}

std::unique_ptr<Expression> Parser::parse_addition()
{
    auto result = parse_multiplication();

    while (is_current(TokenType::PLUS) || is_current(TokenType::MOINS))
    {
        Token operator_token = consume(current_token().type);
        auto right_side = parse_multiplication();
        result = operate(std::move(result), operator_token, std::move(right_side));
    }

    return result;
}

std::unique_ptr<Expression> Parser::parse_multiplication()
{
    auto result = parse_unary();

    while (is_current(TokenType::FOIS) || is_current(TokenType::DIVISE))
    {
        Token operator_token = consume(current_token().type);
        auto right_side = parse_unary();
        result = operate(std::move(result), operator_token, std::move(right_side));
    }

    return result;
}

std::unique_ptr<Expression> Parser::parse_value()
{
    Token token = current_token();

    if (is_current(TokenType::NOMBRE))
    {
        consume(TokenType::NOMBRE);
        auto number = std::make_unique<NumberExpression>();
        number->value = std::stod(token.valeur);
        number->line = token.line;
        number->column = token.column;
        return number;
    }

    if (is_current(TokenType::TEXTE))
    {
        consume(TokenType::TEXTE);
        auto text = std::make_unique<TextExpression>();
        text->value = token.valeur;
        text->line = token.line;
        text->column = token.column;
        return text;
    }

    if (is_current(TokenType::VRAI) || is_current(TokenType::FAUX))
    {
        Token token = consume(current_token().type);
        auto boolean = std::make_unique<BooleanExpression>();
        boolean->value = token.type == TokenType::VRAI;
        boolean->line = token.line;
        boolean->column = token.column;
        return boolean;
    }

    if (is_current(TokenType::NON))
    {
        Token operator_token = consume(TokenType::NON);

        auto non = std::make_unique<UnaryExpression>();
        non->operation = operator_token.type;
        non->value = parse_unary();
        non->line = operator_token.line;
        non->column = operator_token.column;

        return non;
    }

    if (is_current(TokenType::IDENTIFIANT))
    {
        if (position + 1 < static_cast<int>(tokens.size())
            && tokens[position + 1].type == TokenType::PARENTHESE_GAUCHE)
        {
            return parse_function_call_expression();
        }

        consume(TokenType::IDENTIFIANT);
        auto variable = std::make_unique<IdentifierExpression>();
        variable->name = token.valeur;
        variable->line = token.line;
        variable->column = token.column;
        return variable;
    }

    if (is_current(TokenType::PARENTHESE_GAUCHE))
    {
        consume(TokenType::PARENTHESE_GAUCHE);
        auto inside = parse_expression();
        consume(TokenType::PARENTHESE_DROITE);
        return inside;
    }

    throw SyntaxError("nombre, texte, variable ou parenthèse attendu", token.line, token.column);
}


std::unique_ptr<Expression> Parser::parse_unary()
{
    if (is_current(TokenType::MOINS))
    {
        Token operator_token = consume(TokenType::MOINS);
        auto negative = std::make_unique<UnaryExpression>();
        negative->operation = operator_token.type;
        negative->value = parse_unary();
        negative->line = operator_token.line;
        negative->column = operator_token.column;
        return negative;
    }

    return parse_value();
}


std::unique_ptr<FunctionStatement> Parser::parse_function()
{
    consume(TokenType::FONCTION);
    Token name = consume(TokenType::IDENTIFIANT);

    auto function = std::make_unique<FunctionStatement>();
    function->name = name.valeur;

    consume(TokenType::PARENTHESE_GAUCHE);

    if (!is_current(TokenType::PARENTHESE_DROITE))
    {
        while (true)
        {
            Token parameter_type = current_token();
            if (parameter_type.type != TokenType::TYPE_NOMBRE
                && parameter_type.type != TokenType::TYPE_TEXTE
                && parameter_type.type != TokenType::TYPE_BOOLEEN)
            {
                throw SyntaxError("type de paramètre nombre, texte ou booléen attendu", parameter_type.line, parameter_type.column);
            }
            consume(parameter_type.type);

            Token parameter_name = consume(TokenType::IDENTIFIANT);
            function->parameters.push_back({parameter_type.type, parameter_name.valeur});

            if (!is_current(TokenType::VIRGULE)) {
                break;
            }

            consume(TokenType::VIRGULE);
        }
    }

    consume(TokenType::PARENTHESE_DROITE);
    function->statements = parse_block();

    return function;
}

std::unique_ptr<FunctionCallExpression> Parser::parse_function_call_expression()
{
    Token name = consume(TokenType::IDENTIFIANT);
    consume(TokenType::PARENTHESE_GAUCHE);

    auto call = std::make_unique<FunctionCallExpression>();
    call->name = name.valeur;
    call->line = name.line;
    call->column = name.column;

    if (!is_current(TokenType::PARENTHESE_DROITE))
    {
        while (true)
        {
            call->arguments.push_back(parse_expression());

            if (!is_current(TokenType::VIRGULE))
                break;

            consume(TokenType::VIRGULE);
        }
    }

    consume(TokenType::PARENTHESE_DROITE);
    return call;
}

std::unique_ptr<FunctionCallExpression> Parser::parse_function_call()
{
    auto call = parse_function_call_expression();
    consume(TokenType::POINT_VIRGULE);
    return call;
}

std::unique_ptr<ReturnStatement> Parser::parse_return()
{
    consume(TokenType::RETOURNER);

    auto statement = std::make_unique<ReturnStatement>();
    statement->value = parse_expression();
    consume(TokenType::POINT_VIRGULE);
    return statement;
}

bool Parser::has_function_call_syntax()
{
    return position + 1 < static_cast<int>(tokens.size())
        && tokens[position].type == TokenType::IDENTIFIANT
        && tokens[position + 1].type == TokenType::PARENTHESE_GAUCHE;
}

bool Parser::is_current(TokenType type)
{
    return current_token().type == type;
}

bool Parser::has_assignment_syntax()
{
    return position + 1 < static_cast<int>(tokens.size())
        && tokens[position].type == TokenType::IDENTIFIANT
        && tokens[position + 1].type == TokenType::A_POUR_VALEUR;
}
