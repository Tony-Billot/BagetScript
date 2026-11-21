#include "common/token.hpp"
#include "ast/statements/declaration.hpp"
#include "ast/statements/print_statement.hpp"
#include "ast/statements/assignment.hpp"
#include "ast/statements/if_statement.hpp"
#include "ast/statements/function.hpp"
#include "ast/expressions/function_call_expression.hpp"

#include <vector>

class Parser
{
    private:
        std::vector<Token> tokens;
        int position = 0;

        Token consume(TokenType expected);
        std::string token_type_to_string(TokenType type);
        std::unique_ptr<Expression> operate(std::unique_ptr<Expression> left, Token operator_token, std::unique_ptr<Expression> right);
        bool is_current(TokenType type);
        std::unique_ptr<Expression> parse_expression();
        std::unique_ptr<Expression> parse_logical_or();
        std::unique_ptr<Expression> parse_logical_and();
        std::unique_ptr<Expression> parse_equality();
        std::unique_ptr<Expression> parse_addition();
        std::unique_ptr<Expression> parse_multiplication();
        std::unique_ptr<Expression> parse_value();
        std::unique_ptr<Expression> parse_unary();
        std::vector<std::unique_ptr<Statement>> parse_block();
        std::unique_ptr<FunctionCallExpression> parse_function_call_expression();
        std::unique_ptr<ReturnStatement> parse_return();
    public:
        Parser(std::vector<Token> tokens);
        Token current_token();
        bool has_more_tokens() const;
        bool has_assignment_syntax();
        bool has_function_call_syntax();
        Declaration parse_declaration();
        PrintStatement parse_print_statement();
        Assignment parse_assignment();
        IfStatement parse_if_statement();
        std::unique_ptr<FunctionStatement> parse_function();
        std::unique_ptr<FunctionCallExpression> parse_function_call();
};