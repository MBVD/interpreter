#include "expression.hpp"
#include "exceptions.hpp"
#include "parser.hpp"
#include <iostream>

Parser::expr_ptr Parser::parse_binary_expression() {
    auto expression = parse_assignment_expression();
    return expression;
}

Parser::expr_ptr Parser::parse_assignment_expression() {// 1 ? x : y = ...
    auto left = parse_comparison_expression(); // ++ X ++ = ... (int) x = ...
    if (asssign_ops.contains(this->tokens[index].type)){ // TODO make set
        auto op = tokens[index++];
        auto right = parse_assignment_expression();
        left = std::make_unique<AssignmentExpression>(std::move(left), op, std::move(right));
    }
    return left;
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

Parser::expr_ptr Parser::parse_ternary_expression() { // 1 + 2 ... || 1 < 2 ? x : y ? y : x =
    auto ternary_expr_index = index;
    auto conditional_expression = parse_sum_expression();
    while (this->tokens[index] == TokenType::QUESTION){
        index++;
        auto true_expr = parse_comparison_expression();
        if (this->tokens[index++] != TokenType::COLON) {
            throw parse_ternary_expr_error("");
        }
        auto false_expr = parse_binary_expression();
        conditional_expression = std::make_unique<TernaryExpression>(std::move(conditional_expression), std::move(true_expr), std::move(false_expr));
    }
    return conditional_expression;
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
    auto unary_op = this->tokens[index];
    if (unary_ops.contains(unary_op.type)) {
        index++;
        auto base = parse_post_expression();
        return std::make_unique<UnaryExpression>(std::move(base), unary_op);
    } else {
        index = unary_index;
        return parse_post_expression();
    }
}

Parser::expr_ptr Parser::parse_post_expression() {
    auto post_index = index;
    Parser::expr_ptr left;
    left = parse_base();
    while (post_ops.contains(this->tokens[index].type)) { // TODO make set
        auto op = tokens[index++];
        left = parse_post_helper(op, std::move(left));
    }
    return left;
}

Parser::expr_ptr Parser::parse_post_helper(Token op, expr_ptr base){
    auto helper_index = index;
    if (op == TokenType::ARROW){
        return parse_access_expression(std::move(base));
    } else if (op == TokenType::INDEX_LEFT){
         return parse_subscript_expression(std::move(base));
    } else if (op == TokenType::PARENTHESIS_LEFT){
        return parse_call_expression(std::move(base));
    } else if (op == TokenType::INCREMENT || op == TokenType::DECREMENT){
        return parse_increment_expression(std::move(base), op);
    }
    throw expression_parsing_error("");
}

Parser::expr_ptr Parser::parse_access_expression(Parser::expr_ptr base) {// ->
    if (index >= this->tokens.size() || this->tokens[index] != TokenType::ID) {
        throw expression_parsing_error("Expected identifier after '->'");
    }

    auto member = this->tokens[index++];
    return std::make_unique<AccessExpression>(std::move(base), member);
}

Parser::expr_ptr Parser::parse_subscript_expression(expr_ptr base) { // []
    std::vector<Parser::expr_ptr> indexes;

    while (index < this->tokens.size() && this->tokens[index] != TokenType::INDEX_RIGHT) {
        if (this->tokens[index] == TokenType::COMMA) {
            index++;
            continue;
        }
        indexes.push_back(parse_expression());
    }

    if (index >= this->tokens.size() || this->tokens[index++] != TokenType::INDEX_RIGHT) {
        throw expression_parsing_error("Expected ']' to close subscript expression");
    }

    return std::make_unique<SubscriptExpression>(std::move(base), std::move(indexes));
}

Parser::expr_ptr Parser::parse_call_expression(expr_ptr base) { // ()
    std::vector<Parser::expr_ptr> args;

    while (index < this->tokens.size() && this->tokens[index] != TokenType::PARENTHESIS_RIGHT) {
        if (this->tokens[index++] == TokenType::COMMA) {
            continue;
        }

        args.push_back(parse_expression());
    }

    if (index >= this->tokens.size() || this->tokens[index++] != TokenType::PARENTHESIS_RIGHT) {
        throw expression_parsing_error("Expected ')' to close function call");
    }

    return std::make_unique<CallExpression>(std::move(base), std::move(args));
}

Parser::expr_ptr Parser::parse_increment_expression(Parser::expr_ptr base, Token op) {// ++ --
    return std::make_unique<PostfixExpression>(std::move(base), op);
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