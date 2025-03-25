#pragma once 
#include <bits/stdc++.h>
#include "ast.hpp"
#include "token.hpp"

class Declaration : public ASTNode {
public:
    virtual ~Declaration() = default;
    Declaration() = default;
};

class VarDeclaration : public Declaration {
public:
    VarDeclaration(Token token) : type(type) {};
private:    
    Token type; // может быть типом или id
    std::vector<InitDeclarator*> init_declarators;
};

class InitDeclarator : public Declaration {
public:
    InitDeclarator(IdDeclorator* declarator, Expression* expression) : declarator(declarator), expression(expression) {}
    IdDeclorator* declarator;
    Expression* expression;
};

enum class IDDeclaratorType {   
    REF, POINTER, ARRAY
};

class IdDeclorator : public Declaration{
public:
    IdDeclorator(Token token, IDDeclaratorType type, Expression* expression) : id(token), type(type), expression(expression){}
    Token id;
    IDDeclaratorType type; 
    Expression* expression;
};


class FuncDeclorator : public Declaration {
public:

private:
    Token type; // может быть и id
    Token name;
    std::vector<ParamDeclarator*> params;
    // BlockStatement* statement;
};

class ParamDeclarator : Declaration {
private:
    Token type; // может быть и id
    IdDeclorator* declorator;
};


class StructDecloration : Declaration {
private:
    Token id;
    std::vector<VarDeclaration*> vars;
};
















class FuncDeclaration : public Declaration {

};

class StructDeclaration : public Declaration {

};
