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
            auto id_declorator = init_declorator->get_declarator();
            auto name = id_declorator->get_id().value;
            auto expression = init_declorator->get_expression();
            if (expression != nullptr){
                this->visit(expression.get()); // проходимся по expression и проверяем является ли он типом который может конвертироваться в нашу структуру
                // TODO нужно сделать метод который ищет какой scope для нашей структуры и там находить func - конструктор с таким параметром
                // if (typeid(current_type) == typeid(struct_type)) // пока так, потом будем проверять есть ли конструктор
            } // здесь просто проверка
            // надо дополнительно пройтись в id declorator и проверить явлется ли он массивом
            this->visit(id_declorator.get()); // будет либо тот же тип либо маасив, указатель или ссылка в current_type
            scope->push_variable(name, current_type);
        }
    }
    if (type.type == TokenType::TYPE){
        // дефолтный типы по типу int char и тд
        for (auto& init_declorator : declarations){
            auto current_type = default_types.at(type.value);
            auto id_declorator = init_declorator->get_declarator();
            auto name = id_declorator->get_id().value;
            this->visit(id_declorator.get()); // после того как мы пройдем сюда мы поменяем current_type на маасив и тд если они такими являются
            scope->push_variable(name, current_type);
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

void Analyzer::visit(InitDeclarator* node) {

}

