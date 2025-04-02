#pragma once
#include "ast.hpp"
#include <vector>
#include <memory>

class VarDeclarator;

class ExpressionStatement : public Statement {
public:
    ExpressionStatement(std::unique_ptr<Expression>& expression);
    void accept(Visitor& visitor);
private:
    std::unique_ptr<Expression> expression;
};

class DeclorationStatement : public Statement {
public:
    DeclorationStatement(std::unique_ptr<VarDeclarator>& declaration);
    void accept(Visitor& visitor);
private:
    std::unique_ptr<VarDeclarator> declaration;
};

class ReturnStatement : public Statement {
public:
    ReturnStatement(std::unique_ptr<Expression>& expression);
    void accept(Visitor& visitor);
private:
    std::unique_ptr<Expression> expression;
};

class BreakStatement : public Statement {
public:
    ~BreakStatement() = default;
    void accept(Visitor& visitor);
};

class ContinueStatement : public Statement {
public:
    ~ContinueStatement() = default;
    void accept(Visitor& visitor);
};

class BlockStatement : public Statement {
public:
    BlockStatement(std::vector<std::unique_ptr<Statement>>& statements);
    BlockStatement() = default;
    void accept(Visitor& visitor);
private:
    std::vector<std::unique_ptr<Statement>> statements;
};

class ConditionalStatement : public Statement {
public:
    ConditionalStatement(std::unique_ptr<Expression>& conditional,
                         std::unique_ptr<Statement>& true_statement,
                         std::unique_ptr<Statement>& false_statement);
    ConditionalStatement(std::unique_ptr<Expression>& conditional,
                         std::unique_ptr<Statement>& true_statement);
    void accept(Visitor& visitor);
private:
    std::unique_ptr<Expression> conditional;
    std::unique_ptr<Statement> true_statement;
    std::unique_ptr<Statement> false_statement;
};

class LoopStatement : public Statement {
public:
    virtual ~LoopStatement() = default;
    virtual void accept(Visitor& visitor);
};

class WhileStatement : public LoopStatement {
public:
    WhileStatement(std::unique_ptr<Expression>& conditional,
                   std::unique_ptr<Statement>& statement);
    void accept(Visitor& visitor);
private:
    std::unique_ptr<Expression> conditional;
    std::unique_ptr<Statement> statement;
};

class DoWhileStatement : public LoopStatement {
public:
    DoWhileStatement(std::unique_ptr<Statement>& statement,
                     std::unique_ptr<Expression>& expression);
    void accept(Visitor& visitor);
private:
    std::unique_ptr<Statement> statement;
    std::unique_ptr<Expression> expression;
};

class ForStatement : public LoopStatement {
public:
    ForStatement(std::unique_ptr<VarDeclarator>& var,
                 std::unique_ptr<Expression>& cond_expr,
                 std::unique_ptr<Expression>& iter_expr);
    ForStatement(std::unique_ptr<Expression>& init_expr,
                 std::unique_ptr<Expression>& cond_expr,
                 std::unique_ptr<Expression>& iter_expr);
    void accept(Visitor& visitor);
private:
    std::unique_ptr<VarDeclarator> var;
    std::unique_ptr<Expression> init_expr;
    std::unique_ptr<Expression> cond_expr;
    std::unique_ptr<Expression> iter_expr;
};