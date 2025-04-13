#include "statement.hpp"
#include "declaration.hpp"
#include "visitor.hpp"

// ExpressionStatement
ExpressionStatement::ExpressionStatement(std::unique_ptr<Expression> expression)
    : expression(std::move(expression)) {}

void ExpressionStatement::accept(Visitor& visitor) { visitor.visit(this); }

const std::unique_ptr<Expression>& ExpressionStatement::get_expression(){
    return this->expression;
}

// DeclarationStatement
DeclarationStatement::DeclarationStatement(std::unique_ptr<VarDeclarator> declaration)
    : declaration(std::move(declaration)) {}

void DeclarationStatement::accept(Visitor& visitor) { visitor.visit(this); }

const std::unique_ptr<VarDeclarator>& DeclarationStatement::get_declaration() {
    return this->declaration;
}

// ReturnStatement
ReturnStatement::ReturnStatement(std::unique_ptr<Expression> expression)
    : expression(std::move(expression)) {}

ReturnStatement::ReturnStatement() : expression(nullptr){}

void ReturnStatement::accept(Visitor& visitor) { visitor.visit(this); }

const std::unique_ptr<Expression>& ReturnStatement::get_expression() {
    return this->expression;
}

// BlockStatement
BlockStatement::BlockStatement(std::vector<std::unique_ptr<Statement>> statements)
    : statements(std::move(statements)) {}

void BlockStatement::accept(Visitor& visitor) { visitor.visit(this); }

const std::vector<std::unique_ptr<Statement>>& BlockStatement::get_statements(){
    return this->statements;
}

// ConditionalStatement
ConditionalStatement::ConditionalStatement(std::unique_ptr<Expression> conditional,
                                           std::unique_ptr<Statement> true_statement,
                                           std::unique_ptr<Statement> false_statement)
    : conditional(std::move(conditional)),
      true_statement(std::move(true_statement)),
      false_statement(std::move(false_statement)) {}

ConditionalStatement::ConditionalStatement(std::unique_ptr<Expression> conditional,
                                           std::unique_ptr<Statement> true_statement)
    : conditional(std::move(conditional)),
      true_statement(std::move(true_statement)),
      false_statement(nullptr) {}

void ConditionalStatement::accept(Visitor& visitor) { visitor.visit(this); }

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
WhileStatement::WhileStatement(std::unique_ptr<Expression> conditional,
                               std::unique_ptr<Statement> statement)
    : conditional(std::move(conditional)), statement(std::move(statement)) {}

void WhileStatement::accept(Visitor& visitor) { visitor.visit(this); }

const std::unique_ptr<Expression>& WhileStatement::get_conditional(){
    return this->conditional;
}

const std::unique_ptr<Statement>& WhileStatement::get_statement(){
    return this->statement;
}

// DoWhileStatement
DoWhileStatement::DoWhileStatement(std::unique_ptr<Statement> statement,
                                   std::unique_ptr<Expression> expression)
    : statement(std::move(statement)), expression(std::move(expression)) {}

void DoWhileStatement::accept(Visitor& visitor) { visitor.visit(this); }

const std::unique_ptr<Statement>& DoWhileStatement::get_statement() {
    return this->statement;
}

const std::unique_ptr<Expression>& DoWhileStatement::get_expression(){
    return this->expression;
}   

// ForStatement
ForStatement::ForStatement(std::unique_ptr<VarDeclarator> var,
                           std::unique_ptr<Expression> cond_expr,
                           std::unique_ptr<Expression> iter_expr,
                           std::unique_ptr<Statement> statement)
    : var(std::move(var)),
      cond_expr(std::move(cond_expr)),
      iter_expr(std::move(iter_expr)),
      statement(std::move(statement)) {}

ForStatement::ForStatement(std::unique_ptr<Expression> init_expr,
                           std::unique_ptr<Expression> cond_expr,
                           std::unique_ptr<Expression> iter_expr,
                           std::unique_ptr<Statement> statement)
    : init_expr(std::move(init_expr)),
      cond_expr(std::move(cond_expr)),
      iter_expr(std::move(iter_expr)),
      statement(std::move(statement)){}

void ForStatement::accept(Visitor& visitor) { visitor.visit(this); }

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

// BreakStatement
void BreakStatement::accept(Visitor& visitor) { visitor.visit(this); }

// ContinueStatement
void ContinueStatement::accept(Visitor& visitor) { visitor.visit(this); }

void EmptyStatement::accept(Visitor& visitor) { visitor.visit(this); }