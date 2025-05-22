#include "symbolTable.hpp"

SymbolTable::SymbolTable(std::shared_ptr<SymbolTable> prev_table) : prev_table(prev_table) {}

std::shared_ptr<SymbolTable> SymbolTable::get_prev_table() {
    return prev_table;
}

void SymbolTable::push_variable(const std::string& name, std::shared_ptr<Type> value) {
    variables[name] = value;
}

void SymbolTable::push_struct(const std::string& name, std::shared_ptr<StructType> struc) {
    variables[name] = struc;
}

void SymbolTable::push_function(const std::string& name, std::unique_ptr<FuncDeclarator> func_declarator) {
    auto returnable_type = func_declarator->get_returnable_type();
    auto args = func_declarator->get_params();
    auto func_type = std::make_shared<FuncType>(returnable_type, args);
    functions.emplace(name, std::make_pair(std::move(func_declarator), std::unordered_map<std::string, std::shared_ptr<Type>>{}));
}

std::any SymbolTable::match_variable(const std::string& name) {
    auto it = variables.find(name);
    if (it != variables.end()) {
        return it->second;
    }
    if (prev_table) {
        return prev_table->match_variable(name);
    }
    throw std::runtime_error("Variable not found: " + name);
}