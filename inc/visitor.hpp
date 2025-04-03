#include "token.hpp"
#include "ast.hpp"
#include "declaration.hpp"
#include "expression.hpp"
#include <memory>

class Visitor {
public:
    virtual void visit(std::unique_ptr<Declaration>&) = 0;
    virtual void visit(std::unique_ptr<VarDeclarator>&) = 0;
    virtual void visit(std::unique_ptr<InitDeclarator>& ) = 0;
    virtual void visit(std::unique_ptr<IdDeclorator>& ) = 0;
    virtual void visit(std::unique_ptr<FuncDeclarator>& ) = 0;
    virtual void visit(std::unique_ptr<ParamDeclarator>& ) = 0;
    virtual void visit(std::unique_ptr<StructDeclarator>& ) = 0;

    virtual void visit(std::unique_ptr<Expression>&) = 0;
    virtual void visit(std::unique_ptr<BinaryExpression>&) = 0;
    virtual void visit(std::unique_ptr<TernaryExpression>&) = 0;
    virtual void visit(std::unique_ptr<UnaryExpression>&) = 0;
    virtual void visit(std::unique_ptr<PostfixExpression>&) = 0;
    virtual void visit(std::unique_ptr<SubscriptExpression>&) = 0;
    virtual void visit(std::unique_ptr<CallExpression>&) = 0;
    virtual void visit(std::unique_ptr<AccessExpression>&) = 0;
    virtual void visit(std::unique_ptr<LiteralExpression>&) = 0;
    virtual void visit(std::unique_ptr<IDexpression>&) = 0;
    virtual void visit(std::unique_ptr<GroupExpression>&) = 0;

    virtual void visit(std::unique_ptr<Statement>&) = 0;
    virtual void visit(std::unique_ptr<ExpressionStatement>&) = 0;
    virtual void visit(std::unique_ptr<DeclorationStatement>&) = 0;
    virtual void visit(std::unique_ptr<ReturnStatement>&) = 0;
    virtual void visit(std::unique_ptr<BreakStatement>&) = 0;
    virtual void visit(std::unique_ptr<ContinueStatement>&) = 0;
    virtual void visit(std::unique_ptr<BlockStatement>&) = 0;
    virtual void visit(std::unique_ptr<ConditionalStatement>&) = 0;
    virtual void visit(std::unique_ptr<WhileStatement>&) = 0;
    virtual void visit(std::unique_ptr<DoWhileStatement>&) = 0;
    virtual void visit(std::unique_ptr<ForStatement>&) = 0;

    
    virtual void visit(const std::unique_ptr<Declaration>&) = 0;
    virtual void visit(const std::unique_ptr<VarDeclarator>&) = 0;
    virtual void visit(const std::unique_ptr<InitDeclarator>& ) = 0;
    virtual void visit(const std::unique_ptr<IdDeclorator>& ) = 0;
    virtual void visit(const std::unique_ptr<FuncDeclarator>& ) = 0;
    virtual void visit(const std::unique_ptr<ParamDeclarator>& ) = 0;
    virtual void visit(const std::unique_ptr<StructDeclarator>& ) = 0;
    
    virtual void visit(const std::unique_ptr<Expression>&) = 0;
    virtual void visit(const std::unique_ptr<BinaryExpression>&) = 0;
    virtual void visit(const std::unique_ptr<TernaryExpression>&) = 0;
    virtual void visit(const std::unique_ptr<UnaryExpression>&) = 0;
    virtual void visit(const std::unique_ptr<PostfixExpression>&) = 0;
    virtual void visit(const std::unique_ptr<SubscriptExpression>&) = 0;
    virtual void visit(const std::unique_ptr<CallExpression>&) = 0;
    virtual void visit(const std::unique_ptr<AccessExpression>&) = 0;
    virtual void visit(const std::unique_ptr<LiteralExpression>&) = 0;
    virtual void visit(const std::unique_ptr<IDexpression>&) = 0;
    virtual void visit(const std::unique_ptr<GroupExpression>&) = 0;

    virtual void visit(const std::unique_ptr<Statement>&) = 0;
    virtual void visit(const std::unique_ptr<ExpressionStatement>&) = 0;
    virtual void visit(const std::unique_ptr<DeclorationStatement>&) = 0;
    virtual void visit(const std::unique_ptr<ReturnStatement>&) = 0;
    virtual void visit(const std::unique_ptr<BreakStatement>&) = 0;
    virtual void visit(const std::unique_ptr<ContinueStatement>&) = 0;
    virtual void visit(const std::unique_ptr<BlockStatement>&) = 0;
    virtual void visit(const std::unique_ptr<ConditionalStatement>&) = 0;
    virtual void visit(const std::unique_ptr<LoopStatement>& node) = 0;
    virtual void visit(const std::unique_ptr<WhileStatement>&) = 0;
    virtual void visit(const std::unique_ptr<DoWhileStatement>&) = 0;
    virtual void visit(const std::unique_ptr<ForStatement>&) = 0;
private:
};