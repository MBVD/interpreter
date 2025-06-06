#include "analyzer.hpp"
#include "token.hpp"
#include "type.hpp"
#include <memory>
#include <algorithm>

std::unordered_map<std::string, std::shared_ptr<Type>> Analyzer::default_types = {
    {"int", std::make_shared<IntegerType>()},
    {"double", std::make_shared<FloatType>()},
    {"float", std::make_shared<FloatType>()},
    {"char", std::make_shared<CharType>()}
};

std::unordered_map<std::string, std::function<std::shared_ptr<Type>(std::vector<std::shared_ptr<Type>>)>> Analyzer::libary_functions = {
    {"printf", [](std::vector<std::shared_ptr<Type>> args) -> std::shared_ptr<Type> {
        if (args.size() < 1 || !args[0]->is_array() || !dynamic_pointer_cast<ArrayType>(args[0])->get_base()->is_char()) {
            throw std::runtime_error("printf requires at least one argument of type string.");
        }
        auto str = std::dynamic_pointer_cast<ArrayType>(args[0]);
        std::string output;
        auto elements = std::dynamic_pointer_cast<ArrayType>(str)->get_elements();
        for (auto i = 1; i < args.size(); ++i) {
            if (args[i]->is_char()) {
                continue;
            } else if (args[i]->is_integer()) {
                continue;
            } else if (args[i]->is_floating()) {
                continue;
            } else {
                throw std::runtime_error("Unsupported type for printf.");
            }
        }
        return std::make_shared<VoidType>(); // Return void type
    }}
};

Analyzer::Analyzer() : scope(std::make_shared<Scope>(nullptr)) {}

bool Analyzer::can_convert(const std::shared_ptr<Type>& from, const std::shared_ptr<Type>& to) {
    if (from == to) return true;

    if (dynamic_cast<Arithmetic*>(from.get()) && dynamic_cast<Arithmetic*>(to.get())) {
        return true;
    }
    return false;
}

void Analyzer::analyze(TranslationUnit & unit){
    for (const auto& i : unit.get_nodes()){
        i->accept(*this);
    }
}

void Analyzer::visit(ASTNode* node){
    node->accept(*this);
}

void Analyzer::visit(Declarator* node){
    node->accept(*this);
}

void Analyzer::visit(VarDeclarator* node){
    auto type = node->get_type();
    const auto& declarations = node->get_init_declarators();
    for (const auto& init_declorator : declarations){
        current_type = get_type(type);
        init_declorator->accept(*this);
    }
}

void Analyzer::visit(InitDeclarator* node) {
    const auto& id_declarator = node->get_declarator();
    const auto& expression = node->get_expression();
    if (expression != nullptr){
        id_declarator->accept(*this);
        auto id_decl_type = current_type;
        expression->accept(*this);
        auto expression_type = current_type;
        if (can_convert(expression_type, id_decl_type)) {
            current_type = id_decl_type;
        } else {
            throw std::runtime_error("not tknown conv");
        }
        // проходимся по expression и проверяем является ли он типом который может конвертироваться в нашу структуру
        // TODO нужно сделать метод который ищет какой scope для нашей структуры и там находить func - конструктор с таким параметром
        // if (typeid(current_type) == typeid(struct_type)) // пока так, потом будем проверять есть ли конструктор
    } else {
        id_declarator->accept(*this);
    }      
}

void Analyzer::visit(IdDeclorator* node){
    auto name = node->get_id().value;
    auto type = node->get_declorator_type();
    if (scope->contains_symbol(name)) {
        throw std::runtime_error("Symbol '" + name + "' already exists in scope.");
    }
    switch(type){
        case IDDeclaratorType::NONE : {

        } break;
        case IDDeclaratorType::POINTER : {
            current_type = std::make_shared<PointerType>(current_type);
        } break;
        case IDDeclaratorType::ARRAY : {
            auto array_element_type = current_type;
            const auto& expression = node->get_expression();
            expression->accept(*this);
            if (!current_type->is_integer()){
                throw std::runtime_error("Array size must be an integer type.");
            }
            std::vector<std::shared_ptr<Symbol>> array_elements;
            auto array_element = std::make_shared<VarSymbol>(array_element_type);
            array_elements.push_back(array_element);
            current_type = std::make_shared<ArrayType>(array_element_type, array_elements);
        } break;
        case IDDeclaratorType::REF : {
            // создать тип данных сссылка
        } break;
    }
    scope->push_symbol(name, std::make_shared<VarSymbol>(current_type));
}

