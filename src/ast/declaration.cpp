#include "declaration.hpp"
#include "statement.hpp"
#include "visitor.hpp"

// VarDeclarator
VarDeclarator::VarDeclarator(const Token& token, std::vector<std::unique_ptr<InitDeclarator>> init_declarators) 
    : type(token), init_declarators(std::move(init_declarators)) {}

void VarDeclarator::accept(Visitor& visitor) {
    visitor.visit(this);
}

Token VarDeclarator::get_type() {
    return this->type;
}

const std::vector<std::unique_ptr<InitDeclarator>>& VarDeclarator::get_init_declarators(){
    return this->init_declarators;
}

// InitDeclarator
InitDeclarator::InitDeclarator(std::unique_ptr<IdDeclorator> declarator) 
    : declarator(std::move(declarator)) {}

InitDeclarator::InitDeclarator(std::unique_ptr<IdDeclorator> declarator, std::unique_ptr<Expression> expression)
    : declarator(std::move(declarator)), expression(std::move(expression)) {}

void InitDeclarator::accept(Visitor& visitor) {
    visitor.visit(this);
}

const std::unique_ptr<IdDeclorator>& InitDeclarator::get_declarator(){
    return this->declarator;
}

const std::unique_ptr<Expression>& InitDeclarator::get_expression(){
    return this->expression;
}

// IdDeclorator
IdDeclorator::IdDeclorator(const Token& id, IDDeclaratorType type, std::unique_ptr<Expression> expression)
    : id(id), type(type), expression(std::move(expression)) {}

IdDeclorator::IdDeclorator(const Token& id, IDDeclaratorType type) 
    : id(id), type(type), expression(nullptr) {}

IdDeclorator::IdDeclorator(const Token& id) 
    : id(id), type(IDDeclaratorType::NONE), expression(nullptr) {}

void IdDeclorator::accept(Visitor& visitor) {
    visitor.visit(this);
}

Token IdDeclorator::get_id() {
    return this->id;
}

IDDeclaratorType IdDeclorator::get_declorator_type(){
    return this->type;
}

const std::unique_ptr<Expression>& IdDeclorator::get_expression(){
    return std::move(this->expression);
}

// FuncDeclarator
FuncDeclarator::FuncDeclarator(const Token& returnable_type, const Token& name, std::vector<std::unique_ptr<ParamDeclarator>> params)
    : returnable_type(returnable_type), name(name), params(std::move(params)), block(nullptr) {}

FuncDeclarator::FuncDeclarator(const Token& returnable_type, const Token& name, std::vector<std::unique_ptr<ParamDeclarator>> params, std::unique_ptr<BlockStatement> block)
    : returnable_type(returnable_type), name(name), params(std::move(params)), block(std::move(block)) {}

void FuncDeclarator::accept(Visitor& visitor) {
    visitor.visit(this);
}

Token FuncDeclarator::get_returnable_type(){
    return this->returnable_type;
}

Token FuncDeclarator::get_name() {
    return this->name;
}   

const std::vector<std::unique_ptr<ParamDeclarator>>& FuncDeclarator::get_params(){
    return this->params;
}

const std::unique_ptr<BlockStatement>& FuncDeclarator::get_block(){
    return this->block;
}

// ParamDeclarator
ParamDeclarator::ParamDeclarator(const Token& type, std::unique_ptr<InitDeclarator> declorator)
    : type(type), declorator(std::move(declorator)) {}

void ParamDeclarator::accept(Visitor& visitor) {
    visitor.visit(this);
}

Token ParamDeclarator::get_type() {
    return this->type;
}

const std::unique_ptr<InitDeclarator>& ParamDeclarator::get_declorator() {
    return this->declorator;
}

// StructDeclarator
StructDeclarator::StructDeclarator(const Token& id, std::vector<std::unique_ptr<VarDeclarator>> vars)
    : id(id), vars(std::move(vars)) {}

void StructDeclarator::accept(Visitor& visitor) {
    visitor.visit(this);
}

Token StructDeclarator::get_id() {
    return this->id;
}

const std::vector<std::unique_ptr<VarDeclarator>>& StructDeclarator::get_vars(){
    return this->vars;
}