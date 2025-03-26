#include "ast.hpp"
#include "declaration.hpp"
#include <vector>
#include <memory>

class ExpressionStatement : public Statement {
public:
    ExpressionStatement(std::unique_ptr<Expression>& expression)
        : expression(std::move(expression)) {}
private:
    std::unique_ptr<Expression> expression;
};

class DeclorationStatement : public Statement {
public:
    DeclorationStatement(std::unique_ptr<VarDeclaration>& declaration)
        : declaration(std::move(declaration)) {}
private:
    std::unique_ptr<VarDeclaration> declaration;
};

class ReturnStatement : public Statement {
public:
    ReturnStatement(std::unique_ptr<Expression>& expression)
        : expression(std::move(expression)) {}
private:
    std::unique_ptr<Expression> expression;
};

class BreakStatement : public Statement {
    ~BreakStatement() = default;
};

class ContinueStatement : public Statement {
    ~ContinueStatement() = default;
};

class BlockStatement : public Statement {
public:
    BlockStatement(std::vector<std::unique_ptr<Statement>>& statements)
        : statements(std::move(statements)) {}
private:
    std::vector<std::unique_ptr<Statement>> statements;
};

class ConditionalStatement : public Statement {
public:
    ConditionalStatement(std::unique_ptr<Expression>& conditional,
                         std::unique_ptr<Statement>& true_statement,
                         std::unique_ptr<Statement>& false_statement)
        : conditional(std::move(conditional)),
          true_statement(std::move(true_statement)),
          false_statement(std::move(false_statement)) {}
private:
    std::unique_ptr<Expression> conditional;
    std::unique_ptr<Statement> true_statement;
    std::unique_ptr<Statement> false_statement;
};

class LoopStatement : public Statement {
public:
    virtual ~LoopStatement() = default;
};

class WhileStatement : public LoopStatement {
public:
    WhileStatement(std::unique_ptr<Expression>& conditional,
                   std::unique_ptr<Statement>& statement)
        : conditional(std::move(conditional)), statement(std::move(statement)) {}
private:
    std::unique_ptr<Expression> conditional;
    std::unique_ptr<Statement> statement;
};

class DoWhileStatement : public LoopStatement {
public:
    DoWhileStatement(std::unique_ptr<Statement>& statement,
                     std::unique_ptr<Expression>& expression)
        : statement(std::move(statement)), expression(std::move(expression)) {}
private:
    std::unique_ptr<Statement> statement;
    std::unique_ptr<Expression> expression;
};

class ForStatement : public LoopStatement {
public:
    ForStatement(std::unique_ptr<VarDeclaration>& vars,
                 std::unique_ptr<Expression>& init_expr,
                 std::unique_ptr<Expression>& cond_expr,
                 std::unique_ptr<Expression>& iter_expr)
        : vars(std::move(vars)),
          init_expr(std::move(init_expr)),
          cond_expr(std::move(cond_expr)),
          iter_expr(std::move(iter_expr)) {}
private:
    std::unique_ptr<VarDeclaration> vars;
    std::unique_ptr<Expression> init_expr;
    std::unique_ptr<Expression> cond_expr;
    std::unique_ptr<Expression> iter_expr;
};