void Analyzer::visit(FuncDeclarator* node){
    auto returnable_type_token = node->get_returnable_type();
    auto name = node->get_name().value;
    const auto& block = node->get_block();
    auto default_type = get_type(returnable_type_token);
    const auto& args = node->get_params();
    std::vector<std::shared_ptr<Type>> type_args;
    scope = scope->create_new_table(scope);
    for (const auto& i : args){
        i->accept(*this);
        type_args.push_back(current_type);
        scope->push_symbol(i->get_type().value, std::make_shared<VarSymbol>(current_type));
    }
    auto func = std::make_shared<FuncType>(default_type, type_args);
    scope->push_symbol(name, std::make_shared<FuncSymbol>(func, node));
    block->accept(*this); // заходим в наш блок
    scope->push_symbol(name, std::make_shared<FuncSymbol>(func, node));
    current_type = func;
}

void Analyzer::visit(ParamDeclarator* node) {
    auto type = node->get_type();
    const auto& init_declarator = node->get_declorator();
    current_type = get_type(type);
    init_declarator->accept(*this);
}

void Analyzer::visit(StructDeclarator* node) {
    auto id = node->get_id();
    const auto& vars = node->get_vars();
    scope = scope->create_new_table(scope);
    for (const auto& var : vars){
        var->accept(*this);
        auto var_type = current_type;
        var_type = get_type(var->get_type());
    }
    auto scope_multi_vars = scope->get_symbols();
    std::unordered_map<std::string, std::shared_ptr<Type>> struct_vars;
    std::unordered_map<std::string, std::shared_ptr<Symbol>> scope_vars;
    for (auto var : scope_multi_vars) {
        struct_vars[var.first] = var.second->type;
        scope_vars[var.first] = var.second;
    }
    scope = scope->get_prev_table();
    auto struc = std::make_shared<StructType>(struct_vars);
    auto struc_symbol = std::make_shared<Record>(struc, scope_vars);
    scope->push_symbol(id.value, struc_symbol);
    current_type = struc;
}

void Analyzer::visit(Expression* node) {
    node->accept(*this);
}

void Analyzer::visit(ComparisonExpression* node){
    const auto& left = node->get_left();
    const auto& right = node->get_right();
    auto op = node->get_op();
    left->accept(*this);
    auto left_type = current_type;
    right->accept(*this);
    auto right_type = current_type;
    if (dynamic_cast<StructType*>(left_type.get())){
        // найти оператор сравнения для него 
        // либо конвертацию
    }
    if (dynamic_cast<StructType*>(right_type.get())){
        // 
    }

    if (dynamic_cast<Arithmetic*>(left_type.get()) && dynamic_cast<Arithmetic*>(right_type.get())) {
        current_type = std::make_shared<BoolType>();
    } else {
        throw std::runtime_error("error ");
    }
}

void Analyzer::visit(TernaryExpression* node) {
    const auto& cond_expr = node->get_cond_expression();
    const auto& true_expr = node->get_true_expression();
    const auto& false_expr = node->get_false_expression();
    cond_expr->accept(*this);
    auto cond_type = current_type;
    true_expr->accept(*this);
    auto true_expr_type = current_type;
    false_expr->accept(*this);   
    auto false_expr_type = current_type;
    if (!dynamic_cast<BoolType*>(cond_type.get())) {
        throw std::runtime_error("error in checking cond expression");
    }

    if (dynamic_cast<Composite*>(true_expr_type.get())) {
        // 
    }
    if (dynamic_cast<Composite*>(false_expr_type.get())){
        //
    }

    if(can_convert(true_expr_type, false_expr_type)) { // значит тип правый конвертируется в левый тип
        current_type = true_expr_type;
    }
    throw std::runtime_error("not valid ternary expression for this types");
}

void Analyzer::visit(LogicalOrExpression* node){
    const auto& left = node->get_left();
    const auto& right = node->get_right();
    left->accept(*this);
    auto left_type = current_type;
    right->accept(*this);
    auto right_type = current_type;

    if (can_convert(std::make_shared<BoolType>(), left_type) && can_convert(std::make_shared<BoolType>(), right_type)) {
        current_type = std::make_shared<BoolType>();
        return;
    }
    throw std::runtime_error("not valid logical or expression for this types");
}

void Analyzer::visit(LogicalAndExpression* node) {
    const auto& left = node->get_left();
    const auto& right = node->get_right();
    left->accept(*this);
    auto left_type = current_type;
    right->accept(*this);
    auto right_type = current_type;

    if (can_convert(std::make_shared<BoolType>(), left_type) && can_convert(std::make_shared<BoolType>(), right_type)) {
        current_type = std::make_shared<BoolType>();
        return;
    }
    throw std::runtime_error("not valid logical and expression for this types");
}

