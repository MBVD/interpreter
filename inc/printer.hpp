#include "visitor.hpp"

class Printer : Visitor {
public:
    void print(TranslationUnit& );
    void visit(Expression*) final;
private:
    void visit(ASTNode* ) final;
    void visit(Declarator*) final;
    void visit(VarDeclarator*) final;
    void visit(InitDeclarator*) final;
    void visit(IdDeclorator*) final;
    void visit(FuncDeclarator*) final;
    void visit(ParamDeclarator*) final;
    void visit(StructDeclarator*) final;

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
};