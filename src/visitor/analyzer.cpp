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
    }
    if (dynamic_cast<StructType*>(&right_type)){
        // 
    }

    if (dynamic_cast<Arithmetic*>(&left_type) && dynamic_cast<Arithmetic*>(&right_type)) {
        current_type = BoolType();
    } else {
        throw std::runtime_error("error ");
    }
}

void Analyzer::visit(TernaryExpression* node) {
    auto cond_expr = node->get_cond_expression();
    auto true_expr = node->get_true_expression();
    auto false_expr = node->get_false_expression();
    this->visit(cond_expr.get());
    auto cond_type = current_type;
    this->visit(true_expr.get());
    auto true_expr_type = current_type;
    this->visit(false_expr.get());
    auto false_expr_type = current_type;
    if (!dynamic_cast<BoolType*>(&cond_type)) {
        throw std::runtime_error("error in checking cond expression");
    }

    if (dynamic_cast<Composite*>(&true_expr_type)) {
        // 
    }
    if (dynamic_cast<Composite*>(&false_expr_type)){
        //
    }

    if(dynamic_cast<decltype(true_expr_type)*>(&false_expr_type)) { // значит тип правый конвертируется в левый тип
        current_type = true_expr_type;
    }
    throw std::runtime_error("hello kitty");
}

void Analyzer::visit(BinaryExpression* node) {
    auto left = node->get_left();
    auto right = node->get_right();
    auto op = node->get_op();
    this->visit(left.get());
    auto left_type = current_type;
    this->visit(right.get());
    auto right_type = current_type;

    if (dynamic_cast<Composite*>(&left_type)) {
        //
    }
    if (dynamic_cast<Composite*>(&right_type)) {
        //
    }
    if (dynamic_cast<Arithmetic*>(&left_type) && dynamic_cast<Arithmetic*>(&right_type)) {
        current_type = left_type;
    }
    throw std::runtime_error("hello kerropi");
}

void Analyzer::visit(PostfixExpression* node) {
    auto expression = node-> get_expression();
    auto op = node->get_op(); // ну тут может быть только ++ или --
    this->visit(expression.get());
    auto expression_type = current_type;
    if (!dynamic_cast<Arithmetic*>(&expression_type)) {
        throw std::runtime_error("hello melodi");
    }
}

void Analyzer::visit(SubscriptExpression* node) {// []
    auto expression = node->get_expression();
    auto indexes = node->get_indexes();
    auto op = node->get_op();
    this->visit(expression.get());
    auto expression_type = current_type;
    if (dynamic_cast<StructType*>(&expression_type)) {
        // ищем оператор []
    }
    for (auto& i : indexes){
        this->visit(i.get());
        if (!dynamic_cast<IntegerType*>(&current_type)){
            throw std::runtime_error("hello maru");
        }
    }
    auto* pointer = dynamic_cast<PointerType*>(&expression_type);
    if (!pointer) {
        throw std::runtime_error("hello maru");    
    }
    if (indexes.size() >= pointer->get_star_count()) {
        throw std::runtime_error("hello maru");
    }
    current_type = pointer->get_type_by_star_count(indexes.size());
}

void Analyzer::visit(CallExpression* node) { // (
    auto expression = node->get_expression();
    auto op = node->get_op();
    auto args = node->get_args();
    this->visit(expression.get());
    auto expression_type = current_type;
    if (dynamic_cast<StructType*>(&expression_type)) {
        //
    }
    auto* expression_func = dynamic_cast<FuncType*>(&expression_type);
    if (!expression_func) {
        throw std::runtime_error("hello there");
    }
    auto expr_func_args = expression_func->get_args();
}

void Analyzer::visit(AccessExpression* node) { // ->
    
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
        current_type = scope->match_function(node->get_token().value, std::vector<Type>{});
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

