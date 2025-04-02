#include "statement.hpp"
#include "declaration.hpp"
#include "visitor.hpp"

ExpressionStatement::ExpressionStatement(std::unique_ptr<Expression>& expression)
    : expression(std::move(expression)) {}

void ExpressionStatement::accept(Visitor& visitor) { visitor.visit(*this); }

DeclorationStatement::DeclorationStatement(std::unique_ptr<VarDeclarator>& declaration)
    : declaration(std::move(declaration)) {}

void DeclorationStatement::accept(Visitor& visitor) { visitor.visit(*this); }

ReturnStatement::ReturnStatement(std::unique_ptr<Expression>& expression)
    : expression(std::move(expression)) {}

void ReturnStatement::accept(Visitor& visitor) { visitor.visit(*this); }

BlockStatement::BlockStatement(std::vector<std::unique_ptr<Statement>>& statements)
    : statements(std::move(statements)) {}

void BlockStatement::accept(Visitor& visitor) { visitor.visit(*this); }

ConditionalStatement::ConditionalStatement(std::unique_ptr<Expression>& conditional,
                                           std::unique_ptr<Statement>& true_statement,
                                           std::unique_ptr<Statement>& false_statement)
    : conditional(std::move(conditional)),
    true_statement(std::move(true_statement)),
    false_statement(std::move(false_statement)) {}

ConditionalStatement::ConditionalStatement(std::unique_ptr<Expression>& conditional,
                                           std::unique_ptr<Statement>& true_statement)
    : conditional(std::move(conditional)),
    true_statement(std::move(true_statement)) {}    

void ConditionalStatement::accept(Visitor& visitor) { visitor.visit(*this); }

WhileStatement::WhileStatement(std::unique_ptr<Expression>& conditional,
                               std::unique_ptr<Statement>& statement)
    : conditional(std::move(conditional)), statement(std::move(statement)) {}

void WhileStatement::accept(Visitor& visitor) { visitor.visit(*this); }

DoWhileStatement::DoWhileStatement(std::unique_ptr<Statement>& statement,
                                   std::unique_ptr<Expression>& expression)
    : statement(std::move(statement)), expression(std::move(expression)) {}

void DoWhileStatement::accept(Visitor& visitor) { visitor.visit(*this); }

ForStatement::ForStatement(std::unique_ptr<VarDeclarator>& var,
                           std::unique_ptr<Expression>& cond_expr,
                           std::unique_ptr<Expression>& iter_expr)
    : var(std::move(var)),
    cond_expr(std::move(cond_expr)),
    iter_expr(std::move(iter_expr)) {}

void ForStatement::accept(Visitor& visitor) { visitor.visit(*this); }

ForStatement::ForStatement(std::unique_ptr<Expression>& init_expr,
                           std::unique_ptr<Expression>& cond_expr,
                           std::unique_ptr<Expression>& iter_expr)
    : init_expr(std::move(init_expr)),
    cond_expr(std::move(cond_expr)),
    iter_expr(std::move(iter_expr)) {}

void ForStatement::accept(Visitor& visitor) { visitor.visit(*this); }