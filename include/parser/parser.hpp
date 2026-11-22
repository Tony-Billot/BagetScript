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

        Token current_token();
        bool is_current(TokenType type);
        Token consume(TokenType expected);
        bool has_assignment_syntax();
        bool has_function_call_syntax();

        std::unique_ptr<Statement> parse_statement();
        Declaration parse_declaration();
        Assignment parse_assignment();
        PrintStatement parse_print_statement();
        IfStatement parse_if_statement();
        std::unique_ptr<FunctionStatement> parse_function();
        std::unique_ptr<FunctionCallExpression> parse_function_call();
        std::unique_ptr<ReturnStatement> parse_return();
        std::vector<std::unique_ptr<Statement>> parse_block();

        std::unique_ptr<Expression> parse_expression();
        std::unique_ptr<Expression> parse_logical_or();
        std::unique_ptr<Expression> parse_logical_and();
        std::unique_ptr<Expression> parse_equality();
        std::unique_ptr<Expression> parse_addition();
        std::unique_ptr<Expression> parse_multiplication();
        std::unique_ptr<Expression> parse_unary();
        std::unique_ptr<Expression> parse_value();
        std::unique_ptr<FunctionCallExpression> parse_function_call_expression();
        std::unique_ptr<Expression> operate(std::unique_ptr<Expression> left, Token operator_token, std::unique_ptr<Expression> right);
        
    public:
        Parser(std::vector<Token> tokens);
        std::vector<std::unique_ptr<Statement>> parse();
};