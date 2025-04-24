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
        for (auto& init_declorator : declarations){
            current_type = scope->match_struct(type.value); // вернет либо обьект либо экспешн что такой структуры нет
            this->visit(init_declorator.get());
        }
    }
    if (type.type == TokenType::TYPE){
        // дефолтный типы по типу int char и тд
        for (auto& init_declorator : declarations){
            auto current_type = default_types.at(type.value);
            this->visit(init_declorator.get());
        }
    }
}

void Analyzer::visit(InitDeclarator* node) {
    auto id_declarator = node->get_declarator();
    auto expression = node->get_expression();
    if (expression != nullptr){
        this->visit(expression.get()); // проходимся по expression и проверяем является ли он типом который может конвертироваться в нашу структуру
        // TODO нужно сделать метод который ищет какой scope для нашей структуры и там находить func - конструктор с таким параметром
        // if (typeid(current_type) == typeid(struct_type)) // пока так, потом будем проверять есть ли конструктор
    }
    auto  expression_type = current_type;   
    this->visit(id_declarator.get());
}

void Analyzer::visit(IdDeclorator* node){
    auto name = node->get_id().value;
    auto type = node->get_declorator_type();
    switch(type){
        case IDDeclaratorType::NONE : {
            scope->push_variable(name, current_type);
        } break;
        case IDDeclaratorType::POINTER : {
            // создать тип данных pointer 
        } break;
        case IDDeclaratorType::ARRAY : {
            // создать тип данных массив
        } break;
        case IDDeclaratorType::REF : {
            // создать тип данных сссылка
        } break;
    }
}

void Analyzer::visit(FuncDeclarator* node){
    auto returnable_type = node->get_returnable_type();
    auto name = node->get_name().value;
    if (returnable_type.type == TokenType::ID){
        auto struct_type = scope->match_struct(returnable_type.value); // вернет либо обьект либо экспешн что такой структуры нет
        auto args = node->get_params();
        std::vector<Type> type_args;
        auto block = node->get_block();
        scope = scope->create_new_table(scope, std::move(block));
        for (auto& i : args){
            this->visit(i.get()); // поверяем являются ли они в зоне видимости
            type_args.push_back(current_type);
            scope->push_variable(i->get_type().value, current_type);
        }
        this->visit(block.get());
        auto type_returnable = scope->match_struct(returnable_type.value);
        auto func = FuncType(type_returnable, type_args);
        scope = scope->get_prev_table();
        scope->push_func(name, func);
        current_type = func;
    } else {
        auto default_type = default_types.at(returnable_type.value);
        auto args = node->get_params();
        auto block = node->get_block();
        std::vector<Type> type_args;
        scope = scope->create_new_table(scope, std::move(block));
        for (auto& i : args){
            this->visit(i.get()); // поверяем являются ли они в зоне видимости
            type_args.push_back(current_type);
            scope-> push_variable(i->get_type().value, current_type);
        }
        auto func = FuncType(default_type, type_args);
        this->visit(block.get()); // заходим в наш блок
        scope = scope->get_prev_table();
        scope->push_func(name, func);
        current_type = func;
    }
}