void Analyzer::visit(BiteIncOrExpression* node) {
    const auto& left = node->get_left();
    const auto& right = node->get_right();
    left->accept(*this);
    auto left_type = current_type;
    right->accept(*this);
    auto right_type = current_type;

    if (dynamic_cast<StructType*>(left_type.get())) {
        // найти оператор побитового или для него
        // либо конвертацию
    }

    if (dynamic_cast<StructType*>(right_type.get())) {
        // 
    }

    if (dynamic_cast<Integral*>(left_type.get()) && dynamic_cast<Integral*>(right_type.get())) {
        current_type = left_type;
        return;
    }
    throw std::runtime_error("not valid bitwise or expression for this types");
}

void Analyzer::visit(BiteExcOrExpression* node) {
    const auto& left = node->get_left();
    const auto& right = node->get_right();
    left->accept(*this);
    auto left_type = current_type;
    right->accept(*this);
    auto right_type = current_type;

    if (dynamic_cast<Integral*>(left_type.get()) && dynamic_cast<Integral*>(right_type.get())) {
        current_type = left_type;
        return;
    }
    throw std::runtime_error("not valid bitwise xor expression for this types");
}

void Analyzer::visit(BiteAndExpression* node) {
    const auto& left = node->get_left();
    const auto& right = node->get_right();
    left->accept(*this);
    auto left_type = current_type;
    right->accept(*this);
    auto right_type = current_type;

    if (dynamic_cast<StructType*>(left_type.get())) {
        // найти оператор побитового и для него
        // либо конвертацию
    }
    if (dynamic_cast<StructType*>(right_type.get())) {
        // 
    }

    if (dynamic_cast<Integral*>(left_type.get()) && dynamic_cast<Integral*>(right_type.get())) {
        current_type = left_type;
        return;
    }
    throw std::runtime_error("not valid bitwise and expression for this types");
}

void Analyzer::visit(ShiftExpression* node) {
    const auto& left = node->get_left();
    const auto& right = node->get_right();
    auto op = node->get_op();
    left->accept(*this);
    auto left_type = current_type;
    right->accept(*this);
    auto right_type = current_type;

    if (dynamic_cast<StructType*>(left_type.get())) {
        // найти оператор сдвига для него
        // либо конвертацию
    }
    if (dynamic_cast<StructType*>(right_type.get())) {
        // 
    }

    if (dynamic_cast<Integral*>(left_type.get()) && dynamic_cast<Integral*>(right_type.get())) {
        current_type = left_type;
        return;
    }
    throw std::runtime_error("not valid shift expression for this types");
}

void Analyzer::visit(BinaryExpression* node) {
    const auto& left = node->get_left();
    const auto& right = node->get_right();
    auto op = node->get_op();
    left->accept(*this);
    auto left_type = current_type;
    right->accept(*this);
    auto right_type = current_type;

    if (dynamic_cast<Composite*>(left_type.get())) {
        //
    }
    if (dynamic_cast<Composite*>(right_type.get())) {
        //
    }
    if (dynamic_cast<Arithmetic*>(left_type.get()) && dynamic_cast<Arithmetic*>(right_type.get())) {
        current_type = compare_types(left_type, right_type);
        return;
    }
    throw std::runtime_error("not valid binary expression for this types");
}

void Analyzer::visit(CommaExpression* node) {
    const auto& left = node->get_left();
    const auto& right = node->get_right();
    left->accept(*this);
    right->accept(*this);
}

void Analyzer::visit(AssignmentExpression* node) {
    const auto& left = node->get_left();
    const auto& right = node->get_right();
    auto op = node->get_op();
    left->accept(*this);
    auto left_type = current_type;
    right->accept(*this);
    auto right_type = current_type;

    if (dynamic_cast<StructType*>(left_type.get())) {
        // найти оператор присваивания для него
        // либо конвертацию
    }
    if (dynamic_cast<StructType*>(right_type.get())) {
        // 
    }

    if (can_convert(right_type, left_type)) { // значит тип правый конвертируется в левый тип
        current_type = left_type;
        return;
    }
    throw std::runtime_error("not valid assignment expression for this types");
}

