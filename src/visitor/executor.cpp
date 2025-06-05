#include "executor.hpp"
#include <algorithm>
#include <cmath>
#include "type.hpp"

Executor::Executor() : symbolTable(std::make_shared<Scope>(nullptr)) {};

std::unordered_map<std::string, std::shared_ptr<Symbol>> Executor::default_types = {
    {"int", std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(0))},
    // {"string", std::make_shared<VarSymbol>(std::make_shared<StringType>())},
    {"double", std::make_shared<VarSymbol>(std::make_shared<FloatType>(), std::make_any<double>(0))},
    {"float", std::make_shared<VarSymbol>(std::make_shared<FloatType>(), std::make_any<double>(0))},
    {"char", std::make_shared<VarSymbol>(std::make_shared<CharType>(), std::make_any<char>('\0'))},
    {"bool", std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(true))},
    {"void", std::make_shared<VarSymbol>(std::make_shared<VoidType>())}
};

std::shared_ptr<Symbol> Executor::match_symbol(const Token& token) {
    if (token.type == TokenType::ID) {
        auto symbol = symbolTable->match_global(token.value);
        if (!symbol) {
            throw std::runtime_error("Symbol '" + token.value + "' not found in scope.");
        }
        return symbol;
    } else {
        auto it = default_types.find(token.value);
        if (it != default_types.end()) {
            return it->second;
        } else {
            throw std::runtime_error("Unknown type: " + token.value);
        }
    }
}

void Executor::execute(TranslationUnit& unit){
    for (const auto& i : unit.get_nodes()){
        i->accept(*this);
    }
    auto main = symbolTable->match_global("main");
    if (!main || !main->is_func()) {
        throw std::runtime_error("Main function not found or is not a function.");
    }
    auto func_type = std::dynamic_pointer_cast<FuncSymbol>(main);
    if (func_type->func_declarator->get_block() != nullptr) {
        func_type->func_declarator->get_block()->accept(*this);
    } else {
        throw std::runtime_error("Main function has no block.");
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
    current_value = match_symbol(type);

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
        auto value = std::dynamic_pointer_cast<VarSymbol>(current_value)->value;
        id_declarator->accept(*this);
    } else {
        id_declarator->accept(*this);
    }
}

void Executor::visit(IdDeclorator* node){
    auto name = node->get_id().value;
    auto type = node->get_declorator_type();
    switch(type){
        case IDDeclaratorType::NONE : {
            if (current_value->is_record()) {
                auto record_type = std::dynamic_pointer_cast<RecordType>(current_value->type);
                if (record_type) {
                    auto record_symbol = std::dynamic_pointer_cast<Record>(current_value);
                    symbolTable->push_symbol(name, std::make_shared<Record>(record_type, record_symbol->fields));
                }
            } else if (!is_rvalue) {
                symbolTable->push_symbol(name, current_value);
            } else {
                auto value = std::dynamic_pointer_cast<VarSymbol>(current_value)->value;
                symbolTable->push_symbol(name, std::make_shared<VarSymbol>(current_value->type, value));
            }
        } break;
        case IDDeclaratorType::POINTER : {
            symbolTable->push_symbol(name, std::make_shared<VarSymbol>(std::make_shared<PointerType>(current_value->type)));
        } break;
        case IDDeclaratorType::REF : {
            
        } break;
        case IDDeclaratorType::ARRAY : {
            auto array_element_type = std::dynamic_pointer_cast<VarSymbol>(current_value);
            const auto& expression = node->get_expression();
            expression->accept(*this);
            auto size = std::any_cast<int>(std::dynamic_pointer_cast<VarSymbol>(current_value)->value);
            std::vector<std::shared_ptr<Symbol>> array_elements;
            for (int i = 0; i < size; ++i) {
                auto array_element = std::make_shared<VarSymbol>(array_element_type->type, array_element_type->value);
                array_elements.push_back(array_element);
            }
            symbolTable->push_symbol(name, std::make_shared<VarSymbol>(std::make_shared<ArrayType>(current_value->type, array_elements)));
        } break;
    }
}

void Executor::visit(FuncDeclarator* node) {
    auto returnable_type_token = node->get_returnable_type();
    auto name = node->get_name().value;
    const auto& block = node->get_block();
    auto default_type = match_symbol(returnable_type_token)->type;
    const auto& args = node->get_params();
    std::vector<std::shared_ptr<Type>> type_args;
    symbolTable = symbolTable->create_new_table(symbolTable);
    for (const auto& i : args){
        i->accept(*this);
        type_args.push_back(current_value->type);
        symbolTable->push_symbol(i->get_type().value, std::make_shared<VarSymbol>(current_value->type));
    }
    auto func = std::make_shared<FuncType>(default_type, type_args);
    symbolTable = symbolTable->get_prev_table();
    symbolTable->push_symbol(name, std::make_shared<FuncSymbol>(func, node));
}

void Executor::visit(ParamDeclarator* node) {
    auto type = node->get_type();
    const auto& init_declarator = node->get_declorator();
    current_value = match_symbol(type);
    init_declarator->accept(*this);
}

void Executor::visit(StructDeclarator* node) {
    auto id = node->get_id();
    const auto& vars = node->get_vars();
    symbolTable = symbolTable->create_new_table(symbolTable);
    for (const auto& var : vars) {
        var->accept(*this);
    }
    auto scope_multi_vars = symbolTable->get_symbols();
    symbolTable = symbolTable->get_prev_table();
    std::unordered_map<std::string, std::shared_ptr<Type>> struct_vars;
    std::unordered_map<std::string, std::shared_ptr<Symbol>> scope_vars;
    for (auto var : scope_multi_vars) {
        struct_vars[var.first] = var.second->type;
        scope_vars[var.first] = var.second;
    }
    auto struc = std::make_shared<StructType>(struct_vars);
    auto struc_symbol = std::make_shared<Record>(struc, scope_vars);
    symbolTable->push_symbol(id.value, struc_symbol);
}

