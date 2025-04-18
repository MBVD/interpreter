#include "analyzer.hpp"
#include "token.hpp"

Analyzer::Analyzer() : scope(nullptr, nullptr) {}

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
        auto struct_type = scope.match_struct(type.value); // вернет либо обьект либо экспешн что такой структуры нет
        for (auto& i : declarations){
            auto name = i->get_declarator()->get_id();
            scope.push_variable(name.value, struct_type);
        }
    }
    if (type.type == TokenType::TYPE){
        // дефолтный типы по типу int char и тд
        if (type.value == "int"){
            for (auto& i : declarations){
                auto name = i->get_declarator()->get_id();
                scope.push_variable(name.value, IntegerType());
            }
        }
        if (type.value == "char"){
            for (auto& i : declarations){
                auto name = i->get_declarator()->get_id();
                scope.push_variable(name.value, IntegerType());
            }
        }
        if (type.value == "double" || type.value == "float"){
            for (auto& i : declarations){
                auto name = i->get_declarator()->get_id();
                scope.push_variable(name.value, IntegerType());
            }
        }
    }
}

