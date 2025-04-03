#pragma once

class Visitor;
class ASTNode {
public:
    ~ASTNode() = default;
    virtual void accept(Visitor& visitor) = 0;
};

class Statement : public ASTNode{
public:
    ~Statement() = default;
    virtual void accept(Visitor& visitor) = 0;
};

class Expression : public ASTNode{
public:
    ~Expression() = default;
    virtual void accept(Visitor& visitor) = 0;
};

class Declarator : public ASTNode {
public:
    virtual ~Declarator() = default;
    virtual void accept(Visitor& visitor) = 0;
};



