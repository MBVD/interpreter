#include "statement.hpp"
#include "declaration.hpp"

ExpressionStatement::ExpressionStatement(std::unique_ptr<Expression>& expression)
    : expression(std::move(expression)) {}

DeclorationStatement::DeclorationStatement(std::unique_ptr<VarDeclaration>& declaration)
    : declaration(std::move(declaration)) {}

ReturnStatement::ReturnStatement(std::unique_ptr<Expression>& expression)
    : expression(std::move(expression)) {}

BlockStatement::BlockStatement(std::vector<std::unique_ptr<Statement>>& statements)
    : statements(std::move(statements)) {}

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


WhileStatement::WhileStatement(std::unique_ptr<Expression>& conditional,
               std::unique_ptr<Statement>& statement)
    : conditional(std::move(conditional)), statement(std::move(statement)) {}

DoWhileStatement::DoWhileStatement(std::unique_ptr<Statement>& statement,
                 std::unique_ptr<Expression>& expression)
    : statement(std::move(statement)), expression(std::move(expression)) {}

ForStatement::ForStatement(std::unique_ptr<VarDeclaration>& var,
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