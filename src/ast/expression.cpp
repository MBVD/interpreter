#include "expression.hpp"
#include "visitor.hpp"

// BinaryExpression
BinaryExpression::BinaryExpression(std::unique_ptr<Expression> left, Token op, std::unique_ptr<Expression> right)
    : left(std::move(left)), op(op), right(std::move(right)) {}

void BinaryExpression::accept(Visitor& visitor) {
    visitor.visit(this);
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

//CommaExpression
CommaExpression::CommaExpression(std::unique_ptr<Expression> left, Token op, std::unique_ptr<Expression> right)
    : BinaryExpression(std::move(left), op, std::move(right)){}

void CommaExpression::accept(Visitor& visitor){
    visitor.visit(this);
}

//AssignmentExpression
AssignmentExpression::AssignmentExpression(std::unique_ptr<Expression> left, Token op, std::unique_ptr<Expression> right) 
    : BinaryExpression(std::move(left), op, std::move(right)){};

void AssignmentExpression::accept(Visitor& visitor){
    visitor.visit(this);
}

// TernaryExpression
TernaryExpression::TernaryExpression(std::unique_ptr<Expression> cond_expression,
                                     std::unique_ptr<Expression> true_expression,
                                     std::unique_ptr<Expression> false_expression)
    : cond_expression(std::move(cond_expression)),
      true_expression(std::move(true_expression)),
      false_expression(std::move(false_expression)) {}

void TernaryExpression::accept(Visitor& visitor) {
    visitor.visit(this);
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

LogicalOrExpression::LogicalOrExpression(std::unique_ptr<Expression> left, Token op, std::unique_ptr<Expression> right) : BinaryExpression(std::move(left), op, std::move(right)){}

void LogicalOrExpression::accept(Visitor& visitor) {visitor.visit(this);}

LogicalAndExpression::LogicalAndExpression(std::unique_ptr<Expression> left, Token op, std::unique_ptr<Expression> right) : BinaryExpression(std::move(left), op, std::move(right)){}

void LogicalAndExpression::accept(Visitor& visitor) {visitor.visit(this);}

BiteIncOrExpression::BiteIncOrExpression(std::unique_ptr<Expression> left, Token op, std::unique_ptr<Expression> right) : BinaryExpression(std::move(left), op, std::move(right)){} 

void BiteIncOrExpression::accept(Visitor& visitor) {visitor.visit(this);}

BiteExcOrExpression::BiteExcOrExpression(std::unique_ptr<Expression> left, Token op, std::unique_ptr<Expression> right) : BinaryExpression(std::move(left), op, std::move(right)){}

void BiteExcOrExpression::accept(Visitor& visitor) {visitor.visit(this);}

BiteAndExpression::BiteAndExpression(std::unique_ptr<Expression> left, Token op, std::unique_ptr<Expression> right) : BinaryExpression(std::move(left), op, std::move(right)){}

void BiteAndExpression::accept(Visitor& visitor) {visitor.visit(this);}

//ComparisonExpression
ComparisonExpression::ComparisonExpression(std::unique_ptr<Expression> left, Token op, std::unique_ptr<Expression> right)
    : BinaryExpression(std::move(left), op, std::move(right)) {}

void ComparisonExpression::accept(Visitor& visitor) {
    visitor.visit(this);
}
//shift
ShiftExpression::ShiftExpression(std::unique_ptr<Expression> left, Token op, std::unique_ptr<Expression> right) : BinaryExpression(std::move(left), op, std::move(right)){}

void ShiftExpression::accept(Visitor& visitor){
    visitor.visit(this);
}

// UnaryExpression
UnaryExpression::UnaryExpression(std::unique_ptr<Expression> base, const Token& op)
    : base(std::move(base)), op(op) {}

void UnaryExpression::accept(Visitor& visitor) {
    visitor.visit(this);
}

const Token& UnaryExpression::get_op(){
    return this->op;
}
const std::unique_ptr<Expression>& UnaryExpression::get_base(){
    return this->base;
}

// PostfixExpression
PostfixExpression::PostfixExpression(std::unique_ptr<Expression> expression, const Token& op)
    : expression(std::move(expression)), op(op) {}

void PostfixExpression::accept(Visitor& visitor) {
    visitor.visit(this);
}

const Token& PostfixExpression::get_op(){
    return this->op;
}
const std::unique_ptr<Expression>& PostfixExpression::get_expression(){
    return this->expression;
}

// SubscriptExpression
SubscriptExpression::SubscriptExpression(std::unique_ptr<Expression> expression,
                                         std::vector<std::unique_ptr<Expression>> indexes)
    : PostfixExpression(std::move(expression), {TokenType::INDEX_LEFT, "["}), indexes(std::move(indexes)) {}

void SubscriptExpression::accept(Visitor& visitor) {
    visitor.visit(this);
}

const std::vector<std::unique_ptr<Expression>>& SubscriptExpression::get_indexes(){
    return this->indexes;
}

// CallExpression
CallExpression::CallExpression(std::unique_ptr<Expression> expression,
                               std::vector<std::unique_ptr<Expression>> args)
    : PostfixExpression(std::move(expression), {TokenType::PARENTHESIS_LEFT}), args(std::move(args)) {}

void CallExpression::accept(Visitor& visitor) {
    visitor.visit(this);
}

const std::vector<std::unique_ptr<Expression>>& CallExpression::get_args(){
    return this->args;
}

// AccessExpression
AccessExpression::AccessExpression(std::unique_ptr<Expression> expression,
                                   Token member)
    : PostfixExpression(std::move(expression), {TokenType::ARROW}), member(member) {}

void AccessExpression::accept(Visitor& visitor) {
    visitor.visit(this);
}

const Token& AccessExpression::get_member(){
    return this->member;
}

// GroupExpression
GroupExpression::GroupExpression(std::unique_ptr<Expression> base)
    : base(std::move(base)) {}

void GroupExpression::accept(Visitor& visitor) {
    visitor.visit(this);
}

const std::unique_ptr<Expression>& GroupExpression::get_base(){
    return this->base;
}

// LiteralNumExpression
LiteralNumExpression::LiteralNumExpression(const Token& token) {
    value = std::stoi(token.value);
}

void LiteralNumExpression::accept(Visitor& visitor) {
    visitor.visit(this);
}

const int LiteralNumExpression::get_value(){
    return this->value;
}

//LiteralFloatExpression

LiteralFloatExpression::LiteralFloatExpression(const Token& token){
    value = std::stod(token.value);
}

const double LiteralFloatExpression::get_value(){
    return this->value;
}

void LiteralFloatExpression::accept(Visitor& visitor) {
    visitor.visit(this);
}

//LiteralCharExpression

LiteralCharExpression::LiteralCharExpression(const Token& token){
    value = token.value[0];
}

char LiteralCharExpression::get_value(){
    return this->value;
}

void LiteralCharExpression::accept(Visitor& visitor) {
    visitor.visit(this);
}

//LiteralStringExpression

LiteralStringExpression::LiteralStringExpression(const Token& token){
    value = token.value;
}

std::string LiteralStringExpression::get_value(){
    return this->value;
}

void LiteralStringExpression::accept(Visitor& visitor) {
    visitor.visit(this);
}

// IDexpression
IDexpression::IDexpression(const Token& token) : token(token) {}

void IDexpression::accept(Visitor& visitor) {
    visitor.visit(this);
}

const Token& IDexpression::get_token() {
    return this->token;
}