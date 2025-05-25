#include <unordered_map>
#include <iostream>
#include <set>
#include "scope.hpp"
#include "memory"

Scope::Scope(std::shared_ptr<Scope> prev_table  = nullptr) : 
    prev_table(prev_table) {}

std::shared_ptr<Scope> Scope::get_prev_table() {
    return this->prev_table;
}

std::shared_ptr<Scope> Scope::create_new_table(std::shared_ptr<Scope> prev_scope) {
    auto scope = std::make_shared<Scope>(prev_scope);
    return scope;
}   

std::shared_ptr<Type> Scope::match_variable(std::string name) {
    if (variables.find(name) != variables.end()){
        return variables.at(name);
    }
    if (prev_table == nullptr){
        throw std::runtime_error("Variable '" + name + "' not found in scope.");
    }
    return this->prev_table->match_variable(name); // возвращаем из старшей области видимости
}

std::shared_ptr<StructType> Scope::match_struct(std::string name){
    if (structs.find(name) != structs.end()){
        return structs.at(name);
    }
    if (prev_table == nullptr){
        throw std::runtime_error("Struct '" + name + "' not found in scope."); //some expression not found struct
    }
    return prev_table->match_struct(name);
}

std::vector<std::shared_ptr<FuncType>> Scope::match_functions(std::string name){
    auto range = functions.equal_range(name);
    std::vector<std::shared_ptr<FuncType>> matched_functions;
    for (auto i = range.first; i != range.second; ++i){
        matched_functions.push_back(i->second); // собрали все функции с этим именем
    }
    if (prev_table != nullptr) {
        auto prev_funcs = prev_table->match_functions(name);
        matched_functions.insert(matched_functions.end(), prev_funcs.begin(), prev_funcs.end());
    } 
    return matched_functions;
}

void Scope::push_variable(std::string name, std::shared_ptr<Type> var) {
    variables.insert({name, var});
}

void Scope::push_struct(std::string name, std::shared_ptr<StructType> structure){
    structs.insert({name, structure});
}

void Scope::push_func(std::string name, std::shared_ptr<FuncType> func) {
    functions.insert({name, func});
}