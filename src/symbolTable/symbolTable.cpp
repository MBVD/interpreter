#include "symbolTable.hpp"

SymbolTable::SymbolTable(std::shared_ptr<SymbolTable> prev_table) : prev_table(prev_table) {}

std::shared_ptr<SymbolTable> SymbolTable::get_prev_table() {
    return prev_table;
}

void SymbolTable::push_variable(const std::string& name, const std::any& value) {
    variables[name] = value;
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