#include "statement.hpp"
#include "declaration.hpp"
#include "visitor.hpp"

// ExpressionStatement
ExpressionStatement::ExpressionStatement(std::unique_ptr<Expression>& expression)
    : expression(std::move(expression)) {}

ExpressionStatement::ExpressionStatement(ExpressionStatement& other)
    : expression(std::move(other.expression)) {}

void ExpressionStatement::accept(Visitor& visitor) { visitor.visit(std::make_unique<ExpressionStatement>(*this)); }

const std::unique_ptr<Expression>& ExpressionStatement::get_expression(){
    return this->expression;
}

// DeclorationStatement
DeclorationStatement::DeclorationStatement(std::unique_ptr<VarDeclarator>& declaration)
    : declaration(std::move(declaration)) {}

DeclorationStatement::DeclorationStatement(DeclorationStatement& other)
    : declaration(std::move(other.declaration)) {}

void DeclorationStatement::accept(Visitor& visitor) { visitor.visit(std::make_unique<DeclorationStatement>(*this)); }

const std::unique_ptr<VarDeclarator>& DeclorationStatement::get_declaration() {
    return this->declaration;
}

// ReturnStatement
ReturnStatement::ReturnStatement(std::unique_ptr<Expression>& expression)
    : expression(std::move(expression)) {}

ReturnStatement::ReturnStatement(ReturnStatement& other)
    : expression(std::move(other.expression)) {}

void ReturnStatement::accept(Visitor& visitor) { visitor.visit(std::make_unique<ReturnStatement>(*this)); }

const std::unique_ptr<Expression>& ReturnStatement::get_expression() {
    return this->expression;
}

// BlockStatement
BlockStatement::BlockStatement(std::vector<std::unique_ptr<Statement>>& statements)
    : statements(std::move(statements)) {}

BlockStatement::BlockStatement(BlockStatement& other)
    : statements(std::move(other.statements)) {}

void BlockStatement::accept(Visitor& visitor) { visitor.visit(std::make_unique<BlockStatement>(*this)); }

const std::vector<std::unique_ptr<Statement>>& BlockStatement::get_statements(){
    return this->statements;
}

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

void ConditionalStatement::accept(Visitor& visitor) { visitor.visit(std::make_unique<ConditionalStatement>(*this)); }

const std::unique_ptr<Expression>& ConditionalStatement::get_conditional(){
    return this->conditional;
}

const std::unique_ptr<Statement>& ConditionalStatement::get_true_statement(){
    return this->true_statement;
}

const std::unique_ptr<Statement>& ConditionalStatement::get_false_statement(){
    return this->false_statement;
}

// WhileStatement
WhileStatement::WhileStatement(std::unique_ptr<Expression>& conditional,
                               std::unique_ptr<Statement>& statement)
    : conditional(std::move(conditional)), statement(std::move(statement)) {}

WhileStatement::WhileStatement(WhileStatement& other)
    : conditional(std::move(other.conditional)), statement(std::move(other.statement)) {}

void WhileStatement::accept(Visitor& visitor) { visitor.visit(std::make_unique<WhileStatement>(*this)); }

const std::unique_ptr<Expression>& WhileStatement::get_conditional(){
    return this->conditional;
}

const std::unique_ptr<Statement>& WhileStatement::get_statement(){
    return this->statement;
}

// DoWhileStatement
DoWhileStatement::DoWhileStatement(std::unique_ptr<Statement>& statement,
                                   std::unique_ptr<Expression>& expression)
    : statement(std::move(statement)), expression(std::move(expression)) {}

DoWhileStatement::DoWhileStatement(DoWhileStatement& other)
    : statement(std::move(other.statement)), expression(std::move(other.expression)) {}

void DoWhileStatement::accept(Visitor& visitor) { visitor.visit(std::make_unique<DoWhileStatement>(*this)); }

const std::unique_ptr<Statement>& DoWhileStatement::get_statement() {
    return this->statement;
}

const std::unique_ptr<Expression>& DoWhileStatement::get_expression(){
    return this->expression;
}   

// ForStatement
ForStatement::ForStatement(std::unique_ptr<VarDeclarator>& var,
                           std::unique_ptr<Expression>& cond_expr,
                           std::unique_ptr<Expression>& iter_expr,
                           std::unique_ptr<Statement>& statement)
    : var(std::move(var)),
      cond_expr(std::move(cond_expr)),
      iter_expr(std::move(iter_expr)),
      statement(std::move(statement)) {}

ForStatement::ForStatement(std::unique_ptr<Expression>& init_expr,
                           std::unique_ptr<Expression>& cond_expr,
                           std::unique_ptr<Expression>& iter_expr,
                           std::unique_ptr<Statement>& statement)
    : init_expr(std::move(init_expr)),
      cond_expr(std::move(cond_expr)),
      iter_expr(std::move(iter_expr)),
      statement(std::move(statement)){}

ForStatement::ForStatement(ForStatement& other)
    : var(std::move(other.var)),
    init_expr(std::move(other.init_expr)),
    cond_expr(std::move(other.cond_expr)),
    iter_expr(std::move(other.iter_expr)) {}

void ForStatement::accept(Visitor& visitor) { visitor.visit(std::make_unique<ForStatement>(*this)); }

const std::unique_ptr<VarDeclarator>& ForStatement::get_var(){
    return this->var;
}

const std::unique_ptr<Expression>& ForStatement::get_init_expr(){
    return this->init_expr;
}

const std::unique_ptr<Expression>& ForStatement::get_cond_expr(){
    return this->cond_expr;
}

const std::unique_ptr<Expression>& ForStatement::get_iter_expr(){
    return this->iter_expr;
}

const std::unique_ptr<Statement>& ForStatement::get_statement(){
    return this->statement;
}


void BreakStatement::accept(Visitor& visitor) { visitor.visit(std::make_unique<BreakStatement>(*this)); }

void ContinueStatement::accept(Visitor& visitor) { visitor.visit(std::make_unique<ContinueStatement>(*this)); }
