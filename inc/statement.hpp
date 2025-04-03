#pragma once
#include "ast.hpp"
#include <vector>
#include <memory>

class VarDeclarator;

class ExpressionStatement : public Statement {
public:
    ExpressionStatement(std::unique_ptr<Expression>& expression);
    ExpressionStatement(ExpressionStatement& other);
    void accept(Visitor& visitor);
    const std::unique_ptr<Expression>& get_expression();
private:
    std::unique_ptr<Expression> expression;
};

class DeclarationStatement : public Statement {
public:
    DeclarationStatement(std::unique_ptr<VarDeclarator>& declaration);
    DeclarationStatement(DeclarationStatement& other);
    void accept(Visitor& visitor);
    const std::unique_ptr<VarDeclarator>& get_declaration();
private:
    std::unique_ptr<VarDeclarator> declaration;
};

class ReturnStatement : public Statement {
public:
    ReturnStatement(std::unique_ptr<Expression>& expression);
    ReturnStatement(ReturnStatement& other);
    void accept(Visitor& visitor);
    const std::unique_ptr<Expression>& get_expression();
private:
    std::unique_ptr<Expression> expression;
};

class BreakStatement : public Statement {
public:
    ~BreakStatement() = default;
    BreakStatement() = default;
    BreakStatement(BreakStatement& other) = default;
    void accept(Visitor& visitor);
};

class ContinueStatement : public Statement {
public:
    ~ContinueStatement() = default;
    ContinueStatement() = default;
    ContinueStatement(ContinueStatement& other) = default;
    void accept(Visitor& visitor);
};

class BlockStatement : public Statement {
public:
    BlockStatement(std::vector<std::unique_ptr<Statement>>& statements);
    BlockStatement(BlockStatement& other);
    BlockStatement() = default;
    void accept(Visitor& visitor);
    const std::vector<std::unique_ptr<Statement>>& get_statements();
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
    ConditionalStatement(ConditionalStatement& other);
    void accept(Visitor& visitor);
    const std::unique_ptr<Expression>& get_conditional();
    const std::unique_ptr<Statement>& get_true_statement();
    const std::unique_ptr<Statement>& get_false_statement();
private:
    std::unique_ptr<Expression> conditional;
    std::unique_ptr<Statement> true_statement;
    std::unique_ptr<Statement> false_statement;
};

class LoopStatement : public Statement {
public:
    virtual ~LoopStatement() = default;
    virtual void accept(Visitor& visitor) = 0;
};

class WhileStatement : public LoopStatement {
public:
    WhileStatement(std::unique_ptr<Expression>& conditional,
                   std::unique_ptr<Statement>& statement);
    WhileStatement(WhileStatement& other);
    void accept(Visitor& visitor);
    const std::unique_ptr<Expression>& get_conditional();
    const std::unique_ptr<Statement>& get_statement();
private:
    std::unique_ptr<Expression> conditional;
    std::unique_ptr<Statement> statement;
};

class DoWhileStatement : public LoopStatement {
public:
    DoWhileStatement(std::unique_ptr<Statement>& statement,
                     std::unique_ptr<Expression>& expression);
    DoWhileStatement(DoWhileStatement& other);
    void accept(Visitor& visitor);
    const std::unique_ptr<Statement>& get_statement();
    const std::unique_ptr<Expression>& get_expression();
private:
    std::unique_ptr<Statement> statement;
    std::unique_ptr<Expression> expression;
};

class ForStatement : public LoopStatement {
public:
    ForStatement(std::unique_ptr<VarDeclarator>& var,
                 std::unique_ptr<Expression>& cond_expr,
                 std::unique_ptr<Expression>& iter_expr,
                 std::unique_ptr<Statement>& statement);
    ForStatement(std::unique_ptr<Expression>& init_expr,
                 std::unique_ptr<Expression>& cond_expr,
                 std::unique_ptr<Expression>& iter_expr, 
                 std::unique_ptr<Statement>& statement);
            
    ForStatement(ForStatement& other);

    void accept(Visitor& visitor);
    const std::unique_ptr<VarDeclarator>& get_var();
    const std::unique_ptr<Expression>& get_init_expr();
    const std::unique_ptr<Expression>& get_cond_expr();
    const std::unique_ptr<Expression>& get_iter_expr();
    const std::unique_ptr<Statement>& get_statement();
private:
    std::unique_ptr<VarDeclarator> var;
    std::unique_ptr<Expression> init_expr;
    std::unique_ptr<Expression> cond_expr;
    std::unique_ptr<Expression> iter_expr;
    std::unique_ptr<Statement> statement;
};