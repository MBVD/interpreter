#include "declaration.hpp"
#include "statement.hpp"

VarDeclaration::VarDeclaration(const Token& token) : type(token) {}

InitDeclarator::InitDeclarator(std::unique_ptr<IdDeclorator>& declarator, std::unique_ptr<Expression>& expression)
        : declarator(std::move(declarator)), expression(std::move(expression)) {}

IdDeclorator::IdDeclorator(const Token& token, IDDeclaratorType type, std::unique_ptr<Expression>& expression)
    : id(token), type(type), expression(std::move(expression)) {}

FuncDeclarator::FuncDeclarator(const Token& returnable_type, const Token& name, std::vector<std::unique_ptr<ParamDeclarator>>&& params)
    : returnable_type(returnable_type), name(name), params(std::move(params)), semicolon(TokenType::SEMICOLON) {};

ParamDeclarator::ParamDeclarator(const Token& type, std::unique_ptr<IdDeclorator>& declorator)
    : type(type), declorator(std::move(declorator)) {}

StructDecloration::StructDecloration(const Token& id, std::vector<std::unique_ptr<VarDeclaration>>& vars)
    : id(id), vars(std::move(vars)) {}
