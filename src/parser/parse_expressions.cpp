#include "expression.hpp"
#include "exceptions.hpp"
#include "parser.hpp"
#include <iostream>

Parser::ternary_expr_ptr Parser::parse_ternary_expression() {
    auto ternary_expr_index = index;
    Parser::expr_ptr conditional_expression;
    try {
        conditional_expression = parse_expression();
    } catch (const expression_parsing_error&){
        index = ternary_expr_index;
        throw parse_ternary_expr_error("");
    }
    auto q_token = this->tokens[index++];
    if (q_token != TokenType::QUESTION){
        index = ternary_expr_index;
        throw parse_ternary_expr_error("no ?");
    }
    Parser::expr_ptr true_expr;
    try {
        true_expr = parse_expression();
    } catch (const expression_parsing_error&) {
        index = ternary_expr_index;
        throw parse_ternary_expr_error("");
    }
    auto colon_token = this->tokens[index++];
    if (colon_token != TokenType::COLON){
        index = ternary_expr_index;
        throw parse_ternary_expr_error("expected :");
    }
    Parser::expr_ptr false_expr;
    try {
        false_expr = parse_expression();
        return std::make_unique<TernaryExpression>(conditional_expression, true_expr, false_expr);
    } catch (const expression_parsing_error&){ 
        index = ternary_expr_index;
        throw parse_ternary_expr_error("expected expression");
    }
}

Parser::expr_ptr Parser::parse_binary_expression() {
    auto expression = Parser::parse_sum_expression();
    return expression;
}

Parser::expr_ptr Parser::parse_sum_expression() {
    auto left = parse_mul_expression();
    while (this->tokens[index] == TokenType::PLUS || this->tokens[index] == TokenType::MINUS){
        auto op = this->tokens[index++];
        auto right = parse_sum_expression();
        left = std::make_unique<BinaryExpression>(left, op, right);
    }
    return left;
}

Parser::expr_ptr Parser::parse_mul_expression() {
    auto left = parse_pow_expression();
    while (this->tokens[index] == TokenType::MULTIPLY || this->tokens[index] == TokenType::DIVIDE){
        auto op = this->tokens[index++];
        auto right = parse_mul_expression();
        left = std::make_unique<BinaryExpression>(left, op, right);
    }
    return left;
}

Parser::expr_ptr Parser::parse_pow_expression() {
    auto left = parse_unary_expression();
    if (this->tokens[index] == TokenType::POWER){
        auto op = this->tokens[index];
        auto right = parse_pow_expression();
        left = std::make_unique<BinaryExpression>(left, op, right);
    }
    return left;
}

std::unordered_set<TokenType> Parser::unary_ops = {TokenType::INCREMENT, TokenType::DECREMENT, TokenType::PLUS, TokenType::MINUS, TokenType::ID, TokenType::TYPE}; // ++ -- + -  
 

Parser::expr_ptr Parser::parse_unary_expression() {
    auto unary_index = index;
    auto unary_op = this->tokens[index++];
    if (unary_ops.contains(unary_op.type)){
        auto base = parse_base();
        return std::make_unique<UnaryExpression>(base, unary_op);
    } else {
        index = unary_index;
        return parse_base();
    }
} 

Parser::expr_ptr Parser::parse_base() {
    auto base_index = index;
    auto token = this->tokens[index++];
    if (token == TokenType::LITERAL_NUM){
        return std::make_unique<LiteralExpression>(token);
    }
    if (token == TokenType::ID) {
        return std::make_unique<IDexpression>(token);
    }
    if (token == TokenType::PARENTHESIS_LEFT) {
        index++;
        auto base = parse_binary_expression();
        if (this->tokens[index++] != TokenType::PARENTHESIS_RIGHT){
            index = base_index;
            throw parse_base_expr_error("unclosed )");
        }
        return std::make_unique<GroupExpression>(base);
    }
    index = base_index;
    throw parse_base_expr_error("no base expression"); // TODO dodelat'
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
        if (token == TokenType::INDEX_LEFT){ // subscript
            left = parse_subscript_expression(left);
            continue;
        } else if (token == TokenType::PARENTHESIS_LEFT) { // call
            left = parse_call_expression(left);
            continue;
        } else if (token == TokenType::INCREMENT || token == TokenType::DECREMENT){
            left = std::make_unique<PostfixExpression>(left, token);
            index++;
            continue;
        } if (token == TokenType::ARROW){
            left = parse_access_expression(left);
        } else {
            break;
        }
    }
    return left;
}

Parser::access_expr_ptr Parser::parse_access_expression(Parser::expr_ptr& base) {
    auto right = parse_post_expression();
    return std::make_unique<AccessExpression>(base, right);
}

Parser::subscript_expr_ptr Parser::parse_subscript_expression(Parser::expr_ptr& base) {
    std::vector<Parser::expr_ptr> indexes;
    while (this->tokens[index++] != TokenType::INDEX_RIGHT){
        if (this->tokens[index] == TokenType::COMMA) {
            continue;
        }
        indexes.push_back(parse_expression());
    }
    return std::make_unique<SubscriptExpression>(base, indexes);
}

Parser::call_expr_ptr Parser::parse_call_expression(Parser::expr_ptr& base) {
    std::vector<Parser::expr_ptr> args;
    while (this->tokens[index++] != TokenType::PARENTHESIS_RIGHT){
        if (this->tokens[index] == TokenType::COMMA) {
            continue;
        }
        args.push_back(parse_expression());
    }
    return std::make_unique<CallExpression>(base, args);
}