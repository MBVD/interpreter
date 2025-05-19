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
    Scope(std::shared_ptr<Scope> get_prev_table);
    
    std::shared_ptr<Scope> get_prev_table();
    std::shared_ptr<Scope> create_new_table(std::shared_ptr<Scope>);
    
    std::shared_ptr<Type> match_variable(std::string);
    std::shared_ptr<StructType> match_struct(std::string);
    std::vector<std::shared_ptr<FuncType>> match_functions(std::string);
    
    void push_variable(std::string, std::shared_ptr<Type>);
    void push_struct(std::string, std::shared_ptr<StructType>);
    void push_func(std::string, std::shared_ptr<FuncType>);
    
private:
    std::shared_ptr<Scope>prev_table;
    std::unordered_map<std::string, std::shared_ptr<Type>> variables;
    std::unordered_map<std::string, std::shared_ptr<StructType>> structs;
    std::multimap<std::string, std::shared_ptr<FuncType>> functions;
};