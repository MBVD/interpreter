#pragma once
#include <vector>
#include <memory>

class Visitor;
class ASTNode {
public:
    ~ASTNode() = default;
    virtual void accept(Visitor& visitor) = 0;
};

class TranslationUnit {
public:
    TranslationUnit(std::vector<std::unique_ptr<ASTNode>>);
    std::vector<std::unique_ptr<ASTNode>>& get_nodes();
private:
    std::vector<std::unique_ptr<ASTNode>> nodes;
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



