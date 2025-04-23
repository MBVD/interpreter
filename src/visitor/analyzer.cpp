#include "analyzer.hpp"
#include "token.hpp"
#include "memory"

std::unordered_map<std::string, Type> Analyzer::default_types = {
    {"int", IntegerType()},
    {"double", FloatType()},
    {"float", FloatType()},
    {"char", CharType()}
};

Analyzer::Analyzer() : scope(nullptr) {}

void Analyzer::analyze(TranslationUnit & unit){
    for (auto& i : unit.get_nodes()){
        this->visit(i.get());
    }
}

void Analyzer::visit(ASTNode* node){
    this->visit(node);
}

void Analyzer::visit(VarDeclarator* node){
    auto type = node->get_type();
    auto declarations = node->get_init_declarators();
    if (type.type == TokenType::ID){
        // значит может быть какой то сложной фигней
        // значит если нет его в нашей таблице видимости значит ошиька семанитики. нет такого типа данных
        auto struct_type = scope->match_struct(type.value); // вернет либо обьект либо экспешн что такой структуры нет
        for (auto& i : declarations){
            auto name = i->get_declarator()->get_id();
            scope->push_variable(name.value, struct_type);
        }
    }
    if (type.type == TokenType::TYPE){
        // дефолтный типы по типу int char и тд
        auto current_type = default_types.at(type.value);
        for (auto& i : declarations){
            auto name = i->get_declarator()->get_id();
            scope->push_variable(name.value, current_type);
        }
    }
}

void Analyzer::visit(FuncDeclarator* node){
    auto returnable_type = node->get_returnable_type();
    auto name = node->get_name().value;
    if (returnable_type.type == TokenType::ID){
        auto struct_type = scope->match_struct(returnable_type.value); // вернет либо обьект либо экспешн что такой структуры нет
        auto args = node->get_params();
        std::vector<Type> type_args;
        for (auto& i : args){
            this->visit(i.get()); // поверяем являются ли они в зоне видимости
            type_args.push_back(current_type);
        }
        auto type_returnable = scope->match_struct(returnable_type.value);
        auto func = FuncType(type_returnable, type_args);
        auto block = node->get_block();
        auto current_scope = scope->create_new_table(std::move(block)); // создаем новую таблицу видимости для блока функции
        scope = current_scope;
        this->visit(block.get());
    } else {
        auto default_type = default_types.at(returnable_type.value);
        auto args = node->get_params();
        std::vector<Type> type_args;
        for (auto& i : args){
            this->visit(i.get()); // поверяем являются ли они в зоне видимости
            type_args.push_back(current_type);
        }
        auto func = FuncType(default_type, type_args);
        auto block = node->get_block();
        auto current_scope = scope->create_new_table(std::move(block)); // создаем новую таблицу видимости для блока функции
        scope = current_scope;
        this->visit(block.get()); // заходим в наш блок
    }
}

