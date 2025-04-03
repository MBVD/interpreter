#pragma once
#include <vector>
#include <memory>
#include "ast.hpp"
#include "token.hpp"
#include "statement.hpp"

class InitDeclarator;
class IdDeclorator;
class ParamDeclarator;

class VarDeclarator : public Declaration {
public:
    VarDeclarator(const Token& token, std::vector<std::unique_ptr<InitDeclarator>>& init_declarators);
    VarDeclarator(VarDeclarator& other);
    void accept(Visitor& visitor);
private:
    Token type; // может быть типом или id
    std::vector<std::unique_ptr<InitDeclarator>> init_declarators;
};

class InitDeclarator : public Declaration {
public:
    InitDeclarator(std::unique_ptr<IdDeclorator>& declarator);
    InitDeclarator(std::unique_ptr<IdDeclorator>& declarator, std::unique_ptr<Expression>& expression);
    InitDeclarator(InitDeclarator& other);
    void accept(Visitor& visitor);
private:
    std::unique_ptr<IdDeclorator> declarator;
    std::unique_ptr<Expression> expression;
};

enum class IDDeclaratorType {
    REF, POINTER, ARRAY
};

class IdDeclorator : public Declaration {
public:
    IdDeclorator(const Token& id, IDDeclaratorType type, std::unique_ptr<Expression>& expression);
    IdDeclorator(const Token& id, IDDeclaratorType type);
    IdDeclorator(const Token& id);
    IdDeclorator(IdDeclorator& other);
    void accept(Visitor& visitor);
private:
    Token id;
    IDDeclaratorType type;
    std::unique_ptr<Expression> expression;
};

class FuncDeclarator : public Declaration {
public:
    FuncDeclarator(const Token& returnable_type, const Token& name, std::vector<std::unique_ptr<ParamDeclarator>>& params);
    FuncDeclarator(const Token& returnable_type, const Token& name, std::vector<std::unique_ptr<ParamDeclarator>>& params, std::unique_ptr<BlockStatement>& block);
    FuncDeclarator(FuncDeclarator& other);
    void accept(Visitor& visitor);
private:
    Token returnable_type;
    Token name;
    std::vector<std::unique_ptr<ParamDeclarator>> params;
    std::unique_ptr<BlockStatement> block;
};

class ParamDeclarator : public Declaration {
public:
    ParamDeclarator(const Token& type, std::unique_ptr<Declaration>& declorator);
    ParamDeclarator(ParamDeclarator& other);
    void accept(Visitor& visitor);
private:
    Token type; // может быть и id
    std::unique_ptr<Declaration> declorator;
};

class StructDeclarator : public Declaration {
public:
    StructDeclarator(const Token& id, std::vector<std::unique_ptr<VarDeclarator>>& vars);
    StructDeclarator(StructDeclarator& other);
    void accept(Visitor& visitor);
private:
    Token id;
    std::vector<std::unique_ptr<VarDeclarator>> vars;
};

