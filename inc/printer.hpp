#include "visitor.hpp"

class Printer : Visitor {
public:
    void visit(const std::unique_ptr<Declaration>& ) final;
    void visit(const std::unique_ptr<VarDeclarator>& ) final;
    void visit(const std::unique_ptr<InitDeclarator>& ) final;
    void visit(const std::unique_ptr<IdDeclorator>& ) final;
    void visit(const std::unique_ptr<FuncDeclarator>& ) final;
    void visit(const std::unique_ptr<ParamDeclarator>& ) final;
    void visit(const std::unique_ptr<StructDeclarator>& ) final;

    void visit(const std::unique_ptr<Expression>& ) final;
    void visit(const std::unique_ptr<BinaryExpression>&) final;
    void visit(const std::unique_ptr<TernaryExpression>&) final;
    void visit(const std::unique_ptr<UnaryExpression>&) final;
    void visit(const std::unique_ptr<PostfixExpression>&) final;
    void visit(const std::unique_ptr<SubscriptExpression>&) final;
    void visit(const std::unique_ptr<CallExpression>&) final;
    void visit(const std::unique_ptr<AccessExpression>&) final;
    void visit(const std::unique_ptr<LiteralExpression>&) final;
    void visit(const std::unique_ptr<IDexpression>&) final;
    void visit(const std::unique_ptr<GroupExpression>&) final;

    void visit(const std::unique_ptr<Statement>& ) final;
    void visit(const std::unique_ptr<ExpressionStatement>&) final;
    void visit(const std::unique_ptr<DeclorationStatement>&) final;
    void visit(const std::unique_ptr<ReturnStatement>&) final;
    void visit(const std::unique_ptr<BreakStatement>&) final;
    void visit(const std::unique_ptr<ContinueStatement>&) final;
    void visit(const std::unique_ptr<BlockStatement>&) final;
    void visit(const std::unique_ptr<ConditionalStatement>&) final;
    void visit(const std::unique_ptr<LoopStatement>& node) final;
    void visit(const std::unique_ptr<WhileStatement>&) final;
    void visit(const std::unique_ptr<DoWhileStatement>&) final;
    void visit(const std::unique_ptr<ForStatement>&) final;
};