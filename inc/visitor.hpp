#include "token.hpp"
#include "ast.hpp"
#include "declaration.hpp"
#include "expression.hpp"
#include <memory>

class Visitor {
public:
    virtual void visit(Declarator*) = 0;
    virtual void visit(VarDeclarator*) = 0;
    virtual void visit(InitDeclarator*) = 0;
    virtual void visit(IdDeclorator*) = 0;
    virtual void visit(FuncDeclarator*) = 0;
    virtual void visit(ParamDeclarator*) = 0;
    virtual void visit(StructDeclarator*) = 0;

    virtual void visit(Expression*) = 0;
    virtual void visit(ComparisonExpression*) = 0;
    virtual void visit(TernaryExpression*) = 0;
    virtual void visit(BinaryExpression*) = 0;
    virtual void visit(UnaryExpression*) = 0;
    virtual void visit(PostfixExpression*) = 0;
    virtual void visit(SubscriptExpression*) = 0;
    virtual void visit(CallExpression*) = 0;
    virtual void visit(AccessExpression*) = 0;
    virtual void visit(LiteralExpression*) = 0;
    virtual void visit(IDexpression*) = 0;
    virtual void visit(GroupExpression*) = 0;
 
    virtual void visit(Statement*) = 0;
    virtual void visit(ExpressionStatement*) = 0;
    virtual void visit(DeclarationStatement*) = 0;
    virtual void visit(ReturnStatement*) = 0;
    virtual void visit(BreakStatement*) = 0;
    virtual void visit(ContinueStatement*) = 0;
    virtual void visit(BlockStatement*) = 0;
    virtual void visit(ConditionalStatement*) = 0;
    virtual void visit(LoopStatement*) = 0;
    virtual void visit(WhileStatement*) = 0;
    virtual void visit(DoWhileStatement*) = 0;
    virtual void visit(ForStatement*) = 0;
};