#include <vector>
#include <memory>
#include "ast.hpp"

TranslationUnit::TranslationUnit(std::vector<std::unique_ptr<ASTNode>> nodes)
    : nodes(std::move(nodes)){}

std::vector<std::unique_ptr<ASTNode>>& TranslationUnit::get_nodes(){
    return this->nodes;
}