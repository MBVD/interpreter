#include "expression.hpp"
#include "visitor.hpp"

BinaryExpression::BinaryExpression(std::unique_ptr<Expression>& left, Token op, std::unique_ptr<Expression>& right)
    : left(std::move(left)), op(op), right(std::move(right)) {}

void BinaryExpression::accept(Visitor& visitor) { visitor.visit(*this); }

TernaryExpression::TernaryExpression(std::unique_ptr<Expression>& cond_expression,
                                     std::unique_ptr<Expression>& true_expression,
                                     std::unique_ptr<Expression>& false_expression)
    : cond_expression(std::move(cond_expression)),
      true_expression(std::move(true_expression)),
      false_expression(std::move(false_expression)) {}

void TernaryExpression::accept(Visitor& visitor) { visitor.visit(*this); }

UnaryExpression::UnaryExpression(std::unique_ptr<Expression>& base, const Token& op)
    : base(std::move(base)), op(op) {}

void UnaryExpression::accept(Visitor& visitor) { visitor.visit(*this); }

PostfixExpression::PostfixExpression(std::unique_ptr<Expression>& expression, const Token& op)
    : expression(std::move(expression)), op(op) {}

void PostfixExpression::accept(Visitor& visitor) { visitor.visit(*this); }

SubscriptExpression::SubscriptExpression(std::unique_ptr<Expression>& expression,
                                         std::vector<std::unique_ptr<Expression>>& indexes)
    : PostfixExpression(expression, {TokenType::INDEX_LEFT}), indexes(std::move(indexes)) {}

void SubscriptExpression::accept(Visitor& visitor) { visitor.visit(*this); }

CallExpression::CallExpression(std::unique_ptr<Expression>& expression,
                               std::vector<std::unique_ptr<Expression>>& args)
    : PostfixExpression(expression, {TokenType::PARENTHESIS_LEFT}), args(std::move(args)) {}

void CallExpression::accept(Visitor& visitor) { visitor.visit(*this); }

AccessExpression::AccessExpression(std::unique_ptr<Expression>& expression,
                                   std::unique_ptr<Expression>& expression_to_access)
    : PostfixExpression(expression, {TokenType::ARROW}), expression_to_access(std::move(expression_to_access)) {}

void AccessExpression::accept(Visitor& visitor) { visitor.visit(*this); }

GroupExpression::GroupExpression(std::unique_ptr<Expression>& base)
    : base(std::move(base)) {}

void GroupExpression::accept(Visitor& visitor) { visitor.visit(*this); }