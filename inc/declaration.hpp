#pragma once
#include <vector>
#include <memory>
#include "ast.hpp"
#include "token.hpp"
#include "statement.hpp"

class InitDeclarator;
class IdDeclorator;
class ParamDeclarator;

class VarDeclaration : public Declaration {
public:
    VarDeclaration(const Token& token);
private:
    Token type; // может быть типом или id
    std::vector<std::unique_ptr<InitDeclarator>> init_declarators;
};

class InitDeclarator : public Declaration {
public:
    InitDeclarator(std::unique_ptr<IdDeclorator>& declarator, std::unique_ptr<Expression>& expression);
private:
    std::unique_ptr<IdDeclorator> declarator;
    std::unique_ptr<Expression> expression;
};

enum class IDDeclaratorType {
    REF, POINTER, ARRAY
};

class IdDeclorator : public Declaration {
public:
    IdDeclorator(const Token& token, IDDeclaratorType type, std::unique_ptr<Expression>& expression);
private:
    Token id;
    IDDeclaratorType type;
    std::unique_ptr<Expression> expression;
};

class FuncDeclarator : public Declaration {
    public:
        FuncDeclarator(const Token& returnable_type, const Token& name, std::vector<std::unique_ptr<ParamDeclarator>>&& params);
    private:
        Token returnable_type;
        Token name;
        std::vector<std::unique_ptr<ParamDeclarator>>params;
        Token semicolon;
        std::unique_ptr<BlockStatement> block;
    };
class ParamDeclarator : public Declaration {
public:
    ParamDeclarator(const Token& type, std::unique_ptr<IdDeclorator>& declorator);
private:
    Token type; // может быть и id
    std::unique_ptr<IdDeclorator> declorator;
};

class StructDecloration : public Declaration {
public:
    StructDecloration(const Token& id, std::vector<std::unique_ptr<VarDeclaration>>& vars);
private:
    Token id;
    std::vector<std::unique_ptr<VarDeclaration>> vars;
};

