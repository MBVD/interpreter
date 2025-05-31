#pragma once
#include <any>
#include <memory>
#include <unordered_map>
#include "visitor.hpp"
#include "declaration.hpp"
#include "expression.hpp"
#include "statement.hpp"
#include "scope.hpp"
#include "token.hpp"

class Executor : public Visitor {
public:
    Executor();
    void execute(TranslationUnit&);
private:
    void visit(ASTNode* ) final;
    void visit(Declarator*) final;
    void visit(VarDeclarator*) final;
    void visit(InitDeclarator*) final;
    void visit(IdDeclorator*) final;
    void visit(FuncDeclarator*) final;
    void visit(ParamDeclarator*) final;
    void visit(StructDeclarator*) final;

    void visit(Expression*) final;
    void visit(ComparisonExpression*) final;
    void visit(TernaryExpression*) final;
    void visit(BinaryExpression*) final;
    void visit(UnaryExpression*) final;
    void visit(PostfixExpression*) final;
    void visit(SubscriptExpression*) final;
    void visit(CallExpression*) final;
    void visit(AccessExpression*) final;
    void visit(LiteralNumExpression*) final;
    void visit(LiteralFloatExpression*) final;
    void visit(LiteralCharExpression*) final;
    void visit(LiteralStringExpression*) final;
    void visit(IDexpression*) final;
    void visit(GroupExpression*) final;

    void visit(Statement*) final;
    void visit(ExpressionStatement*) final;
    void visit(DeclarationStatement*) final;
    void visit(ReturnStatement*) final;
    void visit(BreakStatement*) final;
    void visit(ContinueStatement*) final;
    void visit(BlockStatement*) final;
    void visit(ConditionalStatement*) final;
    void visit(LoopStatement*) final;
    void visit(WhileStatement*) final;
    void visit(DoWhileStatement*) final;
    void visit(ForStatement*) final;
    void visit(EmptyStatement*) final;

    std::shared_ptr<Symbol> match_symbol (const Token& token);
    bool is_record_type(const std::shared_ptr<Type>& type);
    bool count_bool(std::any, Token&, std::any);
    std::any binary_operation(std::any, Token&, std::any);
    std::any unary_operation(std::any, Token&);
    std::any postgix_operation(std::any, Token&);
    bool can_convert(const std::shared_ptr<Type>& from, const std::shared_ptr<Type>& to);

    std::shared_ptr<Symbol> current_value;
    std::shared_ptr<Scope> symbolTable;
    std::vector<std::shared_ptr<FuncType>> matched_functions;
    static std::unordered_map<std::string, std::shared_ptr<Symbol>> default_types;
};