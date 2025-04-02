#include "declaration.hpp"
#include "statement.hpp"
#include "visitor.hpp"

VarDeclarator::VarDeclarator(const Token& token, std::vector<std::unique_ptr<InitDeclarator>>& init_declarators) 
    : type(token), init_declarators(std::move(init_declarators)) {}

void VarDeclarator::accept(Visitor& visitor) { visitor.visit(*this); }

InitDeclarator::InitDeclarator(std::unique_ptr<IdDeclorator>& declarator) 
    : declarator(std::move(declarator)) {}

InitDeclarator::InitDeclarator(std::unique_ptr<IdDeclorator>& declarator, std::unique_ptr<Expression>& expression)
    : declarator(std::move(declarator)), expression(std::move(expression)) {}

void InitDeclarator::accept(Visitor& visitor) { visitor.visit(*this); }

IdDeclorator::IdDeclorator(const Token& id, IDDeclaratorType type, std::unique_ptr<Expression>& expression)
    : id(id), type(type), expression(std::move(expression)) {}

IdDeclorator::IdDeclorator(const Token& id, IDDeclaratorType type) 
    : id(id), type(type) {}

IdDeclorator::IdDeclorator(const Token& id) 
    : id(id) {}

void IdDeclorator::accept(Visitor& visitor) { visitor.visit(*this); }

FuncDeclarator::FuncDeclarator(const Token& returnable_type, const Token& name, std::vector<std::unique_ptr<ParamDeclarator>>& params)
    : returnable_type(returnable_type), name(name), params(std::move(params)) {}

FuncDeclarator::FuncDeclarator(const Token& returnable_type, const Token& name, std::vector<std::unique_ptr<ParamDeclarator>>& params, std::unique_ptr<BlockStatement>& block)
    : returnable_type(returnable_type), name(name), params(std::move(params)), block(std::move(block)) {}

void FuncDeclarator::accept(Visitor& visitor) { visitor.visit(*this); }

ParamDeclarator::ParamDeclarator(const Token& type, std::unique_ptr<Declaration>& declorator)
    : type(type), declorator(std::move(declorator)) {}

void ParamDeclarator::accept(Visitor& visitor) { visitor.visit(*this); }

StructDeclarator::StructDeclarator(const Token& id, std::vector<std::unique_ptr<VarDeclarator>>& vars)
    : id(id), vars(std::move(vars)) {}

void StructDeclarator::accept(Visitor& visitor) { visitor.visit(*this); }
