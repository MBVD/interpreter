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
#include <functional>
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
    void visit(BinaryExpression*) final;
    void visit(CommaExpression*) final;
    void visit(AssignmentExpression*) final;
    void visit(TernaryExpression*) final;
    void visit(LogicalOrExpression*) final;
    void visit(LogicalAndExpression*) final;
    void visit(BiteIncOrExpression*) final;
    void visit(BiteExcOrExpression*) final;
    void visit(BiteAndExpression*) final;
    void visit(ComparisonExpression*) final;
    void visit(ShiftExpression*) final;
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
    std::shared_ptr<VarSymbol> binary_operation(std::shared_ptr<VarSymbol>, Token&, std::shared_ptr<VarSymbol>);
    std::shared_ptr<VarSymbol> assignment_operation(std::shared_ptr<VarSymbol>, Token&, std::shared_ptr<VarSymbol>);
    std::shared_ptr<VarSymbol> unary_operation(std::shared_ptr<VarSymbol>, Token&);
    std::shared_ptr<VarSymbol> postgix_operation(std::shared_ptr<VarSymbol>, Token&);
    bool can_convert(const std::shared_ptr<Type>& from, const std::shared_ptr<Type>& to);
    
    std::vector<std::shared_ptr<FuncSymbol>> matched_functions;
    std::shared_ptr<Symbol> current_value;
    std::shared_ptr<Scope> symbolTable;
    static std::unordered_map<std::string, std::shared_ptr<Symbol>> default_types;
    bool is_rvalue = false;
    bool is_library_function = false;
    static std::unordered_map<std::string, std::function<std::shared_ptr<Symbol>(std::vector<std::shared_ptr<Symbol>>)>> libary_functions;
};
