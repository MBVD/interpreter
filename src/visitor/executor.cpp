#include "executor.hpp"

Executor::Executor() : table(std::make_shared<SymbolTable>()) {};

std::unordered_map<std::string, std::shared_ptr<Arithmetic>> Executor::default_types_values = {
    {"int", std::make_shared<IntegerType>(0)},
    {"char", std::make_shared<CharType>('\0')},
    {"float", std::make_shared<FloatType>(0.0f)},
    {"bool", std::make_shared<BoolType>(false)}
};

std::shared_ptr<Type> Executor::get_default_type(std::string name) {
    auto val = default_types_values[name];
    if (dynamic_cast<IntegerType*>(val.get())) {
        return std::make_shared<IntegerType>(val->get_any_value());
    }
    if (dynamic_cast<CharType*>(val.get())) {
        return std::make_shared<CharType>(val->get_any_value());
    }
    if (dynamic_cast<FloatType*>(val.get())) {
        return std::make_shared<FloatType>(val->get_any_value());
    }
    if (dynamic_cast<BoolType*>(val.get())) {
        return std::make_shared<BoolType>(val->get_any_value());
    }
}

void Executor::execute(TranslationUnit& unit){
    for (const auto& i : unit.get_nodes()){
        i->accept(*this);
    }
}

void Executor::visit(ASTNode* node){
    node->accept(*this);
}

void Executor::visit(Declarator* node){
    node->accept(*this);
}

void Executor::visit(VarDeclarator* node) {
    auto type = node->get_type();
    if (type != TokenType::ID) {
        current_value = get_default_type(type.value);
    } else {
        current_value = table->match_struct(type.value);
    }

    auto default_var_value = current_value;
    const auto& declarations = node->get_init_declarators();
    for (const auto& init_declorator : declarations){
        init_declorator->accept(*this);
        current_value = default_var_value;
    }
}

void Executor::visit(InitDeclarator* node) {
    const auto& id_declarator = node->get_declarator();
    const auto& expression = node->get_expression();
    if (expression != nullptr){
        expression->accept(*this);
        auto value = current_value;
        table->push_variable(id_declarator->get_id().value, value);
    } else {
        id_declarator->accept(*this);
    }
}

void Executor::visit(IdDeclorator* node){
    auto name = node->get_id().value;
    auto type = node->get_declorator_type();
    switch(type){
        case IDDeclaratorType::NONE : {
            table->push_variable(name, current_value); // типо дефолтное значение
        } break;
        case IDDeclaratorType::POINTER : {
            // создать тип данных pointer 
        } break;
        case IDDeclaratorType::REF : {
            // создать тип данных ref
        } break;
        case IDDeclaratorType::ARRAY : {
            // создать тип данных array
        } break;
    }
}

void Executor::visit(FuncDeclarator* node) {
    auto returnable_type_token = node->get_returnable_type();
    auto name = node->get_name().value;
    const auto& args = node->get_params();
    const auto& block = node->get_block();
    // TODO
}

void Executor::visit(ParamDeclarator* node) {
    // TODO
}

void Executor::visit(StructDeclarator* node) {
    auto id = node->get_id();
    const auto& vars = node->get_vars();
    std::unordered_map<std::string, std::shared_ptr<Type>> struct_vars;
    for (const auto& i : vars) {
        i->accept(*this);
        for (const auto& init_declorator : i->get_init_declarators()) {
            init_declorator->accept(*this);
            auto name = init_declorator->get_declarator()->get_id().value;
            struct_vars[name] = current_value;
        }
    }
    auto struc = std::make_shared<StructType>(struct_vars);
    table->push_struct(id.value, struc);
}

void Executor::visit(Expression* node) {
    node->accept(*this);
}

