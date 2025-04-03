#include "expression.hpp"
#include "visitor.hpp"

// BinaryExpression
BinaryExpression::BinaryExpression(std::unique_ptr<Expression>& left, Token op, std::unique_ptr<Expression>& right)
    : left(std::move(left)), op(op), right(std::move(right)) {}


BinaryExpression::BinaryExpression(BinaryExpression& other)
    : left(std::move(other.left)), op(other.op), right(std::move(other.right)) {}

void BinaryExpression::accept(Visitor& visitor) {
    visitor.visit(std::make_unique<BinaryExpression>(*this));
}

const Token& BinaryExpression::get_op(){
    return this->op;
}

const std::unique_ptr<Expression>& BinaryExpression::get_left() {
    return this->left;
}

const std::unique_ptr<Expression>& BinaryExpression::get_right() {
    return this->right;
}

// TernaryExpression
TernaryExpression::TernaryExpression(std::unique_ptr<Expression>& cond_expression,
                                     std::unique_ptr<Expression>& true_expression,
                                     std::unique_ptr<Expression>& false_expression)
    : cond_expression(std::move(cond_expression)),
      true_expression(std::move(true_expression)),
      false_expression(std::move(false_expression)) {}

TernaryExpression::TernaryExpression(TernaryExpression& other)
    : cond_expression(std::move(other.cond_expression)),
      true_expression(std::move(other.true_expression)),
      false_expression(std::move(other.false_expression)) {}

void TernaryExpression::accept(Visitor& visitor) {
    visitor.visit(std::make_unique<TernaryExpression>(*this));
}

const std::unique_ptr<Expression>& TernaryExpression::get_cond_expression(){
    return this->cond_expression;
}

const std::unique_ptr<Expression>& TernaryExpression::get_true_expression(){
    return this->true_expression;
}

const std::unique_ptr<Expression>& TernaryExpression::get_false_expression(){
    return this->false_expression;
}

// UnaryExpression
UnaryExpression::UnaryExpression(std::unique_ptr<Expression>& base, const Token& op)
    : base(std::move(base)), op(op) {}


UnaryExpression::UnaryExpression(UnaryExpression& other)
    : base(std::move(other.base)), op(other.op) {}

void UnaryExpression::accept(Visitor& visitor) {
    visitor.visit(std::make_unique<UnaryExpression>(*this));
}

const Token& UnaryExpression::get_op(){
    return this->op;
}
const std::unique_ptr<Expression>& UnaryExpression::get_base(){
    return this->base;
}

// PostfixExpression
PostfixExpression::PostfixExpression(std::unique_ptr<Expression>& expression, const Token& op)
    : expression(std::move(expression)), op(op) {}

PostfixExpression::PostfixExpression(PostfixExpression& other)
    : expression(std::move(other.expression)), op(other.op) {}

void PostfixExpression::accept(Visitor& visitor) {
    visitor.visit(std::make_unique<PostfixExpression>(*this));
}

const Token& PostfixExpression::get_op(){
    return this->op;
}
const std::unique_ptr<Expression>& PostfixExpression::get_expression(){
    return this->expression;
}

// SubscriptExpression
SubscriptExpression::SubscriptExpression(std::unique_ptr<Expression>& expression,
                                         std::vector<std::unique_ptr<Expression>>& indexes)
    : PostfixExpression(expression, {TokenType::INDEX_LEFT}), indexes(std::move(indexes)) {}

SubscriptExpression::SubscriptExpression(SubscriptExpression& other)
    : PostfixExpression(other), indexes(std::move(other.indexes)) {}

void SubscriptExpression::accept(Visitor& visitor) {
    visitor.visit(std::make_unique<SubscriptExpression>(*this));
}

const std::vector<std::unique_ptr<Expression>>& SubscriptExpression::get_indexes(){
    return this->indexes;
}

// CallExpression
CallExpression::CallExpression(std::unique_ptr<Expression>& expression,
                               std::vector<std::unique_ptr<Expression>>& args)
    : PostfixExpression(expression, {TokenType::PARENTHESIS_LEFT}), args(std::move(args)) {}

CallExpression::CallExpression(CallExpression& other)
    : PostfixExpression(other), args(std::move(other.args)) {}

void CallExpression::accept(Visitor& visitor) {
    visitor.visit(std::make_unique<CallExpression>(*this));
}

const std::vector<std::unique_ptr<Expression>>& CallExpression::get_args(){
    return this->args;
}

// AccessExpression
AccessExpression::AccessExpression(std::unique_ptr<Expression>& expression,
                                   std::unique_ptr<Expression>& expression_to_access)
    : PostfixExpression(expression, {TokenType::ARROW}), expression_to_access(std::move(expression_to_access)) {}

AccessExpression::AccessExpression(AccessExpression& other)
    : PostfixExpression(other), expression_to_access(std::move(other.expression_to_access)) {}

void AccessExpression::accept(Visitor& visitor) {
    visitor.visit(std::make_unique<AccessExpression>(*this));
}

const std::unique_ptr<Expression>& AccessExpression::get_expression_to_access(){
    return this->expression_to_access;
}

// GroupExpression
GroupExpression::GroupExpression(std::unique_ptr<Expression>& base)
    : base(std::move(base)) {}

GroupExpression::GroupExpression(GroupExpression& other)
    : base(std::move(other.base)) {}

// LiteralExpression
LiteralExpression::LiteralExpression(const Token& token) : token(token) {}

LiteralExpression::LiteralExpression(LiteralExpression& other)
    : token(other.token) {}

// IDexpression
IDexpression::IDexpression(const Token& token) : token(token) {}

IDexpression::IDexpression(IDexpression& other)
    : token(other.token) {}

void GroupExpression::accept(Visitor& visitor) {
    visitor.visit(std::make_unique<GroupExpression>(*this));
}

void LiteralExpression::accept(Visitor& visitor) {
    visitor.visit(std::make_unique<LiteralExpression>(*this));
}

const Token& LiteralExpression::get_token(){
    return this->token;
}

void IDexpression::accept(Visitor& visitor) {
    visitor.visit(std::make_unique<IDexpression>(*this));
}

const Token& IDexpression::get_token() {
    return this->token;
}

const std::unique_ptr<Expression>& GroupExpression::get_base(){
    return this->base;
}
