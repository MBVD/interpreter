#pragma once
#include "ast.hpp"
#include "token.hpp"
#include <vector>
#include <memory>

class BinaryExpression : public Expression {
public:
    BinaryExpression(std::unique_ptr<Expression>& left, Token op, std::unique_ptr<Expression>& right);
    BinaryExpression(BinaryExpression& other);
    void accept(Visitor& visitor);
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
    TernaryExpression(TernaryExpression& other);
    void accept(Visitor& visitor);
private:
    std::unique_ptr<Expression> cond_expression;
    std::unique_ptr<Expression> true_expression;
    std::unique_ptr<Expression> false_expression;
};

class UnaryExpression : public Expression {
public:
    UnaryExpression(std::unique_ptr<Expression>& base, const Token& op);
    UnaryExpression(UnaryExpression& other);
    void accept(Visitor& visitor);
private:
    std::unique_ptr<Expression> base;
    Token op; // unary op or cast
};

class PostfixExpression : public Expression {
public:
    PostfixExpression(std::unique_ptr<Expression>& expression, const Token& op);
    PostfixExpression(PostfixExpression& other);
    virtual void accept(Visitor& visitor);
private:
    std::unique_ptr<Expression> expression;
    Token op;
};

class SubscriptExpression : public PostfixExpression {
public:
    SubscriptExpression(std::unique_ptr<Expression>& expression,
                        std::vector<std::unique_ptr<Expression>>& indexes);
    SubscriptExpression(SubscriptExpression& other);
    void accept(Visitor& visitor);
private:
    std::vector<std::unique_ptr<Expression>> indexes;
};

class CallExpression : public PostfixExpression {
public:
    CallExpression(std::unique_ptr<Expression>& expression,
                   std::vector<std::unique_ptr<Expression>>& args);
    CallExpression(CallExpression& other);
    void accept(Visitor& visitor);
private:
    std::vector<std::unique_ptr<Expression>> args;
};

class AccessExpression : public PostfixExpression {
public:
    AccessExpression(std::unique_ptr<Expression>& expression,
                     std::unique_ptr<Expression>& expression_to_access);
    AccessExpression(AccessExpression& other);
    void accept(Visitor& visitor);
private:
    std::unique_ptr<Expression> expression_to_access;
};

class LiteralExpression : public Expression {
public:
    LiteralExpression(const Token& token);
    LiteralExpression(LiteralExpression& other);
    void accept(Visitor&);
private:
    Token token;
};

class IDexpression : public Expression {
public:
    IDexpression(const Token& token);
    IDexpression(IDexpression& other);
    void accept(Visitor&);
private:
    Token token;
};

class GroupExpression : public Expression {
public:
    GroupExpression(std::unique_ptr<Expression>& base);
    GroupExpression(GroupExpression& other);
    void accept(Visitor& visitor);
private:
    std::unique_ptr<Expression> base;
};