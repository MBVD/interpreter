#pragma once
#include <unordered_map>
#include <string>
#include <any>
#include <vector>
#include "type.hpp"

class SymbolTable {
public:
    SymbolTable& get_next_table();
    SymbolTable& get_prev_table();
    bool match_function();
    bool match_variable();

private:
    SymbolTable& next_table;
    SymbolTable& prev_table;
    std::unordered_map<std::string, Type> variables;
    std::unordered_map<std::string, FuncType> functions;
};