#pragma once
#include <vector>
#include <memory>
#include "ast.hpp"
#include "token.hpp"
#include "statement.hpp"

class InitDeclarator;
class IdDeclorator;
class ParamDeclarator;

class VarDeclarator : public Declarator {
public:
    VarDeclarator(const Token& token, std::vector<std::unique_ptr<InitDeclarator>> init_declarators);
    void accept(Visitor& visitor);
    const Token& get_type();
    const std::vector<std::unique_ptr<InitDeclarator>>& get_init_declarators();
private:
    Token type; // может быть типом или id
    std::vector<std::unique_ptr<InitDeclarator>> init_declarators;
};

class InitDeclarator : public Declarator {
public:
    InitDeclarator(std::unique_ptr<IdDeclorator> declarator);
    InitDeclarator(std::unique_ptr<IdDeclorator> declarator, std::unique_ptr<Expression> expression);
    void accept(Visitor& visitor);
    const std::unique_ptr<IdDeclorator>& get_declarator();
    const std::unique_ptr<Expression>& get_expression();
private:
    std::unique_ptr<IdDeclorator> declarator;
    std::unique_ptr<Expression> expression;
};

enum class IDDeclaratorType {
    NONE, REF, POINTER, ARRAY
};

class IdDeclorator : public Declarator {
public:
    IdDeclorator(const Token& id, IDDeclaratorType type, std::unique_ptr<Expression> expression);
    IdDeclorator(const Token& id, IDDeclaratorType type);
    IdDeclorator(const Token& id);
    void accept(Visitor& visitor);
    const Token& get_id();
    const IDDeclaratorType get_declorator_type();
    const std::unique_ptr<Expression>& get_expression();
private:
    Token id;
    IDDeclaratorType type;
    std::unique_ptr<Expression> expression;
};

class FuncDeclarator : public Declarator {
public:
    FuncDeclarator(const Token& returnable_type, const Token& name, std::vector<std::unique_ptr<ParamDeclarator>> params);
    FuncDeclarator(const Token& returnable_type, const Token& name, std::vector<std::unique_ptr<ParamDeclarator>> params, std::unique_ptr<BlockStatement> block);
    void accept(Visitor& visitor);
    const Token& get_returnable_type();
    const Token& get_name();
    const std::vector<std::unique_ptr<ParamDeclarator>>& get_params();
    const std::unique_ptr<BlockStatement>& get_block();
private:
    Token returnable_type;
    Token name;
    std::vector<std::unique_ptr<ParamDeclarator>> params;
    std::unique_ptr<BlockStatement> block;
};

class ParamDeclarator : public Declarator {
public:
    ParamDeclarator(const Token& type, std::unique_ptr<Declarator> declorator);
    void accept(Visitor& visitor);
    const Token& get_type();
    const std::unique_ptr<Declarator>& get_declorator();
private:
    Token type; // может быть и id
    std::unique_ptr<Declarator> declorator;
};

class StructDeclarator : public Declarator {
public:
    StructDeclarator(const Token& id, std::vector<std::unique_ptr<VarDeclarator>> vars);
    void accept(Visitor& visitor);
    const Token& get_id();
    const std::vector<std::unique_ptr<VarDeclarator>>& get_vars();
private:
    Token id;
    std::vector<std::unique_ptr<VarDeclarator>> vars;
};