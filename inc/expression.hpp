#pragma once
#include "ast.hpp"
#include "token.hpp"
#include <vector>
#include <memory>

class BinaryExpression : public Expression {
public:
    BinaryExpression(std::unique_ptr<Expression>& left, Token op, std::unique_ptr<Expression>& right);
private:
    Token op;
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;
};

class TernaryExpression : public Expression {
public:
    TernaryExpression(std::unique_ptr<Expression>& cond_expression,
                      std::unique_ptr<Expression>& true_expression,
                      std::unique_ptr<Expression>& false_expression);
private:
    std::unique_ptr<Expression> cond_expression;
    std::unique_ptr<Expression> true_expression;
    std::unique_ptr<Expression> false_expression;
};

class UnaryExpression : public Expression {
public:
    UnaryExpression(std::unique_ptr<Expression>& base, const Token& op);
private:
    std::unique_ptr<Expression> base;
    Token op; // unary op or cast
};

class PostfixExpression : public Expression {
public:
    PostfixExpression(std::unique_ptr<Expression>& expression, const Token& op);
private:
    std::unique_ptr<Expression> expression;
    Token op;
};

class SubscriptExpression : public PostfixExpression {
public:
    SubscriptExpression(std::unique_ptr<Expression>& expression,
                        std::vector<std::unique_ptr<Expression>>& indexes);
private:
    std::vector<std::unique_ptr<Expression>> indexes;
};

class CallExpression : public PostfixExpression {
public:
    CallExpression(std::unique_ptr<Expression>& expression,
                   std::vector<std::unique_ptr<Expression>>& args);
private:
    std::vector<std::unique_ptr<Expression>> args;
};

class AccessExpression : public PostfixExpression {
public:
    AccessExpression(std::unique_ptr<Expression>& expression,
                     std::unique_ptr<Expression>& expression_to_access);
private:
    std::unique_ptr<Expression> expression_to_access;
};

class LiteralExpression : public Expression {
public:
    LiteralExpression(const Token& token) : token(token) {}
private:
    Token token;
};

class IDexpression : public Expression {
public:
    IDexpression(const Token& token) : token(token) {}
private:
    Token token;
};

class GroupExpression : public Expression {
public:
    GroupExpression(std::unique_ptr<Expression>& base);
private:
    std::unique_ptr<Expression> base;
};