void Executor::visit(ComparisonExpression* node){
    const auto& left = node->get_left();
    const auto& right = node->get_right();
    auto op = node->get_op();
    left->accept(*this);
    auto left_value = current_value;
    right->accept(*this);
    auto right_value = current_value;
    if (dynamic_cast<Arithmetic*>(left_value.get()) && dynamic_cast<Arithmetic*>(right_value.get())){ // TODO
        auto left_any_val = dynamic_cast<Arithmetic*>(left_value.get())->get_any_value();
        auto right_any_val = dynamic_cast<Arithmetic*>(right_value.get())->get_any_value();
        if (op == TokenType::EQUAL){ // тут еще надо дописать для других типов
            current_value = std::make_shared<BoolType>(std::any_cast<int>(left_any_val) == std::any_cast<int>(right_any_val));
        } else if (op == TokenType::NOT_EQUAL){
            current_value = std::make_shared<BoolType>(std::any_cast<int>(left_any_val) != std::any_cast<int>(right_any_val));
        } else if (op == TokenType::LESS){
            current_value = std::make_shared<BoolType>(std::any_cast<int>(left_any_val) < std::any_cast<int>(right_any_val));
        } else if (op == TokenType::LESS_EQUAL){
            current_value = std::make_shared<BoolType>(std::any_cast<int>(left_any_val) <= std::any_cast<int>(right_any_val));
        } else if (op == TokenType::GREATER){
            current_value = std::make_shared<BoolType>(std::any_cast<int>(left_any_val) > std::any_cast<int>(right_any_val));
        } else if (op == TokenType::GREATER_EQUAL){
            current_value = std::make_shared<BoolType>(std::any_cast<int>(left_any_val) >= std::any_cast<int>(right_any_val));
        }
    } else {
        throw std::runtime_error("error ");
    }
}

void Executor::visit(TernaryExpression* node) {
    const auto& cond_expr = node->get_cond_expression();
    const auto& true_expr = node->get_true_expression();
    const auto& false_expr = node->get_false_expression();
    cond_expr->accept(*this);
    auto cond_value = current_value;
    auto cond_any_val = dynamic_cast<BoolType*>(cond_value.get())->get_any_value();
    if (std::any_cast<bool>(cond_value)){
        true_expr->accept(*this);
        current_value = current_value;
    } else {
        false_expr->accept(*this);
        current_value = current_value;
    }
}
void Executor::visit(BinaryExpression* node) {
    const auto& left = node->get_left();
    const auto& right = node->get_right();
    auto op = node->get_op();
    left->accept(*this);
    auto left_value = current_value;
    auto left_any_val = dynamic_cast<Arithmetic*>(left_value.get())->get_any_value();
    right->accept(*this);
    auto right_value = current_value;
    auto right_any_val = dynamic_cast<Arithmetic*>(right_value.get())->get_any_value();
    if (std::any_cast<int>(left_value) && std::any_cast<int>(right_value)){ // TODO
        if (op == TokenType::PLUS){
            current_value = std::any_cast<int>(left_value) + std::any_cast<int>(right_value);
        } else if (op == TokenType::MINUS){
            current_value = std::any_cast<int>(left_value) - std::any_cast<int>(right_value);
        } else if (op == TokenType::MULTIPLY){
            current_value = std::any_cast<int>(left_value) * std::any_cast<int>(right_value);
        } else if (op == TokenType::DIVIDE){
            current_value = std::any_cast<int>(left_value) / std::any_cast<int>(right_value);
        }
    } else {
        throw std::runtime_error("error ");
    }
}

void Executor::visit(UnaryExpression* node) {// ++ -- (int) 
    const auto& base = node->get_base();
    auto op = node->get_op();
    base->accept(*this);
    auto base_value = current_value;
    if (op == TokenType::INCREMENT){ // TODO
        current_value = std::any_cast<int>(base_value) + 1;
    } else if (op == TokenType::DECREMENT){
        current_value = std::any_cast<int>(base_value) - 1;
    } else if (op == TokenType::PLUS){
        current_value = std::any_cast<int>(base_value);
    } else if (op == TokenType::MINUS){
        current_value = -std::any_cast<int>(base_value);
    }
}

void Executor::visit(PostfixExpression* node){
    const auto& base = node->get_expression();
    auto op = node->get_op();
    base->accept(*this);
    auto base_value = current_value;
    if (op == TokenType::INCREMENT){ // TODO
        current_value = std::any_cast<std::shared_ptr<IntegerType>>(base_value) + 1;
    } else if (op == TokenType::DECREMENT){
        current_value = std::any_cast<int>(base_value) - 1;
    }
}


