#pragma once
#include <unordered_map>
#include <map>
#include <string>
#include <any>
#include <vector>
#include "type.hpp"
#include "ast.hpp"

class Scope {
public:
    ~Scope() = default;
    Scope(std::unique_ptr<ASTNode>); 

    std::shared_ptr<Scope> get_prev_table();
    std::shared_ptr<Scope> create_new_table(std::shared_ptr<Scope> ,std::unique_ptr<ASTNode>);
    
    Type match_variable(std::string);
    StructType match_struct(std::string);
    FuncType match_function(std::string, std::vector<Type>);
    
    void push_variable(std::string, Type);
    void push_struct(std::string, StructType);
    void push_func(std::string, FuncType);
    
private:
    std::shared_ptr<Scope>prev_table;
    std::unique_ptr<ASTNode> node; // чему принадлежит эта облась видимости
    std::unordered_map<std::string, Type> variables;
    std::unordered_map<std::string, StructType> structs;
    std::multimap<std::string, FuncType> functions;
};