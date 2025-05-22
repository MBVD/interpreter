#pragma once
#include <unordered_map>
#include <string>
#include <any>
#include <vector>
#include <memory>
#include "declaration.hpp"
#include "type.hpp"


class SymbolTable {
public:
    SymbolTable(std::shared_ptr<SymbolTable> prev_table = nullptr) : prev_table(prev_table) {}

    std::shared_ptr<SymbolTable> get_prev_table();

    std::shared_ptr<SymbolTable> create_new_table(std::shared_ptr<SymbolTable> prev_table) {
        return std::make_shared<SymbolTable>(prev_table);
    }

    void push_variable(const std::string& name, std::shared_ptr<Type> value);

    void push_struct(const std::string& name, std::shared_ptr<StructType> struc);

    void push_function(const std::string& name, std::unique_ptr<FuncDeclarator> func_declarator);

    std::shared_ptr<FuncType> match_function(const std::string& name);

    std::shared_ptr<StructType> match_struct(const std::string& name);

    std::shared_ptr<Type> match_variable(const std::string& name);
private:
    std::unordered_map<std::string, std::shared_ptr<Type>> variables;
    std::unordered_multimap<std::string, std::pair<std::shared_ptr<FuncType>, std::unique_ptr<FuncDeclarator>>> functions;// TODO odumati na schet 
    std::unordered_map<std::string, std::shared_ptr<StructType>> structs;
    std::shared_ptr<SymbolTable>prev_table;
};