void Executor::visit(Expression* node) {
    node->accept(*this);
}

void Executor::visit(ComparisonExpression* node){
    const auto& left = node->get_left();
    const auto& right = node->get_right();
    auto op = node->get_op();
    left->accept(*this);
    auto left_value = std::dynamic_pointer_cast<VarSymbol>(current_value);
    right->accept(*this);
    auto right_value = std::dynamic_pointer_cast<VarSymbol>(current_value);
    auto left_type = left_value->type;
    auto right_type = right_value->type;
    auto counted = binary_operation(left_value, op, right_value);
    current_value = counted;
}

void Executor::visit(TernaryExpression* node) {
    const auto& cond_expr = node->get_cond_expression();
    const auto& true_expr = node->get_true_expression();
    const auto& false_expr = node->get_false_expression();
    cond_expr->accept(*this);
    auto cond_value = current_value;
    auto cond_any_val = dynamic_cast<VarSymbol*>(cond_value.get())->value;
    if (std::any_cast<bool>(cond_value)){
        true_expr->accept(*this);
    } else {
        false_expr->accept(*this);
    }
}
void Executor::visit(BinaryExpression* node) {
    const auto& left = node->get_left();
    const auto& right = node->get_right();
    auto op = node->get_op();
    left->accept(*this);
    auto left_value = std::dynamic_pointer_cast<VarSymbol>(current_value);
    auto left_any_val = dynamic_cast<VarSymbol*>(left_value.get())->value;
    right->accept(*this);
    auto right_value = std::dynamic_pointer_cast<VarSymbol>(current_value);
    auto right_any_val = dynamic_cast<VarSymbol*>(right_value.get())->value;
    current_value = binary_operation(left_value, op, right_value);
}

void Executor::visit(CommaExpression* node) {
    const auto& left = node->get_left();
    const auto& right = node->get_right();
    left->accept(*this);
    right->accept(*this);
}

void Executor::visit(AssignmentExpression* node) {
    const auto& left = node->get_left();
    const auto& right = node->get_right();
    auto op = node->get_op();
    left->accept(*this);
    auto left_value = std::dynamic_pointer_cast<VarSymbol>(current_value);
    right->accept(*this);
    auto right_value = std::dynamic_pointer_cast<VarSymbol>(current_value);
    
    if (left_value->is_const) {
        throw std::runtime_error("Cannot assign to a constant variable:");
    }
    
    current_value = assignment_operation(left_value, op, right_value);

}

void Executor::visit(LogicalOrExpression* node) {
    const auto& left = node->get_left();
    const auto& right = node->get_right();
    auto op = node->get_op();
    left->accept(*this);
    auto left_value = std::dynamic_pointer_cast<VarSymbol>(current_value);
    right->accept(*this);
    auto right_value = std::dynamic_pointer_cast<VarSymbol>(current_value);
    
    current_value = binary_operation(left_value, op, right_value);
}

void Executor::visit(LogicalAndExpression* node) {
    const auto& left = node->get_left();
    const auto& right = node->get_right();
    auto op = node->get_op();
    left->accept(*this);
    auto left_value = std::dynamic_pointer_cast<VarSymbol>(current_value);
    right->accept(*this);
    auto right_value = std::dynamic_pointer_cast<VarSymbol>(current_value);
    
    current_value = binary_operation(left_value, op, right_value);
}

void Executor::visit(BiteIncOrExpression* node) {
    const auto& left = node->get_left();
    const auto& right = node->get_right();
    auto op = node->get_op();
    left->accept(*this);
    auto left_value = std::dynamic_pointer_cast<VarSymbol>(current_value);
    right->accept(*this);
    auto right_value = std::dynamic_pointer_cast<VarSymbol>(current_value);
    
    current_value = binary_operation(left_value, op, right_value);
}

void Executor::visit(BiteExcOrExpression* node) {
    const auto& left = node->get_left();
    const auto& right = node->get_right();
    auto op = node->get_op();
    left->accept(*this);
    auto left_value = std::dynamic_pointer_cast<VarSymbol>(current_value);
    right->accept(*this);
    auto right_value = std::dynamic_pointer_cast<VarSymbol>(current_value);
    
    current_value = binary_operation(left_value, op, right_value);
}

void Executor::visit(BiteAndExpression* node) {
    const auto& left = node->get_left();
    const auto& right = node->get_right();
    auto op = node->get_op();
    left->accept(*this);
    auto left_value = std::dynamic_pointer_cast<VarSymbol>(current_value);
    right->accept(*this);
    auto right_value = std::dynamic_pointer_cast<VarSymbol>(current_value);
    
    current_value = binary_operation(left_value, op, right_value);  
}

void Executor::visit(ShiftExpression* node) {
    const auto& left = node->get_left();
    const auto& right = node->get_right();
    auto op = node->get_op();
    left->accept(*this);
    auto left_value = std::dynamic_pointer_cast<VarSymbol>(current_value);
    right->accept(*this);
    auto right_value = std::dynamic_pointer_cast<VarSymbol>(current_value);
    
    current_value = binary_operation(left_value, op, right_value);
}

void Executor::visit(UnaryExpression* node) {// ++ -- (int) 
    const auto& base = node->get_base();
    auto op = node->get_op();
    base->accept(*this);
    auto base_value = std::dynamic_pointer_cast<VarSymbol>(current_value);
    current_value = unary_operation(base_value, op);
}

void Executor::visit(PostfixExpression* node){
    const auto& base = node->get_expression();
    auto op = node->get_op();
    base->accept(*this);
    auto base_value = std::dynamic_pointer_cast<VarSymbol>(current_value);
    current_value = postgix_operation(base_value, op);
}

