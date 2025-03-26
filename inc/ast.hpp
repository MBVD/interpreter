#pragma once

class ASTNode {
public:
    ~ASTNode() = default;
};

class Statement : public ASTNode{
public:
    ~Statement() = default;
};

class Expression : public ASTNode{
public:
    ~Expression() = default;
};

class Declaration : public ASTNode {
public:
    virtual ~Declaration() = default;
};



