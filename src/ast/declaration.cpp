#include "declaration.hpp"
#include "statement.hpp"

VarDeclaration::VarDeclaration(const Token& token, std::vector<std::unique_ptr<InitDeclarator>>& init_declarators) : type(token), init_declarators(std::move(init_declarators)){}

InitDeclarator::InitDeclarator(std::unique_ptr<IdDeclorator>& declarator) : declarator(std::move(declarator)) {}

InitDeclarator::InitDeclarator(std::unique_ptr<IdDeclorator>& declarator, std::unique_ptr<Expression>& expression)
        : declarator(std::move(declarator)), expression(std::move(expression)) {}

IdDeclorator::IdDeclorator(const Token& id, IDDeclaratorType type, std::unique_ptr<Expression>& expression)
    : id(id), type(type), expression(std::move(expression)) {}

IdDeclorator::IdDeclorator(const Token& id, IDDeclaratorType type) : id(id), type(type) {}
IdDeclorator::IdDeclorator(const Token& id) : id(id) {}

FuncDeclarator::FuncDeclarator(const Token& returnable_type, const Token& name, std::vector<std::unique_ptr<ParamDeclarator>>&& params)
    : returnable_type(returnable_type), name(name), params(std::move(params)), semicolon(TokenType::SEMICOLON) {};

ParamDeclarator::ParamDeclarator(const Token& type, std::unique_ptr<IdDeclorator>& declorator)
    : type(type), declorator(std::move(declorator)) {}

StructDeclaration::StructDeclaration(const Token& id, std::vector<std::unique_ptr<VarDeclaration>>& vars)
    : id(id), vars(std::move(vars)) {}