void Executor::visit(SubscriptExpression* node) { //[]
    const auto& expression = node->get_expression();
    const auto& indexes = node->get_indexes();
    auto op = node->get_op();
    expression->accept(*this);
    auto expression_value = current_value;
    if (expression_value->is_record()) {
        
    }
    std::vector<int> indexes_values;
    for (const auto& index : indexes) {
        index->accept(*this);
        auto index_value = std::any_cast<int>(std::dynamic_pointer_cast<VarSymbol>(current_value)->value);
        indexes_values.push_back(index_value);
    }
    if (expression_value->type->is_array()) {
        auto array_type = std::dynamic_pointer_cast<ArrayType>(expression_value->type);
        current_value = array_type->get_by_ids(indexes_values, 0);
    }
}
void Executor::visit(CallExpression* node) {
    const auto& expression = node->get_expression();
    const auto& args = node->get_args();
    auto op = node->get_op();
    expression->accept(*this);
    auto expression_value = current_value;
    if (expression_value->is_record()) {
        // TODO
    }
    auto matched_functions = symbolTable->get_mathched_functions();
    std::vector<int> func_ranks(matched_functions.size(), 0);
    for (auto function_it = matched_functions.begin(); function_it != matched_functions.end(); ++function_it) {
        auto function = *function_it;
        auto func_args = std::dynamic_pointer_cast<FuncType>(function->type)->get_args();
        if (func_args.size() != args.size()) { // если количество аргументов не совпадает
            matched_functions.erase(function_it);
            continue;
        }
        for (int i = 0; i < args.size(); ++i) {
            args[i]->accept(*this);
            auto arg_type = current_value->type;
            if (!can_convert(arg_type, func_args[i])) { // если конвертируется типы
                matched_functions.erase(function_it);
                continue;
            } else {
                func_ranks[std::distance(matched_functions.begin(), function_it)]+= getTypeRank(func_args[i]) - getTypeRank(arg_type);
            }
        }
    }
    auto index = *std::max_element(func_ranks.begin(), func_ranks.end());
    auto best_match = matched_functions[index];
    symbolTable = symbolTable->create_new_table(symbolTable);
    auto* func_decl_sp = dynamic_pointer_cast<FuncSymbol>(best_match)->func_declarator;
    const auto& params_names = func_decl_sp->get_params();
    for (int i = 0; i < params_names.size(); ++i) {
        const auto& param = params_names[i];
        auto param_name = param->get_declorator()->get_declarator()->get_id().value;
        args[i]->accept(*this);
        symbolTable->push_symbol(param_name, current_value);
    }
    matched_functions.clear();
    func_decl_sp->get_block()->accept(*this);
};
void Executor::visit(AccessExpression* node) {
    const auto& expression = node->get_expression();    
    auto member = node->get_member();
    auto op = node->get_op();
    expression->accept(*this);
    auto expression_value = current_value;
    if (expression_value->is_record()) {
        auto record = std::dynamic_pointer_cast<Record>(expression_value);
        auto member_symbol = record->fields[member.value];
        current_value = member_symbol;
    }
};
void Executor::visit(LiteralNumExpression* node) {
    auto value = node->get_value();
    current_value = std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(value));
};
void Executor::visit(LiteralFloatExpression* node) {
    auto value = node->get_value();
    current_value = std::make_shared<VarSymbol>(std::make_shared<FloatType>(), std::make_any<double>(value));
};
void Executor::visit(LiteralCharExpression* node) {
    auto value = node->get_value();
    current_value = std::make_shared<VarSymbol>(std::make_shared<CharType>(), std::make_any<char>(value));
};
void Executor::visit(LiteralStringExpression* node) {
    auto value = node->get_value();
    //TODO
    // current_value = std::make_shared<VarSymbol>(std::make_shared<StringType>(), value);
};
void Executor::visit(IDexpression* node) {
    auto token = node->get_token();
    current_value = match_symbol(token);
};
void Executor::visit(GroupExpression* node) {
    const auto& base = node->get_base();
    base->accept(*this);
};

void Executor::visit(Statement* node) {
    node->accept(*this);
}
void Executor::visit(ExpressionStatement* node) {
    const auto& expression = node->get_expression();
    if (expression != nullptr) {
        expression->accept(*this);
    }
}
void Executor::visit(DeclarationStatement* node){
    const auto& declaration = node->get_declaration();
    if (declaration != nullptr) {
        declaration->accept(*this);
    }
}
void Executor::visit(ReturnStatement* node) {
    const auto& expression = node->get_expression();
    expression->accept(*this);
    auto return_value = current_value;
    throw std::runtime_error("Return statement encountered with value:"); // TODO сделать отдельные exceptions для каждого случая
}
void Executor::visit(BreakStatement* node) {
    throw std::runtime_error("Break statement encountered");// TODO сделать отдельные exceptions для каждого случая
}
void Executor::visit(ContinueStatement* node) {
    throw std::runtime_error("Continue statement encountered");// TODO сделать отдельные exceptions для каждого случая
}
void Executor::visit(BlockStatement* node) {
    for (const auto& statement : node->get_statements()) {
        try {
            statement->accept(*this);
        } catch (const std::runtime_error& e) {
            return;
        }
        
    }
    symbolTable = symbolTable->get_prev_table();
}
void Executor::visit(ConditionalStatement* node) {
    const auto& conditional = node->get_conditional();
    const auto& true_statement = node->get_true_statement();
    const auto& false_statement = node->get_false_statement();
    
    conditional->accept(*this);
    auto cond_value = current_value;
    auto cond_any_val = dynamic_cast<VarSymbol*>(cond_value.get())->value;
    
    if (std::any_cast<bool>(cond_any_val)) {
        true_statement->accept(*this);
    } else if (false_statement) {
        false_statement->accept(*this);
    }    
}
void Executor::visit(LoopStatement* node) {
    node->accept(*this);
}
void Executor::visit(WhileStatement* node) {
    const auto& conditional = node->get_conditional();
    const auto& statement = node->get_statement();
    
    while (true) {
        conditional->accept(*this);
        auto cond_value = current_value;
        auto cond_any_val = dynamic_cast<VarSymbol*>(cond_value.get())->value;
        
        if (!std::any_cast<bool>(cond_any_val)) {
            break; // выход из цикла, если условие ложно
        }
        
        statement->accept(*this);// TODO отлавливать ошибки в циклах
    }
}
void Executor::visit(DoWhileStatement* node) {
    const auto& statement = node->get_statement();
    const auto& expression = node->get_expression();
    
    do {
        statement->accept(*this);
        expression->accept(*this);
        auto cond_value = current_value;
        auto cond_any_val = dynamic_cast<VarSymbol*>(cond_value.get())->value;
        
        if (!std::any_cast<bool>(cond_any_val)) {
            break;
        }
    } while (true);
}
void Executor::visit(ForStatement* node) {
    if (node->get_init_expr()) {
        node->get_init_expr()->accept(*this);
    }
    
    while (true) {
        if (node->get_cond_expr()) {
            node->get_cond_expr()->accept(*this);
            auto cond_value = current_value;
            auto cond_any_val = dynamic_cast<VarSymbol*>(cond_value.get())->value;
            if (!std::any_cast<bool>(cond_any_val)) {
                break; // выход из цикла, если условие ложно
            }
        }
        
        node->get_statement()->accept(*this);
        
        if (node->get_iter_expr()) {
            node->get_iter_expr()->accept(*this);
        }
    }
}
void Executor::visit(EmptyStatement* node) {
    return;
}

