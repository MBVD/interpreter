#pragma once
#include <unordered_map>
#include <string>
#include <any>
#include <vector>
#include <memory>


class SymbolTable {
public:
    SymbolTable(std::shared_ptr<SymbolTable> prev_table = nullptr) : prev_table(prev_table) {}

    std::shared_ptr<SymbolTable> get_prev_table();

    std::shared_ptr<SymbolTable> create_new_table(std::shared_ptr<SymbolTable> prev_table) {
        return std::make_shared<SymbolTable>(prev_table);
    }

    void push_variable(const std::string& name, const std::any& value);

    std::any match_variable(const std::string& name);
private:
    std::unordered_map<std::string, std::any> variables;
    std::unordered_multimap<std::string, std::vector<>> functions;// TODO
    std::shared_ptr<SymbolTable>prev_table;
};