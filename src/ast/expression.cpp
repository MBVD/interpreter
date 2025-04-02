#include "expression.hpp"

BinaryExpression::BinaryExpression(std::unique_ptr<Expression>& left, Token op, std::unique_ptr<Expression>& right)
    : left(std::move(left)), op(op), right(std::move(right)) {}

TernaryExpression::TernaryExpression(std::unique_ptr<Expression>& cond_expression,
                                     std::unique_ptr<Expression>& true_expression,
                                     std::unique_ptr<Expression>& false_expression)
    : cond_expression(std::move(cond_expression)),
    true_expression(std::move(true_expression)),
    false_expression(std::move(false_expression)) {}

UnaryExpression::UnaryExpression(std::unique_ptr<Expression>& base, const Token& op)
    : base(std::move(base)), op(op) {}

PostfixExpression::PostfixExpression(std::unique_ptr<Expression>& expression, const Token& op)
    : expression(std::move(expression)), op(op) {}

SubscriptExpression::SubscriptExpression(std::unique_ptr<Expression>& expression,
                    std::vector<std::unique_ptr<Expression>>& indexes)
                : PostfixExpression(expression, {TokenType::INDEX_LEFT}), indexes(std::move(indexes)) {}

CallExpression::CallExpression(std::unique_ptr<Expression>& expression,
                               std::vector<std::unique_ptr<Expression>>& args)
    : PostfixExpression(expression, {TokenType::PARENTHESIS_LEFT}), args(std::move(args)) {}

AccessExpression::AccessExpression(std::unique_ptr<Expression>& expression,
                                   std::unique_ptr<Expression>& expression_to_access)
    : PostfixExpression(expression, {TokenType::ARROW}), expression_to_access(std::move(expression_to_access)) {}

GroupExpression::GroupExpression(std::unique_ptr<Expression>& base)
    : base(std::move(base)) {}