#pragma once
#include "ast.hpp"
#include "token.hpp"
#include <vector>
#include <memory>

class BinaryExpression : public Expression {
public:
    BinaryExpression(std::unique_ptr<Expression> left, Token op, std::unique_ptr<Expression> right);
    void accept(Visitor& visitor) override;
    Token get_op();
    std::unique_ptr<Expression> get_left();
    std::unique_ptr<Expression> get_right();
private:
    Token op;
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;
};

class CommaExpression : public BinaryExpression{
public:
    CommaExpression(std::unique_ptr<Expression> left, Token op, std::unique_ptr<Expression> right);
    void accept(Visitor& visitor) override;
};

class AssignmentExpression : public BinaryExpression{
public:
    AssignmentExpression(std::unique_ptr<Expression> left, Token op, std::unique_ptr<Expression> right);
    void accept(Visitor& visitor) override;
};

class TernaryExpression : public Expression {
public:
    TernaryExpression(std::unique_ptr<Expression> cond_expression,
                        std::unique_ptr<Expression> true_expression,
                        std::unique_ptr<Expression> false_expression);
    void accept(Visitor& visitor) override;
    std::unique_ptr<Expression> get_cond_expression();
    std::unique_ptr<Expression> get_true_expression();
    std::unique_ptr<Expression> get_false_expression();
private:
    std::unique_ptr<Expression> cond_expression;
    std::unique_ptr<Expression> true_expression;
    std::unique_ptr<Expression> false_expression;
};

class LogicalOrExpression : public BinaryExpression {
public:
    LogicalOrExpression(std::unique_ptr<Expression> left, Token op, std::unique_ptr<Expression> right);
    void accept(Visitor& visitor) override;
};

class LogicalAndExpression : public BinaryExpression {
public:
    LogicalAndExpression(std::unique_ptr<Expression> left, Token op, std::unique_ptr<Expression> right);
    void accept(Visitor& visitor) override;
};
  
class BiteIncOrExpression : public BinaryExpression {
public:
    BiteIncOrExpression(std::unique_ptr<Expression> left, Token op, std::unique_ptr<Expression> right);
    void accept(Visitor& visitor) override;
};

class BiteExcOrExpression : public BinaryExpression {
public:
    BiteExcOrExpression(std::unique_ptr<Expression> left, Token op, std::unique_ptr<Expression> right);
    void accept(Visitor& visitor) override;
};

class BiteAndExpression : public BinaryExpression {
public:
    BiteAndExpression(std::unique_ptr<Expression> left, Token op, std::unique_ptr<Expression> right);
    void accept(Visitor& visitor) override;
};

class ComparisonExpression : public BinaryExpression{
public:
    ComparisonExpression(std::unique_ptr<Expression> left, Token op, std::unique_ptr<Expression> right);
    void accept(Visitor& visitor) override;
private:
};

class ShiftExpression : public BinaryExpression {
public:
    ShiftExpression(std::unique_ptr<Expression> left, Token op, std::unique_ptr<Expression> right);
    void accept(Visitor& visitor) override;
private:
};

class UnaryExpression : public Expression {
public:
    UnaryExpression(std::unique_ptr<Expression> base, const Token& op);
    void accept(Visitor& visitor) override;
    Token get_op();
    std::unique_ptr<Expression> get_base();
private:
    std::unique_ptr<Expression> base;
    Token op; // unary op or cast
};

class PostfixExpression : public Expression {
public:
    PostfixExpression(std::unique_ptr<Expression> expression, const Token& op);
    virtual void accept(Visitor& visitor) override;
    Token get_op();
    std::unique_ptr<Expression> get_expression();
private:
    std::unique_ptr<Expression> expression;
    Token op;
};

class SubscriptExpression : public PostfixExpression {
public:
    SubscriptExpression(std::unique_ptr<Expression> expression,
                        std::vector<std::unique_ptr<Expression>> indexes);
    void accept(Visitor& visitor) override;
    std::vector<std::unique_ptr<Expression>> get_indexes();
private:
    std::vector<std::unique_ptr<Expression>> indexes;
};

class CallExpression : public PostfixExpression {
public:
    CallExpression(std::unique_ptr<Expression> expression,
                   std::vector<std::unique_ptr<Expression>> args);
    void accept(Visitor& visitor) override;
    std::vector<std::unique_ptr<Expression>> get_args();
private:
    std::vector<std::unique_ptr<Expression>> args;
};

class AccessExpression : public PostfixExpression {
public:
    AccessExpression(std::unique_ptr<Expression> expression,
                     Token member);
    void accept(Visitor& visitor) override;
    Token get_member();    
private:
    Token member;
};

class LiteralNumExpression : public Expression {
public:
    LiteralNumExpression(const Token& token);
    void accept(Visitor& visitor) override;
    int get_value();
private:
    int8_t value;
};

class LiteralFloatExpression : public Expression {
public:
    LiteralFloatExpression(const Token& token);
    void accept(Visitor& visitor) override;
    double get_value();
private:
    double value;
};

class LiteralCharExpression : public Expression {
public:
    LiteralCharExpression(const Token& token);
    void accept(Visitor& visitor) override;
    char get_value();
private:
    char16_t value;
};

class LiteralStringExpression : public Expression {
public:
    LiteralStringExpression(const Token& token);
    void accept(Visitor& visitor) override;
    std::string get_value();
private:
    std::string value;
};

class IDexpression : public Expression {
public:
    IDexpression(const Token& token);
    void accept(Visitor& visitor) override;
    Token get_token();
private:
    Token token;
};

class GroupExpression : public Expression {
public:
    GroupExpression(std::unique_ptr<Expression> base);
    void accept(Visitor& visitor) override;
    std::unique_ptr<Expression> get_base();
private:
    std::unique_ptr<Expression> base;
};