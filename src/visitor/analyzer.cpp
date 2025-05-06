#include "analyzer.hpp"
#include "token.hpp"
#include <memory>

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
    auto returnable_type_token = node->get_returnable_type();
    auto name = node->get_name().value;
    auto default_type = get_type(returnable_type_token);
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
    scope->push_func(name, func);
    this->visit(block.get()); // заходим в наш блок
    scope = scope->get_prev_table();
    current_type = func;
}

void Analyzer::visit(ParamDeclarator* node) {
    auto type = node->get_type();
    auto init_declarator = node->get_declorator();
    current_type = get_type(type);
    this->visit(init_declarator.get());
}

void Analyzer::visit(StructDeclarator* node) {
    auto id = node->get_id();
    auto vars = node->get_vars();
    std::unordered_map<std::string, Type> struct_vars;
    for (auto& var : vars){
        this->visit(var.get());
        auto name = var->get_type().value;
        struct_vars[name] = current_type;
    }
    auto str = StructType(struct_vars);
    scope->push_struct(id.value, str);
    current_type = str;
}

void Analyzer::visit(ComparisonExpression* node){
    auto left = node->get_left();
    auto right = node->get_right();
    auto op = node->get_op();
    this->visit(left.get());
    auto left_type = current_type;
    this->visit(right.get());
    auto right_type = current_type;
    if (dynamic_cast<StructType*>(&left_type)){
        // найти оператор сравнения для него 
        // либо конвертацию
    } else {
        left_type = BoolType();
    }
    if (dynamic_cast<StructType*>(&right_type)){

    } else {
        right_type = BoolType();
    }
    

}

void Analyzer::visit(ComparisonExpression* node) {

}

void Analyzer::visit(TernaryExpression* node) {

}

void Analyzer::visit(BinaryExpression* node) {

}

void Analyzer::visit(UnaryExpression* node) {

}

void Analyzer::visit(PostfixExpression* node) {

}

void Analyzer::visit(SubscriptExpression* node) {

}

void Analyzer::visit(CallExpression* node) {

}

void Analyzer::visit(AccessExpression* node) {
    auto expression = node->get_expression();
    this->visit(expression.get());
    if (typeid(FuncType()) == typeid(current_type)){
        auto* func_type = dynamic_cast<FuncType*>(&current_type);
        current_type = func_type->get_returnable_type();
    } else if (typeid(StructType()) == typeid(current_type)){
        // найти в структуре оператор ()
    } else {
        throw ;
    }   

}

void Analyzer::visit(LiteralNumExpression* node) {
    current_type = IntegerType();
}

void Analyzer::visit(LiteralFloatExpression* node) {
    current_type = FloatType();
}

void Analyzer::visit(LiteralCharExpression* node) {
    current_type = CharType();
}

void Analyzer::visit(LiteralStringExpression* node) {
    
}

void Analyzer::visit(IDexpression* node) {
    try {
        current_type = scope->match_function(node->get_token().value);
    } catch (...) {}
    current_type = scope->match_variable(node->get_token().value);
}

void Analyzer::visit(GroupExpression* node) {
    this->visit(node->get_base().get());
}


Type Analyzer::get_type(Token token){
    if (token == TokenType::ID){
        return scope->match_struct(token.value);
    } else {
        return default_types.at(token.value);
    }
}

