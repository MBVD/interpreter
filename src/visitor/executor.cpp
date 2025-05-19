#include "executor.hpp"

Executor::Executor() : table(std::make_shared<SymbolTable>()) {};

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
    const auto& declarations = node->get_init_declarators();
    for (const auto& init_declorator : declarations){
        init_declorator->accept(*this);
    }
}

void Executor::visit(InitDeclarator* node) {
    const auto& id_declarator = node->get_declarator();
    const auto& expression = node->get_expression();
    if (expression != nullptr){
        expression->accept(*this);
        auto value = current_value;
        id_declarator->accept(*this);
        table->push_variable(current_namespace + id_declarator->get_id().value, value);
    } else {
        id_declarator->accept(*this);
    }
}

void Executor::visit(IdDeclorator* node){
    auto name = node->get_id().value;
    auto type = node->get_declorator_type();
    switch(type){
        case IDDeclaratorType::NONE : {
            table->push_variable(current_namespace + name, 0); // типо дефолтное значение
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
    current_namespace = id.value + "::";
    const auto& vars = node->get_vars();
    for (const auto& i : vars) {
        i->accept(*this);
    }
    current_namespace = "";
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
    if (std::any_cast<int>(left_value) && std::any_cast<int>(right_value)){ // TODO
        current_value = left_value == right_value;
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
    right->accept(*this);
    auto right_value = current_value;
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
        current_value = std::any_cast<int>(base_value) + 1;
    } else if (op == TokenType::DECREMENT){
        current_value = std::any_cast<int>(base_value) - 1;
    }
}


