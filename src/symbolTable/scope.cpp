#include <unordered_map>
#include <set>
#include "scope.hpp"
#include "memory"

Scope::Scope(std::unique_ptr<ASTNode> node) : 
    node(std::move(node)) {}

std::shared_ptr<Scope> Scope::get_prev_table() {
    return this->prev_table;
}

std::shared_ptr<Scope> Scope::create_new_table(std::shared_ptr<Scope> prev_scope, std::unique_ptr<ASTNode> node) {
    prev_scope = prev_scope;
    auto scope = std::make_shared<Scope>(std::move(node));
    return scope;
}   

Type Scope::match_variable(std::string name) {
    if (variables.find(name) != variables.end()){
        return variables.at(name);
    }
    return this->prev_table->match_variable(name); // возвращаем из старшей области видимости
}

StructType Scope::match_struct(std::string name){
    if (structs.find(name) != structs.end()){
        return structs.at(name);
    }
    if (prev_table == nullptr){
        throw; //some expression not found struct
    }
    return prev_table->match_struct(name);
}

FuncType Scope::match_function(std::string name, std::vector<Type> args){
    auto range = functions.equal_range(name);
    std::map<FuncType, int> functions; 
    for (auto i = range.first; i != range.second; ++i){
        functions.insert({i->second, 0}); // собрали все функции с этим именем
    }
    for (auto function : functions){
        auto func_args = function.first.get_args();
        if (func_args.size() != args.size()){
            functions.erase(function.first);
        }
        bool match = true;
        for (size_t j = 0; j < args.size(); ++j) {
            if (typeid(args[j]) != typeid(func_args[j])) {
                match = false; // Если типы не совпадают, функция не подходит
                break;
            }
            // TODO реализовать чтоб типы были конвертируемы сейчас ищет только идеальное совпадение типов
        }

        if (match) {
            return function.first; // Возвращаем первую подходящую функцию
        }
    }
    if (prev_table.get() == nullptr){
        throw; //throw exceptions no exist function;
    }
    
    return prev_table->match_function(name, args);
}

void Scope::push_variable(std::string name, Type var) {
    variables.insert({name, var});
}

void Scope::push_struct(std::string name, StructType structure){
    structs.insert({name, structure});
}

void Scope::push_func(std::string name, FuncType func) {
    functions.insert({name, func});
}