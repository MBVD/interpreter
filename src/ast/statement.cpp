#include "statement.hpp"
#include "declaration.hpp"
#include "visitor.hpp"

// ExpressionStatement
ExpressionStatement::ExpressionStatement(std::unique_ptr<Expression>& expression)
    : expression(std::move(expression)) {}

ExpressionStatement::ExpressionStatement(ExpressionStatement& other)
    : expression(std::move(other.expression)) {}

void ExpressionStatement::accept(Visitor& visitor) { visitor.visit(*this); }

// DeclorationStatement
DeclorationStatement::DeclorationStatement(std::unique_ptr<VarDeclarator>& declaration)
    : declaration(std::move(declaration)) {}

DeclorationStatement::DeclorationStatement(DeclorationStatement& other)
    : declaration(std::move(other.declaration)) {}

void DeclorationStatement::accept(Visitor& visitor) { visitor.visit(*this); }

// ReturnStatement
ReturnStatement::ReturnStatement(std::unique_ptr<Expression>& expression)
    : expression(std::move(expression)) {}

ReturnStatement::ReturnStatement(ReturnStatement& other)
    : expression(std::move(other.expression)) {}

void ReturnStatement::accept(Visitor& visitor) { visitor.visit(*this); }

// BlockStatement
BlockStatement::BlockStatement(std::vector<std::unique_ptr<Statement>>& statements)
    : statements(std::move(statements)) {}

BlockStatement::BlockStatement(BlockStatement& other)
    : statements(std::move(other.statements)) {}

void BlockStatement::accept(Visitor& visitor) { visitor.visit(*this); }

// ConditionalStatement
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

ConditionalStatement::ConditionalStatement(ConditionalStatement& other)
    : conditional(std::move(other.conditional)),
      true_statement(std::move(other.true_statement)),
      false_statement(std::move(other.false_statement)) {}

void ConditionalStatement::accept(Visitor& visitor) { visitor.visit(*this); }

// WhileStatement
WhileStatement::WhileStatement(std::unique_ptr<Expression>& conditional,
                               std::unique_ptr<Statement>& statement)
    : conditional(std::move(conditional)), statement(std::move(statement)) {}

WhileStatement::WhileStatement(WhileStatement& other)
    : conditional(std::move(other.conditional)), statement(std::move(other.statement)) {}

void WhileStatement::accept(Visitor& visitor) { visitor.visit(*this); }

// DoWhileStatement
DoWhileStatement::DoWhileStatement(std::unique_ptr<Statement>& statement,
                                   std::unique_ptr<Expression>& expression)
    : statement(std::move(statement)), expression(std::move(expression)) {}

DoWhileStatement::DoWhileStatement(DoWhileStatement& other)
    : statement(std::move(other.statement)), expression(std::move(other.expression)) {}

void DoWhileStatement::accept(Visitor& visitor) { visitor.visit(*this); }

// ForStatement
ForStatement::ForStatement(std::unique_ptr<VarDeclarator>& var,
                           std::unique_ptr<Expression>& cond_expr,
                           std::unique_ptr<Expression>& iter_expr)
    : var(std::move(var)),
      cond_expr(std::move(cond_expr)),
      iter_expr(std::move(iter_expr)) {}

ForStatement::ForStatement(std::unique_ptr<Expression>& init_expr,
                           std::unique_ptr<Expression>& cond_expr,
                           std::unique_ptr<Expression>& iter_expr)
    : init_expr(std::move(init_expr)),
      cond_expr(std::move(cond_expr)),
      iter_expr(std::move(iter_expr)) {}

ForStatement::ForStatement(ForStatement& other)
    : var(std::move(other.var)),
      init_expr(std::move(other.init_expr)),
      cond_expr(std::move(other.cond_expr)),
      iter_expr(std::move(other.iter_expr)) {}

void ForStatement::accept(Visitor& visitor) { visitor.visit(*this); }

void BreakStatement::accept(Visitor& visitor) { visitor.visit(*this); }

void ContinueStatement::accept(Visitor& visitor) { visitor.visit(*this); }