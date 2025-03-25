#include "ast.hpp"
#include "declaration.hpp"

class ExpressionStatement : public Statement {

private:
    Expression* expression;
};

class DeclorationSatement : public Statement {

private:
    VarDeclaration* declaration; 
};

class ReturnStatement : public Statement {

private:
    Expression* expression;
};

class BreakStatement : public Statement {

};

class ContinueStatement : public Statement {

};

class BlockStatement : public Statement {

private:
    std::vector<Statement*> statements;
};

class ConditionalStatement : public Statement {

private:
    Expression* conditional;
    Statement* true_statement;
    Statement* false_statement;
};

class LoopStatement : public Statement {
    virtual ~LoopStatement() = default;
};

class WhileStatement : LoopStatement {

private:
    Expression* conditional;
    Statement* statement;
};

class DoWhileStatement : LoopStatement {

private:
    Statement* statement;
    Expression* expression;
};

class ForStatement : LoopStatement {

private:
    VarDeclaration* vars;
    Expression* init_expr;
    Expression* cond_expr;
    Expression* iter_expr;
};   