void Analyzer::visit(UnaryExpression* node) {// ++ -- (int) 
    const auto& base = node->get_base();
    auto op = node->get_op();
    base->accept(*this);
    auto base_type = current_type;
    switch(op.type) {
        case TokenType::INCREMENT : {
            if (dynamic_cast<Integral*>(base_type.get())) {
                current_type = base_type;
                return;
            } else {
                throw std::runtime_error("not valid ++ operator for this type");
            }
        } break;
        case TokenType::DECREMENT : {
            if (dynamic_cast<Integral*>(base_type.get())) {
                current_type = base_type;
                return;
            } else {
                throw std::runtime_error("not valid -- operator for this type");
            }
        } break;
        case TokenType::PLUS : {
            if (dynamic_cast<Arithmetic*>(base_type.get())) {
                current_type = base_type;
                return;
            } else {
                throw std::runtime_error("not valid + operator for this type");
            }
        } break;
        case TokenType::MINUS : {
            if (dynamic_cast<Arithmetic*>(base_type.get())) {
                current_type = base_type;
            } else {
                throw std::runtime_error("not valid - operator for this type");
            }
        } break;
        case TokenType::TYPE : {
            if (can_convert(base_type, get_type(op))) {
                current_type = default_types.at(op.value);
                return;
            } else {
                throw std::runtime_error("not valid cast to " + op.value);
            }
        }
        case TokenType::BIT_NOT : {
            if (!dynamic_cast<Integral*>(base_type.get())) {
                throw std::runtime_error("now valid ~ operator for this type");
            }
        } break;
    }
}

void Analyzer::visit(PostfixExpression* node) {
    const auto& expression = node-> get_expression();
    auto op = node->get_op(); // ну тут может быть только ++ или --
    expression->accept(*this);
    auto expression_type = current_type;
    if (!dynamic_cast<Arithmetic*>(expression_type.get())) {
        throw std::runtime_error("not valid " + op.value);
    }
}

void Analyzer::visit(SubscriptExpression* node) {// []
    const auto& expression = node->get_expression();
    const auto& indexes = node->get_indexes();
    auto op = node->get_op();
    expression->accept(*this);  
    auto expression_type = current_type;
    if (dynamic_cast<StructType*>(expression_type.get())) {
        // ищем оператор []
    }
    auto* pointer = dynamic_cast<PointerType*>(expression_type.get());
    if (!pointer) {
        throw std::runtime_error("cant index non-pointer type");    
    }
    for (const auto& i : indexes){
        i->accept(*this);
        if (!dynamic_cast<IntegerType*>(current_type.get())){
            throw std::runtime_error("cant get access to pointer with non-integer index");
        }
    }
    if (indexes.size() > pointer->get_star_count()) {
        std::cout<<indexes.size()<<" "<<pointer->get_star_count()<<"\n";
        throw std::runtime_error("cant index pointer with more indexes than stars");
    }
    current_type = pointer->get_type_by_star_count(indexes.size());
}

