#pragma once
#include "ast.hpp"
#include "visitor.hpp"
#include "scope.hpp"
#include "type.hpp"

class Analyzer : public Visitor {
public:
    Analyzer();
    void analyze(TranslationUnit &);
private:
    static std::unordered_map<std::string, std::shared_ptr<Type>> default_types;
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

    std::shared_ptr<Type> get_type(Token);
    bool can_convert(const std::shared_ptr<Type>& from, const std::shared_ptr<Type>& to);
    std::shared_ptr<Scope> scope;
    std::shared_ptr<Type> current_type;
    std::vector<std::shared_ptr<FuncType>> matched_functions;
};