#pragma once
#include <vector>
#include <memory>
#include "ast.hpp"
#include "token.hpp"
#include "statement.hpp"

class Declaration : public ASTNode {
public:
    virtual ~Declaration() = default;
    Declaration() = default;
};

class VarDeclaration : public Declaration {
public:
    VarDeclaration(const Token& token) : type(token) {}
private:
    Token type; // может быть типом или id
    std::vector<std::unique_ptr<InitDeclarator>> init_declarators;
};

class InitDeclarator : public Declaration {
public:
    InitDeclarator(std::unique_ptr<IdDeclorator>& declarator, std::unique_ptr<Expression>& expression)
        : declarator(std::move(declarator)), expression(std::move(expression)) {}
private:
    std::unique_ptr<IdDeclorator> declarator;
    std::unique_ptr<Expression> expression;
};

enum class IDDeclaratorType {
    REF, POINTER, ARRAY
};

class IdDeclorator : public Declaration {
public:
    IdDeclorator(const Token& token, IDDeclaratorType type, std::unique_ptr<Expression>& expression)
        : id(token), type(type), expression(std::move(expression)) {}
private:
    Token id;
    IDDeclaratorType type;
    std::unique_ptr<Expression> expression;
};

class FuncDeclorator : public Declaration {
public:
    FuncDeclorator(const Token& type, const Token& name, std::vector<std::unique_ptr<ParamDeclarator>>& params, std::unique_ptr<BlockStatement>& statement)
        : type(type), name(name), params(std::move(params)), statement(std::move(statement)) {}
private:
    Token type; // может быть и id
    Token name;
    std::vector<std::unique_ptr<ParamDeclarator>> params;
    std::unique_ptr<BlockStatement> statement;
};

class ParamDeclarator : public Declaration {
public:
    ParamDeclarator(const Token& type, std::unique_ptr<IdDeclorator>& declorator)
        : type(type), declorator(std::move(declorator)) {}
private:
    Token type; // может быть и id
    std::unique_ptr<IdDeclorator> declorator;
};

class StructDecloration : public Declaration {
public:
    StructDecloration(const Token& id, std::vector<std::unique_ptr<VarDeclaration>>& vars)
        : id(id), vars(std::move(vars)) {}
private:
    Token id;
    std::vector<std::unique_ptr<VarDeclaration>> vars;
};

class FuncDeclaration : public Declaration {
public:
    FuncDeclaration(const Token& returnable_type, const Token& name, std::vector<std::unique_ptr<ParamDeclarator>>&& params)
        : returnable_type(returnable_type), name(name), params(std::move(params)), semicolon(TokenType::SEMICOLON) {};
private:
    Token returnable_type;
    Token name;
    std::vector<std::unique_ptr<ParamDeclarator>>params;
    Token semicolon;
    std::unique_ptr<BlockStatement> block;
};

class StructDeclaration : public Declaration {
private:
    Token struct_name;
    std::vector<std::unique_ptr<VarDeclaration>>vars;
};