void Analyzer::visit(CallExpression* node) { // ()
    const auto& expression = node->get_expression();
    auto op = node->get_op();
    const auto& args = node->get_args();
    auto id_expr = dynamic_cast<IDexpression*>(expression.get());
    if (id_expr && libary_functions.contains(id_expr->get_token().value)) {
        auto func = libary_functions[id_expr->get_token().value];
        std::vector<std::shared_ptr<Type>> func_args;
        for (const auto& arg : args) {
            arg->accept(*this);
            func_args.push_back(current_type);
        }
        current_type = func(func_args);
        return;
    }
    expression->accept(*this);
    auto expression_type = current_type;
    if (dynamic_cast<StructType*>(expression_type.get())) {
        //
    }
    auto* expression_func = dynamic_cast<FuncType*>(expression_type.get());
    if (!expression_func) {
        throw std::runtime_error("hello there");
    }
    auto expr_func_args = expression_func->get_args();
    /* поиск лучшей фукнции*/
    std::vector<int> func_ranks(matched_functions.size(), 0);
    for (auto function_it = matched_functions.begin(); function_it != matched_functions.end(); ++function_it) {
        auto function = *function_it;
        auto func_args = function->get_args();
        if (func_args.size() != args.size()) {
            throw std::runtime_error("hello there");
        }
        for (int i = 0; i < args.size(); ++i) {
            args[i]->accept(*this);
            auto arg_type = current_type;
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
    matched_functions.clear();
    current_type = best_match->get_returnable_type();
}

void Analyzer::visit(AccessExpression* node) { // ->
    const auto& expression = node->get_expression();
    auto member_token = node->get_member();

    expression->accept(*this);
    auto expression_type = current_type;

    if (auto pointer_type = dynamic_cast<PointerType*>(expression_type.get())) {
        auto base_type = pointer_type->get_base();
        if (!base_type) {
            throw std::runtime_error("Error: Pointer dereference of incomplete type.");
        }

        if (auto struct_type = dynamic_cast<StructType*>(base_type.get())) {
            auto members = struct_type->get_members();
            auto member_name = member_token.value;

            if (members.find(member_name) != members.end()) {
                current_type = members.at(member_name);
            } else {
                throw std::runtime_error("Error: Member '" + member_name + "' not found in struct.");
            }
        } else {
            throw std::runtime_error("Error: Member access on non-struct type.");
        }
    } else if (auto struct_type = dynamic_cast<StructType*>(expression_type.get())) {
        auto members = struct_type->get_members();
        auto member_name = member_token.value;

        if (members.find(member_name) != members.end()) {
            current_type = members.at(member_name);
        } else {
            throw std::runtime_error("Error: Member '" + member_name + "' not found in struct.");
        }
    }
    else {
        throw std::runtime_error("Error: Member access on non-struct type.");
    }
}

void Analyzer::visit(LiteralNumExpression* node) {
    current_type = std::make_shared<IntegerType>();
}

void Analyzer::visit(LiteralFloatExpression* node) {
    current_type = std::make_shared<FloatType>();
}

void Analyzer::visit(LiteralCharExpression* node) {
    current_type = std::make_shared<CharType>();
}

void Analyzer::visit(LiteralStringExpression* node) {
    auto value = node->get_value();
    auto char_type = std::make_shared<CharType>();
    std::vector<std::shared_ptr<Symbol>> char_symbols;
    for (auto i = 0; i < value.size(); ++i) {
        auto symbol = value[i];
        auto char_symbol = std::make_shared<VarSymbol>(char_type, std::make_any<char>(symbol));
        char_symbols.push_back(char_symbol);
    }
    current_type = std::make_shared<ArrayType>(char_type, char_symbols);
}

void Analyzer::visit(IDexpression* node) {
    current_type = scope->match_global(node->get_token().value)->type;
    if (dynamic_cast<FuncType*>(current_type.get())) {
        auto functions = scope->match_range(node->get_token().value);
        for (auto function : functions) {
            auto func_type = std::dynamic_pointer_cast<FuncType>(function->type);
            if (func_type) {
                matched_functions.push_back(func_type);
            }
        }
    }
}

void Analyzer::visit(GroupExpression* node) {
    node->get_base()->accept(*this);
}

void Analyzer::visit(Statement* node) {
    node->accept(*this);
}

void Analyzer::visit(BlockStatement* node) {
    for (const auto& s : node->get_statements())
        s->accept(*this);
    scope = scope->get_prev_table();
}

void Analyzer::visit(DeclarationStatement* node) {
    node->get_declaration()->accept(*this);
}

void Analyzer::visit(ExpressionStatement* node) {
    node->get_expression()->accept(*this);            
}

void Analyzer::visit(ConditionalStatement* node) {
    node->get_conditional()->accept(*this);
    node->get_true_statement()->accept(*this);
    if (node->get_false_statement())
        node->get_false_statement()->accept(*this);
}

void Analyzer::visit(WhileStatement* node) {
    node->get_conditional()->accept(*this);
    node->get_statement()->accept(*this);
}

void Analyzer::visit(ForStatement* node) {
    if (node->get_init_expr())
        node->get_init_expr()->accept(*this);

    if (node->get_cond_expr())
        node->get_cond_expr()->accept(*this);

    if (node->get_iter_expr())
        node->get_iter_expr()->accept(*this);
        
    node->get_statement()->accept(*this);
}

void Analyzer::visit(ReturnStatement* node) {
    if (node->get_expression())
        node->get_expression()->accept(*this);
}

void Analyzer::visit(BreakStatement* node) {
    node->accept(*this);
}

void Analyzer::visit(ContinueStatement* node) {
    node->accept(*this);
}

void Analyzer::visit(LoopStatement* node) {
    node->accept(*this);
}

void Analyzer::visit(DoWhileStatement* node) {
    node->get_statement()->accept(*this);
    node->get_expression()->accept(*this);
}

void Analyzer::visit(EmptyStatement* node) {
    node->accept(*this);
}


std::shared_ptr<Type> Analyzer::get_type(Token token){
    if (token == TokenType::ID){
        return scope->match_global(token.value)->type;
    } else {
        return default_types.at(token.value);
    }
}