bool Executor::is_record_type(const std::shared_ptr<Type>& type){
    return dynamic_cast<RecordType*>(type.get()) != nullptr || dynamic_cast<StructType*>(type.get()) != nullptr;
}

std::shared_ptr<VarSymbol> Executor::binary_operation(std::shared_ptr<VarSymbol> left, Token& op, std::shared_ptr<VarSymbol> right) {
    if (is_record_type(left->type) || is_record_type(right->type)) {
        // TODO
    } else {
        switch (op.type) {
            case TokenType::PLUS: {
                if (left->type->is_integer()) {
                    is_rvalue = true;
                    auto left_value = std::any_cast<int>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::any_cast<int>(left_value + right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::any_cast<int>(left_value + right_value));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::any_cast<int>(left_value + right_value));
                    } else if (right->type->is_floating()) {
                        auto right_value = std::any_cast<double>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<FloatType>(), std::any_cast<double>(left_value + right_value));
                    }
                } else if (left->type->is_char()) {
                    auto left_value = std::any_cast<char>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::any_cast<int>(left_value + right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<CharType>(), std::any_cast<char>(left_value + right_value));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<CharType>(), std::any_cast<char>(left_value + right_value));
                    } else if (right->type->is_floating()) {
                        auto right_value = std::any_cast<double>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<FloatType>(), std::any_cast<double>(left_value + right_value));
                    }
                } else if (left->type->is_bool()) {
                    auto left_value = std::any_cast<bool>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::any_cast<int>(left_value + right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<CharType>(), std::any_cast<char>(left_value + right_value));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::any_cast<bool>(left_value + right_value));
                    } else if (right->type->is_floating()) {
                        auto right_value = std::any_cast<double>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<FloatType>(), std::any_cast<double>(left_value + right_value));
                    }
                } else if (left->type->is_floating()){
                    auto left_value = std::any_cast<double>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<FloatType>(), std::any_cast<double>(left_value + right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<FloatType>(), std::any_cast<double>(left_value + right_value));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<FloatType>(), std::any_cast<double>(left_value + right_value));
                    } else if (right->type->is_floating()) {
                        auto right_value = std::any_cast<double>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<FloatType>(), std::any_cast<double>(left_value + right_value));
                    }
                }
            } break;
            case TokenType::MINUS: {
                if (left->type->is_integer()) {
                    is_rvalue = true;
                    auto left_value = std::any_cast<int>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value - right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value - right_value));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value - right_value));
                    } else if (right->type->is_floating()) {
                        auto right_value = std::any_cast<double>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<FloatType>(), std::make_any<double>(left_value - right_value));
                    }
                } else if (left->type->is_char()) {
                    auto left_value = std::any_cast<char>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value - right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<CharType>(), std::make_any<char>(left_value - right_value));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<CharType>(), std::make_any<char>(left_value - right_value));
                    } else if (right->type->is_floating()) {
                        auto right_value = std::any_cast<double>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<FloatType>(), std::make_any<double>(left_value - right_value));
                    }
                } else if (left->type->is_bool()) {
                    auto left_value = std::any_cast<bool>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value - right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value - right_value));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value - right_value));
                    } else if (right->type->is_floating()) {
                        auto right_value = std::any_cast<double>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<FloatType>(), std::make_any<double>(left_value - right_value));
                    }
                } else if (left->type->is_floating()){
                    auto left_value = std::any_cast<double>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<FloatType>(), std::make_any<double>(left_value - right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<FloatType>(), std::make_any<double>(left_value - right_value));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<FloatType>(), std::make_any<double>(left_value - right_value));
                    } else if (right->type->is_floating()) {
                        auto right_value = std::any_cast<double>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<FloatType>(), std::make_any<double>(left_value - right_value));
                    }
                }
            } break;
            case TokenType::MULTIPLY: {
                if (left->type->is_integer()) {
                    is_rvalue = true;
                    auto left_value = std::any_cast<int>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value * right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value * right_value));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value * right_value));
                    } else if (right->type->is_floating()) {
                        auto right_value = std::any_cast<double>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<FloatType>(), std::make_any<double>(left_value * right_value));
                    }
                } else if (left->type->is_char()) {
                    auto left_value = std::any_cast<char>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value * right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value * right_value));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value * right_value));
                    } else if (right->type->is_floating()) {
                        auto right_value = std::any_cast<double>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<FloatType>(), std::make_any<double>(left_value * right_value));
                    }
                } else if (left->type->is_bool()) {
                    auto left_value = std::any_cast<bool>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value * right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value * right_value));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value * right_value));
                    } else if (right->type->is_floating()) {
                        auto right_value = std::any_cast<double>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<FloatType>(), std::make_any<double>(left_value * right_value));
                    }
                } else if (left->type->is_floating()){
                    auto left_value = std::any_cast<double>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<FloatType>(), std::make_any<double>(left_value * right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<FloatType>(), std::make_any<double>(left_value * right_value));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<FloatType>(), std::make_any<double>(left_value * right_value));
                    } else if (right->type->is_floating()) {
                        auto right_value = std::any_cast<double>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<FloatType>(), std::make_any<double>(left_value * right_value));
                    }
                }
            } break; 
            case TokenType::DIVIDE: {
                if (left->type->is_integer()) {
                    is_rvalue = true;
                    auto left_value = std::any_cast<int>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value / right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value / right_value));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value / right_value));
                    } else if (right->type->is_floating()) {
                        auto right_value = std::any_cast<double>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<FloatType>(), std::make_any<double>(left_value / right_value));
                    }
                } else if (left->type->is_char()) {
                    is_rvalue = true;
                    auto left_value = std::any_cast<char>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value / right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value / right_value));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value / right_value));
                    } else if (right->type->is_floating()) {
                        auto right_value = std::any_cast<double>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<FloatType>(), std::make_any<double>(left_value / right_value));
                    }
                } else if (left->type->is_bool()) {
                    is_rvalue = true;
                    auto left_value = std::any_cast<bool>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value / right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value / right_value));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value / right_value));
                    } else if (right->type->is_floating()) {
                        auto right_value = std::any_cast<double>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<FloatType>(), std::make_any<double>(left_value / right_value));
                    }
                } else if (left->type->is_floating()){
                    
                }
            } break;
            case TokenType::POWER: {
                if (left->type->is_integer()) {
                    is_rvalue = true;
                    auto left_value = std::any_cast<int>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(std::pow(left_value, right_value)));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(std::pow(left_value, static_cast<int>(right_value))));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(std::pow(left_value, static_cast<int>(right_value))));
                    }
                } else if (left->type->is_char()) {
                    is_rvalue = true;
                    auto left_value = std::any_cast<char>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(std::pow(left_value, right_value)));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(std::pow(left_value, static_cast<int>(right_value))));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(std::pow(left_value, static_cast<int>(right_value))));
                    }
                } else if (left->type->is_bool()) {
                    is_rvalue = true;
                    auto left_value = std::any_cast<bool>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(std::pow(left_value, right_value)));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(std::pow(left_value, static_cast<int>(right_value))));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(std::pow(left_value, static_cast<int>(right_value))));
                    }
                } else if (left->type->is_floating()){
                    is_rvalue = true;
                    auto left_value = std::any_cast<double>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(std::pow(left_value, right_value)));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(std::pow(left_value, static_cast<int>(right_value))));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(std::pow(left_value, static_cast<int>(right_value))));
                    }
                }
            } break;
            case TokenType::MODULO: {
                if (left->type->is_integer()) {
                    is_rvalue = true;
                    auto left_value = std::any_cast<int>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value % right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value % right_value));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value % right_value));
                    }
                } else if (left->type->is_char()) {
                    is_rvalue = true;
                    auto left_value = std::any_cast<char>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value % right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value % right_value));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value % right_value));
                    }
                } else if (left->type->is_bool()) {
                    is_rvalue = true;
                    auto left_value = std::any_cast<bool>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value % right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value % right_value));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value % right_value));
                    }
                }
            } break;
            case TokenType::BIT_AND: {
                if (left->type->is_integer()) {
                    is_rvalue = true;
                    auto left_value = std::any_cast<int>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value & right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value & static_cast<int>(right_value)));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value & static_cast<int>(right_value)));
                    }
                } else if (left->type->is_char()) {
                    is_rvalue = true;
                    auto left_value = std::any_cast<char>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value & right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value & static_cast<int>(right_value)));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value & static_cast<int>(right_value)));
                    }
                } else if (left->type->is_bool()) {
                    is_rvalue = true;
                    auto left_value = std::any_cast<bool>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value & right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value & static_cast<int>(right_value)));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value & static_cast<int>(right_value)));
                    }
                }
            } break;
            case TokenType::BIT_OR: {
                if (left->type->is_integer()) {
                    is_rvalue = true;
                    auto left_value = std::any_cast<int>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value | right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value | static_cast<int>(right_value)));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value | static_cast<int>(right_value)));
                    }
                } else if (left->type->is_char()) {
                    is_rvalue = true;
                    auto left_value = std::any_cast<char>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value | right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value | static_cast<int>(right_value)));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value | static_cast<int>(right_value)));
                    }
                } else if (left->type->is_bool()) {
                    is_rvalue = true;
                    auto left_value = std::any_cast<bool>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value | right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value | static_cast<int>(right_value)));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value | static_cast<int>(right_value)));
                    }
                }
            } break;
            case TokenType::BIT_XOR: {
                if (left->type->is_integer()) {
                    is_rvalue = true;
                    auto left_value = std::any_cast<int>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value ^ right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value ^ static_cast<int>(right_value)));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value ^ static_cast<int>(right_value)));
                    }
                } else if (left->type->is_char()) {
                    is_rvalue = true;
                    auto left_value = std::any_cast<char>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value ^ right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value ^ static_cast<int>(right_value)));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value ^ static_cast<int>(right_value)));
                    }
                } else if (left->type->is_bool()) {
                    is_rvalue = true;
                    auto left_value = std::any_cast<bool>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value ^ right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value ^ static_cast<int>(right_value)));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value ^ static_cast<int>(right_value)));
                    }
                }
            } break;
            case TokenType::LEFT_SHIFT: {
                if (left->type->is_integer()) {
                    is_rvalue = true;
                    auto left_value = std::any_cast<int>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value << right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value << static_cast<int>(right_value)));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value << static_cast<int>(right_value)));
                    }
                } else if (left->type->is_char()) {
                    is_rvalue = true;
                    auto left_value = std::any_cast<char>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value << right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value << static_cast<int>(right_value)));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value << static_cast<int>(right_value)));
                    }
                } else if (left->type->is_bool()) {
                    is_rvalue = true;
                    auto left_value = std::any_cast<bool>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value << right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value << static_cast<int>(right_value)));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value << static_cast<int>(right_value)));
                    }
                }
            } break;
            case TokenType::RIGHT_SHIFT: {
                if (left->type->is_integer()) {
                    is_rvalue = true;
                    auto left_value = std::any_cast<int>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value >> right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value >> static_cast<int>(right_value)));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value >> static_cast<int>(right_value)));
                    }
                } else if (left->type->is_char()) {
                    is_rvalue = true;
                    auto left_value = std::any_cast<char>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value >> right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value >> static_cast<int>(right_value)));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value >> static_cast<int>(right_value)));
                    }
                } else if (left->type->is_bool()) {
                    is_rvalue = true;
                    auto left_value = std::any_cast<bool>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value >> right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value >> static_cast<int>(right_value)));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value >> static_cast<int>(right_value)));
                    }
                }
            } break;
            case TokenType::GREATER: {
                if (left->type->is_integer()) {
                    is_rvalue = true;
                    auto left_value = std::any_cast<int>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value > right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value > right_value));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value > right_value));
                    } else if (right->type->is_floating()) {
                        auto right_value = std::any_cast<double>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value > right_value));
                    }
                } else if (left->type->is_char()) {
                    is_rvalue = true;
                    auto left_value = std::any_cast<char>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value > right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value > right_value));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value > right_value));
                    } else if (right->type->is_floating()) {
                        auto right_value = std::any_cast<double>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value > right_value));
                    }
                } else if (left->type->is_bool()) {
                    is_rvalue = true;
                    auto left_value = std::any_cast<bool>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value > right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value > right_value));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value > right_value));
                    } else if (right->type->is_floating()) {
                        auto right_value = std::any_cast<double>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value > right_value));
                    }
                } else if (left->type->is_floating()){
                    is_rvalue = true;
                    auto left_value = std::any_cast<double>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value > right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value > right_value));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value > right_value));
                    } else if (right->type->is_floating()) {
                        auto right_value = std::any_cast<double>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value > right_value));
                    }
                }
            } break;
            case TokenType::LESS: {
                if (left->type->is_integer()) {
                    is_rvalue = true;
                    auto left_value = std::any_cast<int>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value < right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value < right_value));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value < right_value));
                    } else if (right->type->is_floating()) {
                        auto right_value = std::any_cast<double>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value < right_value));
                    }
                } else if (left->type->is_char()) {
                    is_rvalue = true;
                    auto left_value = std::any_cast<char>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value < right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value < right_value));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value < right_value));
                    } else if (right->type->is_floating()) {
                        auto right_value = std::any_cast<double>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value < right_value));
                    }
                } else if (left->type->is_bool()) {
                    is_rvalue = true;
                    auto left_value = std::any_cast<bool>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value < right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value < right_value));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value < right_value));
                    } else if (right->type->is_floating()) {
                        auto right_value = std::any_cast<double>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value < right_value));
                    }
                } else if (left->type->is_floating()){
                    is_rvalue = true;
                    auto left_value = std::any_cast<double>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value < right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value < right_value));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value < right_value));
                    } else if (right->type->is_floating()) {
                        auto right_value = std::any_cast<double>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value < right_value));
                    }
                }
            } break;
            case TokenType::GREATER_EQUAL: {
                if (left->type->is_integer()) {
                    is_rvalue = true;
                    auto left_value = std::any_cast<int>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value >= right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value >= right_value));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value >= right_value));
                    } else if (right->type->is_floating()) {
                        auto right_value = std::any_cast<double>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value >= right_value));
                    }
                } else if (left->type->is_char()) {
                    is_rvalue = true;
                    auto left_value = std::any_cast<char>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value >= right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value >= right_value));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value >= right_value));
                    } else if (right->type->is_floating()) {
                        auto right_value = std::any_cast<double>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value >= right_value));
                    }
                } else if (left->type->is_bool()) {
                    is_rvalue = true;
                    auto left_value = std::any_cast<bool>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value >= right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value >= right_value));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value >= right_value));
                    } else if (right->type->is_floating()) {
                        auto right_value = std::any_cast<double>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value >= right_value));
                    }
                } else if (left->type->is_floating()){
                    is_rvalue = true;
                    auto left_value = std::any_cast<double>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value >= right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value >= right_value));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value >= right_value));
                    } else if (right->type->is_floating()) {
                        auto right_value = std::any_cast<double>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value >= right_value));
                    }
                }
            } break;
            case TokenType::LESS_EQUAL: {
                if (left->type->is_integer()) {
                    is_rvalue = true;
                    auto left_value = std::any_cast<int>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value <= right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value <= right_value));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value <= right_value));
                    } else if (right->type->is_floating()) {
                        auto right_value = std::any_cast<double>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value <= right_value));
                    }
                } else if (left->type->is_char()) {
                    is_rvalue = true;
                    auto left_value = std::any_cast<char>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value <= right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value <= right_value));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value <= right_value));
                    } else if (right->type->is_floating()) {
                        auto right_value = std::any_cast<double>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value <= right_value));
                    }
                } else if (left->type->is_bool()) {
                    is_rvalue = true;
                    auto left_value = std::any_cast<bool>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value <= right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value <= right_value));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value <= right_value));
                    } else if (right->type->is_floating()) {
                        auto right_value = std::any_cast<double>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value <= right_value));
                    }
                } else if (left->type->is_floating()){
                    is_rvalue = true;
                    auto left_value = std::any_cast<double>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value <= right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value <= right_value));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value <= right_value));
                    } else if (right->type->is_floating()) {
                        auto right_value = std::any_cast<double>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value <= right_value));
                    }
                }
            } break;
            case TokenType::EQUAL: {
                if (left->type->is_integer()) {
                    is_rvalue = true;
                    auto left_value = std::any_cast<int>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value == right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value == right_value));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value == right_value));
                    } else if (right->type->is_floating()) {
                        auto right_value = std::any_cast<double>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value == right_value));
                    }
                } else if (left->type->is_char()) {
                    is_rvalue = true;
                    auto left_value = std::any_cast<char>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value == right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value == right_value));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value == right_value));
                    } else if (right->type->is_floating()) {
                        auto right_value = std::any_cast<double>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value == right_value));
                    }
                } else if (left->type->is_bool()) {
                    is_rvalue = true;
                    auto left_value = std::any_cast<bool>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value == right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value == right_value));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value == right_value));
                    } else if (right->type->is_floating()) {
                        auto right_value = std::any_cast<double>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value == right_value));
                    }
                } else if (left->type->is_floating()){
                    is_rvalue = true;
                    auto left_value = std::any_cast<double>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value == right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value == right_value));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value == right_value));
                    } else if (right->type->is_floating()) {
                        auto right_value = std::any_cast<double>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value == right_value));
                    }
                }
            } break;
            case TokenType::NOT_EQUAL: {
                if (left->type->is_integer()) {
                    is_rvalue = true;
                    auto left_value = std::any_cast<int>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value != right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value != right_value));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value != right_value));
                    } else if (right->type->is_floating()) {
                        auto right_value = std::any_cast<double>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value != right_value));
                    }
                } else if (left->type->is_char()) {
                    is_rvalue = true;
                    auto left_value = std::any_cast<char>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value != right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value != right_value));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value != right_value));
                    } else if (right->type->is_floating()) {
                        auto right_value = std::any_cast<double>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value != right_value));
                    }
                } else if (left->type->is_bool()) {
                    is_rvalue = true;
                    auto left_value = std::any_cast<bool>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value != right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value != right_value));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value != right_value));
                    } else if (right->type->is_floating()) {
                        auto right_value = std::any_cast<double>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value != right_value));
                    }
                } else if (left->type->is_floating()){
                    is_rvalue = true;
                    auto left_value = std::any_cast<double>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value != right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value != right_value));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value != right_value));
                    } else if (right->type->is_floating()) {
                        auto right_value = std::any_cast<double>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value != right_value));
                    }
                }
            } break;
            case TokenType::AND: {
                if (left->type->is_integer()) {
                    is_rvalue = true;
                    auto left_value = std::any_cast<int>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value && right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value && right_value));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value && right_value));
                    } else if (right->type->is_floating()) {
                        auto right_value = std::any_cast<double>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value && right_value));
                    }
                } else if (left->type->is_char()) {
                    is_rvalue = true;
                    auto left_value = std::any_cast<char>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value && right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value && right_value));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value && right_value));
                    } else if (right->type->is_floating()) {
                        auto right_value = std::any_cast<double>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value && right_value));
                    }
                } else if (left->type->is_bool()) {
                    is_rvalue = true;
                    auto left_value = std::any_cast<bool>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value && right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value && right_value));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value && right_value));
                    } else if (right->type->is_floating()) {
                        auto right_value = std::any_cast<double>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value && right_value));
                    }
                } else if (left->type->is_floating()){
                    is_rvalue = true;
                    auto left_value = std::any_cast<double>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value && right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value && right_value));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value && right_value));
                    } else if (right->type->is_floating()) {
                        auto right_value = std::any_cast<double>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value && right_value));
                    }
                }
            } break;
            case TokenType::OR: {
                if (left->type->is_integer()) {
                    is_rvalue = true;
                    auto left_value = std::any_cast<int>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value || right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value || right_value));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value || right_value));
                    } else if (right->type->is_floating()) {
                        auto right_value = std::any_cast<double>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value || right_value));
                    }
                } else if (left->type->is_char()) {
                    is_rvalue = true;
                    auto left_value = std::any_cast<char>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value || right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value || right_value));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value || right_value));
                    } else if (right->type->is_floating()) {
                        auto right_value = std::any_cast<double>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value || right_value));
                    }
                } else if (left->type->is_bool()) {
                    is_rvalue = true;
                    auto left_value = std::any_cast<bool>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value || right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value || right_value));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value || right_value));
                    } else if (right->type->is_floating()) {
                        auto right_value = std::any_cast<double>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value || right_value));
                    }
                } else if (left->type->is_floating()){
                    is_rvalue = true;
                    auto left_value = std::any_cast<double>(left->value);
                    if (right->type->is_integer()) {
                        auto right_value = std::any_cast<int>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value || right_value));
                    } else if (right->type->is_char()) {
                        auto right_value = std::any_cast<char>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value || right_value));
                    } else if (right->type->is_bool()) {
                        auto right_value = std::any_cast<bool>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value || right_value));
                    } else if (right->type->is_floating()) {
                        auto right_value = std::any_cast<double>(right->value);
                        return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(left_value || right_value));
                    }
                }
            } break;
        }
    }
}

