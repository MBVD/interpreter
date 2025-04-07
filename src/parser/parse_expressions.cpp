#include "expression.hpp"
#include "exceptions.hpp"
#include "parser.hpp"
#include <iostream>

Parser::expr_ptr Parser::parse_ternary_expression() {
    auto ternary_expr_index = index;
    auto conditional_expression = parse_binary_expression();
    while (this->tokens[index] == TokenType::QUESTION){
        index++;
        auto true_expr = parse_binary_expression();
        if (this->tokens[index++] != TokenType::COLON) {
            throw parse_ternary_expr_error("");
        }
        auto false_expr = parse_binary_expression();
        conditional_expression = std::make_unique<TernaryExpression>(std::move(conditional_expression), std::move(true_expr), std::move(false_expr));
    }
    return conditional_expression;
}

Parser::expr_ptr Parser::parse_comparison_expression() {
    auto comp_index = index;
    auto left = parse_ternary_expression();
    while (comp_ops.contains(this->tokens[index].type)){
        auto op = this->tokens[index++];
        auto right = parse_ternary_expression();
        left = std::make_unique<ComparisonExpression>(std::move(left), op, std::move(right));
    }
    return left; 
}

Parser::expr_ptr Parser::parse_binary_expression() {
    auto expression = Parser::parse_sum_expression();
    return expression;
}

Parser::expr_ptr Parser::parse_sum_expression() {
    auto left = parse_mul_expression();
    while (this->tokens[index] == TokenType::PLUS || this->tokens[index] == TokenType::MINUS) {
        auto op = this->tokens[index++];
        auto right = parse_mul_expression();
        left = std::make_unique<BinaryExpression>(std::move(left), op, std::move(right));
    }
    return left;
}

Parser::expr_ptr Parser::parse_mul_expression() {
    auto left = parse_pow_expression();
    while (this->tokens[index] == TokenType::MULTIPLY || this->tokens[index] == TokenType::DIVIDE) {
        auto op = this->tokens[index++];
        auto right = parse_pow_expression();
        left = std::make_unique<BinaryExpression>(std::move(left), op, std::move(right));
    }
    return left;
}

Parser::expr_ptr Parser::parse_pow_expression() {
    auto left = parse_unary_expression();
    if (this->tokens[index] == TokenType::POWER) {
        auto op = this->tokens[index++];
        auto right = parse_pow_expression();
        left = std::make_unique<BinaryExpression>(std::move(left), op, std::move(right));
    }
    return left;
}  

Parser::expr_ptr Parser::parse_unary_expression() {
    auto unary_index = index;
    auto unary_op = this->tokens[index++];
    if (unary_ops.contains(unary_op.type)) {
        auto base = parse_base();
        return std::make_unique<UnaryExpression>(std::move(base), unary_op);
    } else {
        index = unary_index;
        return parse_base();
    }
}

Parser::expr_ptr Parser::parse_base() {
    auto base_index = index;
    auto token = this->tokens[index++];
    if (token == TokenType::LITERAL_NUM) {
        return std::make_unique<LiteralExpression>(token);
    }
    if (token == TokenType::ID) {
        return std::make_unique<IDexpression>(token);
    }
    if (token == TokenType::PARENTHESIS_LEFT) {
        auto base = parse_binary_expression();
        if (this->tokens[index++] != TokenType::PARENTHESIS_RIGHT) {
            index = base_index;
            throw parse_base_expr_error("unclosed )");
        }
        return std::make_unique<GroupExpression>(std::move(base));
    }
    index = base_index;
    throw parse_base_expr_error("no base expression");
}

Parser::expr_ptr Parser::parse_post_expression() {
    auto post_index = index;
    Parser::expr_ptr left;
    try {
        left = parse_base();
    } catch (const parse_base_expr_error&) {
        index = post_index;
        throw;
    }
    while (1) {
        auto token = this->tokens[index++];
        if (token == TokenType::INDEX_LEFT) { // subscript
            left = parse_subscript_expression(std::move(left));
            continue;
        } else if (token == TokenType::PARENTHESIS_LEFT) { // call
            left = parse_call_expression(std::move(left));
            continue;
        } else if (token == TokenType::INCREMENT || token == TokenType::DECREMENT) {
            left = std::make_unique<PostfixExpression>(std::move(left), token);
            continue;
        } else if (token == TokenType::ARROW) {
            left = parse_access_expression(std::move(left));
        } else {
            break;
        }
    }
    return left;
}

Parser::access_expr_ptr Parser::parse_access_expression(Parser::expr_ptr base) {
    auto right = parse_post_expression();
    return std::make_unique<AccessExpression>(std::move(base), std::move(right));
}

Parser::subscript_expr_ptr Parser::parse_subscript_expression(Parser::expr_ptr base) {
    std::vector<Parser::expr_ptr> indexes;
    while (this->tokens[index++] != TokenType::INDEX_RIGHT) {
        if (this->tokens[index] == TokenType::COMMA) {
            continue;
        }
        indexes.push_back(parse_expression());
    }
    return std::make_unique<SubscriptExpression>(std::move(base), std::move(indexes));
}

Parser::call_expr_ptr Parser::parse_call_expression(Parser::expr_ptr base) {
    std::vector<Parser::expr_ptr> args;
    while (this->tokens[index++] != TokenType::PARENTHESIS_RIGHT) {
        if (this->tokens[index] == TokenType::COMMA) {
            continue;
        }
        args.push_back(parse_expression());
    }
    return std::make_unique<CallExpression>(std::move(base), std::move(args));
}