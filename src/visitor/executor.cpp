#include "executor.hpp"
#include <algorithm>
#include "type.hpp"

Executor::Executor() : symbolTable(std::make_shared<Scope>()) {};

std::unordered_map<std::string, std::shared_ptr<Symbol>> Executor::default_types = {
    {"int", std::make_shared<VarSymbol>()},
    {"double", std::make_shared<VarSymbol>()},
    {"float", std::make_shared<VarSymbol>()},
    {"char", std::make_shared<VarSymbol>()},
    {"bool", std::make_shared<VarSymbol>()},
    {"void", std::make_shared<VarSymbol>()}
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
        auto value = current_value;
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
            symbolTable->push_symbol(name, std::make_shared<VarSymbol>(current_value->type));
        } break;
        case IDDeclaratorType::POINTER : {
            symbolTable->push_symbol(name, std::make_shared<VarSymbol>(std::make_shared<PointerType>(current_value->type)));
        } break;
        case IDDeclaratorType::REF : {
            
        } break;
        case IDDeclaratorType::ARRAY : {
            symbolTable->push_symbol(name, std::make_shared<VarSymbol>(std::make_shared<ArrayType>(current_value->type)));
        } break;
    }
}

void Executor::visit(FuncDeclarator* node) {
    auto returnable_type_token = node->get_returnable_type();
    auto name = node->get_name().value;
    const auto& block = node->get_block();
    auto default_type = match_symbol(returnable_type_token);
    const auto& args = node->get_params();
    const auto& block = node->get_block();
    std::vector<std::shared_ptr<Type>> type_args;
    symbolTable = symbolTable->create_new_table(symbolTable);
    for (const auto& i : args){
        i->accept(*this);
        type_args.push_back(current_value->type);
        symbolTable->push_symbol(i->get_type().value, std::make_shared<VarSymbol>(current_value->type));
    }
    auto func = std::make_shared<FuncType>(default_type, type_args, block);
    symbolTable->push_symbol(name, std::make_shared<FuncSymbol>(func));
    block->accept(*this); // заходим в наш блок
    symbolTable->push_symbol(name, std::make_shared<FuncSymbol>(func));
}

void Executor::visit(ParamDeclarator* node) {
    // TODO
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
    std::unordered_map<std::string, std::shared_ptr<Type>> struct_vars;std::unordered_map<std::string, std::shared_ptr<Type>> struct_vars;
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
    auto left_value = current_value;
    right->accept(*this);
    auto right_value = current_value;
    auto left_type = dynamic_cast<VarSymbol*>(left_value.get())->type;
    auto right_type = dynamic_cast<VarSymbol*>(right_value.get())->type;
    if (is_record_type(left_type) || is_record_type(right_type)) {
        // TODO
    }
    auto left_any_val = dynamic_cast<VarSymbol*>(left_value.get())->value;
    auto right_any_val = dynamic_cast<VarSymbol*>(right_value.get())->value;
    auto counted = count_bool(left_any_val, op, right_any_val);
    current_value = std::make_shared<VarSymbol>(std::make_shared<BoolType>(), counted);
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
    auto left_value = current_value;
    auto left_any_val = dynamic_cast<VarSymbol*>(left_value.get())->value;
    right->accept(*this);
    auto right_value = current_value;
    auto right_any_val = dynamic_cast<VarSymbol*>(right_value.get())->value;
    current_value = std::make_shared<VarSymbol>(binary_operation(left_any_val, op, right_any_val));
}

void Executor::visit(UnaryExpression* node) {// ++ -- (int) 
    const auto& base = node->get_base();
    auto op = node->get_op();
    base->accept(*this);
    auto base_value = current_value;
    current_value = std::make_shared<VarSymbol>(unary_operation(base_value, op));
}

void Executor::visit(PostfixExpression* node){
    const auto& base = node->get_expression();
    auto op = node->get_op();
    base->accept(*this);
    auto base_value = current_value;
    current_value = std::make_shared<VarSymbol>(postgix_operation(base_value, op));
}

void Executor::visit(SubscriptExpression* node) { //[]
    const auto& expression = node->get_expression();
    auto indexes = node->get_indexes();
    auto op = node->get_op();
    expression->accept(*this);
    auto expression_value = current_value;
    if (expression_value->is_record()) {
        
    }
    for (const auto& index : indexes) {
        index->accept(*this);
        auto index_value = current_value;
    }
}
void Executor::visit(CallExpression* node) {
    const auto& expression = node->get_expression();
    auto args = node->get_args();
    auto op = node->get_op();
    expression->accept(*this);
    auto expression_value = current_value;
    if (expression_value->is_record()) {
        // TODO
    }
    std::vector<int> func_ranks(matched_functions.size(), 0);
    for (auto function_it = matched_functions.begin(); function_it != matched_functions.end(); ++function_it) {
        auto function = *function_it;
        auto func_args = function->get_args();
        if (func_args.size() != args.size()) {
            throw std::runtime_error("hello there");
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
    if (matched_functions.empty()) {
        throw std::runtime_error("No matching function found for call expr");
    }
    auto index = *std::max_element(func_ranks.begin(), func_ranks.end());
    auto best_match = matched_functions[index];
    symbolTable = symbolTable->create_new_table(symbolTable);
    const auto& params_names = best_match->get_func_declarator()->get_params();
    for (int i = 0; i < params_names.size(); ++i) {
        const auto& param = params_names[i];
        auto param_name = param->get_declorator()->get_declarator()->get_id().value;
        args[i]->accept(*this);
        symbolTable->push_symbol(param_name, current_value);
    }
    matched_functions.clear();
    best_match->get_func_declarator()->get_block()->accept(*this);
};
void Executor::visit(AccessExpression* node) {
    const auto& expression = node->get_expression();    
    auto member = node->get_member();

};
void Executor::visit(LiteralNumExpression* node) {
    auto value = node->get_value();
    current_value = std::make_shared<VarSymbol>(std::make_shared<IntegerType>(), value);
};
void Executor::visit(LiteralFloatExpression* node) {
    auto value = node->get_value();
    current_value = std::make_shared<VarSymbol>(std::make_shared<FloatType>(), value);
};
void Executor::visit(LiteralCharExpression* node) {
    auto value = node->get_value();
    current_value = std::make_shared<VarSymbol>(std::make_shared<CharType>(), value);
};
void Executor::visit(LiteralStringExpression* node) {
    auto value = node->get_value();
    current_value = std::make_shared<VarSymbol>(std::make_shared<StringType>(), value);
};
void Executor::visit(IDexpression* node) {
    auto token = node->get_token();
    current_value = match_symbol(token);
};
void Executor::visit(GroupExpression* node) {
    const auto& base = node->get_base();
    base->accept(*this);
};


