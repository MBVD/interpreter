#include "ast.hpp"
#include "token.hpp"
#include <vector>
#include <memory>

class BinaryExpression : public Expression {
public:
    BinaryExpression(std::unique_ptr<Expression>& left, std::unique_ptr<Expression>& right)
        : left(std::move(left)), right(std::move(right)) {}
private:
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;
};

class TernaryExpression : public Expression {
public:
    TernaryExpression(std::unique_ptr<Expression>& cond_expression,
                      std::unique_ptr<Expression>& true_expression,
                      std::unique_ptr<Expression>& false_expression)
        : cond_expression(std::move(cond_expression)),
          true_expression(std::move(true_expression)),
          false_expression(std::move(false_expression)) {}
private:
    std::unique_ptr<Expression> cond_expression;
    std::unique_ptr<Expression> true_expression;
    std::unique_ptr<Expression> false_expression;
};

class UnaryExpression : public Expression {
public:
    UnaryExpression(std::unique_ptr<Expression>& base, const Token& op)
        : base(std::move(base)), op(op) {}
private:
    std::unique_ptr<Expression> base;
    Token op; // unary op or cast
};

class PostfixExpression : public Expression {
public:
    PostfixExpression(std::unique_ptr<Expression>& expression, const Token& op)
        : expression(std::move(expression)), op(op) {}
private:
    std::unique_ptr<Expression> expression;
    Token op;
};

class SubscriptExpression : public Expression {
public:
    SubscriptExpression(std::unique_ptr<Expression>& expression,
                        std::vector<std::unique_ptr<Expression>>& indexes)
        : expression(std::move(expression)), indexes(std::move(indexes)) {}
private:
    std::unique_ptr<Expression> expression;
    std::vector<std::unique_ptr<Expression>> indexes;
};

class CallExpression : public Expression {
public:
    CallExpression(std::unique_ptr<Expression>& expression,
                   std::vector<std::unique_ptr<Expression>>& args)
        : expression(std::move(expression)), args(std::move(args)) {}
private:
    std::unique_ptr<Expression> expression;
    std::vector<std::unique_ptr<Expression>> args;
};

class AccessExpression : public Expression {
public:
    AccessExpression(std::unique_ptr<Expression>& expression,
                     std::unique_ptr<AccessExpression>& expression_to_access)
        : expression(std::move(expression)), expression_to_access(std::move(expression_to_access)) {}
private:
    std::unique_ptr<Expression> expression;
    std::unique_ptr<AccessExpression> expression_to_access;
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
    GroupExpression(std::unique_ptr<Expression>& base)
        : base(std::move(base)) {}
private:
    std::unique_ptr<Expression> base;
};