std::shared_ptr<VarSymbol> Executor::assignment_operation(std::shared_ptr<VarSymbol> left, Token& op, std::shared_ptr<VarSymbol> right) {
    if (is_record_type(left->type) || is_record_type(right->type)) {
        // TODO
    } else {
        if (is_rvalue) {
            throw std::runtime_error("Cannot assign to an rvalue");
        }
        if (getTypeRank(left->type) != getTypeRank(right->type)) {
            if (left->type->is_integer() && right->type->is_floating()) {
                auto right_value = std::any_cast<double>(right->value);
                left->value = static_cast<int>(right_value);
            } else if (left->type->is_floating() && right->type->is_integer()) {
                auto right_value = std::any_cast<int>(right->value);
                left->value = static_cast<double>(right_value);
            } else if (left->type->is_char() && right->type->is_integer()) {
                auto right_value = std::any_cast<int>(right->value);
                left->value = static_cast<char>(right_value);
            } else if (left->type->is_bool() && right->type->is_integer()) {
                auto right_value = std::any_cast<int>(right->value);
                left->value = static_cast<bool>(right_value);
            }
        } else {
            left->value = right->value;
        }
    }
    return left;
}

std::shared_ptr<VarSymbol> Executor::unary_operation(std::shared_ptr<VarSymbol> left, Token& op) {
    if (is_record_type(left->type)) {
        
    } else {
        if (left->type->is_integer()) {
            is_rvalue = true;
            auto left_value = std::any_cast<int>(left->value);
            switch (op.type) {
                case TokenType::PLUS: {
                    return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value));
                }
                case TokenType::MINUS: {
                    return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(-left_value));
                }
                case TokenType::BIT_NOT: {
                    return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(~left_value));
                }
                case TokenType::NOT: {
                    return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(!left_value));
                } case TokenType::INCREMENT: {
                    left->value = left_value + 1;
                    return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value + 1));
                }
                case TokenType::DECREMENT: {
                    left->value = left_value - 1;
                    return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value + 1));
                }
            }
        } else if (left->type->is_char()) {
            auto left_value = std::any_cast<char>(left->value);
            switch (op.type) {
                case TokenType::PLUS: {
                    return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value));
                }
                case TokenType::MINUS: {
                    return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(-left_value));
                }
                case TokenType::BIT_NOT: {
                    return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(~left_value));
                }
                case TokenType::NOT: {
                    return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(!left_value));
                } case TokenType::INCREMENT: {
                    left->value = left_value + 1;
                    return std::make_shared<VarSymbol>(std::make_shared<CharType>(), std::make_any<char>(left_value + 1));
                }
                case TokenType::DECREMENT: {
                    left->value = left_value - 1;
                    return std::make_shared<VarSymbol>(std::make_shared<CharType>(), std::make_any<char>(left_value + 1));
                }
            }
        } else if (left->type->is_bool()) {
            auto left_value = std::any_cast<bool>(left->value);
            switch (op.type) {
                case TokenType::PLUS: {
                    return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value));
                }
                case TokenType::MINUS: {
                    return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(-left_value));
                }
                case TokenType::BIT_NOT: {
                    return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(~left_value));
                }
                case TokenType::NOT: {
                    return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(!left_value));
                } case TokenType::INCREMENT: {
                    left->value = left_value + 1;
                    return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value + 1));
                }
                case TokenType::DECREMENT: {
                    left->value = left_value - 1;
                    return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value + 1));
                }
            }
        } else if (left->type->is_floating()) {
            auto left_value = std::any_cast<double>(left->value);
            switch (op.type) {
                case TokenType::PLUS: {
                    return std::make_shared<VarSymbol>(std::make_shared<FloatType>(), std::make_any<double>(left_value));
                }
                case TokenType::MINUS: {
                    return std::make_shared<VarSymbol>(std::make_shared<FloatType>(), std::make_any<double>(-left_value));
                }
                case TokenType::NOT: {
                    return std::make_shared<VarSymbol>(std::make_shared<BoolType>(), std::make_any<bool>(!left_value));
                } case TokenType::INCREMENT: {
                    left->value = left_value + 1;
                    return std::make_shared<VarSymbol>(std::make_shared<FloatType>(), std::make_any<double>(left_value + 1));
                }
                case TokenType::DECREMENT: {
                    left->value = left_value - 1;
                    return std::make_shared<VarSymbol>(std::make_shared<FloatType>(), std::make_any<double>(left_value + 1));
                }
            }
        }
    }
}

std::shared_ptr<VarSymbol> Executor::postgix_operation(std::shared_ptr<VarSymbol> left, Token& op){
    if (is_record_type(left->type)) {
        
    } else {
        if (left->type->is_integer()) {
            is_rvalue = false;
            auto left_value = std::any_cast<int>(left->value);
            switch (op.type) {
                case TokenType::INCREMENT: {
                    left->value = left_value + 1;
                    return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value));
                }
                case TokenType::DECREMENT: {
                    left->value = left_value - 1;
                    return std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), std::make_any<int>(left_value));
                }
            }
        } else if (left->type->is_char()) {
            is_rvalue = false;
            auto left_value = std::any_cast<char>(left->value);
            switch (op.type) {
                case TokenType::INCREMENT: {
                    left->value = left_value + 1;
                    return std::make_shared<VarSymbol>(std::make_shared<CharType>(), std::make_any<char>(left_value));
                }
                case TokenType::DECREMENT: {
                    left->value = left_value - 1;
                    return std::make_shared<VarSymbol>(std::make_shared<CharType>(), std::make_any<char>(left_value));
                }
            }
        }else if (left->type->is_floating()) {
            is_rvalue = false;
            auto left_value = std::any_cast<double>(left->value);
            switch (op.type) {
                case TokenType::INCREMENT: {
                    left->value = left_value + 1;
                    return std::make_shared<VarSymbol>(std::make_shared<FloatType>(), std::make_any<double>(left_value));
                }
                case TokenType::DECREMENT: {
                    left->value = left_value - 1;
                    return std::make_shared<VarSymbol>(std::make_shared<FloatType>(), std::make_any<double>(left_value));
                }
            }
        }
    }
}   

bool Executor::can_convert(const std::shared_ptr<Type>& from, const std::shared_ptr<Type>& to) {
    if (from == to) return true;

    if (dynamic_cast<Arithmetic*>(from.get()) && dynamic_cast<Arithmetic*>(to.get())) {
        return true;
    }
    return false;
}

