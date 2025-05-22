#include "analyzer.hpp"
#include "token.hpp"
#include "type.hpp"
#include <memory>

std::unordered_map<std::string, std::shared_ptr<Type>> Analyzer::default_types = {
    {"int", std::make_shared<IntegerType>()},
    {"double", std::make_shared<FloatType>()},
    {"float", std::make_shared<FloatType>()},
    {"char", std::make_shared<CharType>()}
};

Analyzer::Analyzer() : scope(std::make_shared<Scope>(nullptr)) {}

bool Analyzer::can_convert(const std::shared_ptr<Type>& from, const std::shared_ptr<Type>& to) {
    if (from == to) return true;

    if (dynamic_cast<const IntegerType*>(to.get()) && dynamic_cast<const IntegerType*>(from.get())) return true;
    if (dynamic_cast<const FloatType*>(to.get()) && dynamic_cast<const FloatType*>(from.get())) return true;
    if (dynamic_cast<const CharType*>(to.get()) && dynamic_cast<const CharType*>(from.get())) return true;
    if (dynamic_cast<const BoolType*>(to.get()) && dynamic_cast<const BoolType*>(from.get())) return true;
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
    if (type.type == TokenType::ID){
        // значит может быть какой то сложной фигней
        // значит если нет его в нашей таблице видимости значит ошиька семанитики. нет такого типа данных
        for (const auto& init_declorator : declarations){
            current_type = scope->match_struct(type.value); // вернет либо обьект либо экспешн что такой структуры нет
            init_declorator->accept(*this);
        }
    }
    if (type.type == TokenType::TYPE){
        // дефолтный типы по типу int char и тд
        for (const auto& init_declorator : declarations){
            auto current_type = default_types.at(type.value);
            init_declorator->accept(*this);
        }
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
        if (dynamic_cast<Arithmetic*>(id_decl_type.get()) && dynamic_cast<Arithmetic*>(expression_type.get())) {
            current_type = id_decl_type;
        } else {
            throw std::runtime_error("not tknown conv");
        }
        // проходимся по expression и проверяем является ли он типом который может конвертироваться в нашу структуру
        // TODO нужно сделать метод который ищет какой scope для нашей структуры и там находить func - конструктор с таким параметром
        // if (typeid(current_type) == typeid(struct_type)) // пока так, потом будем проверять есть ли конструктор
    }
    auto expression_type = current_type;   
    id_declarator->accept(*this);
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
    const auto& args = node->get_params();
    const auto& block = node->get_block();
    std::vector<std::shared_ptr<Type>> type_args;
    scope = scope->create_new_table(scope);
    for (const auto& i : args){
        i->accept(*this); // поверяем являются ли они в зоне видимости
        type_args.push_back(current_type);
        scope-> push_variable(i->get_type().value, current_type);
    }
    auto func = std::make_shared<FuncType>(default_type, type_args);
    scope->push_func(name, func);
    block->accept(*this); // заходим в наш блок
    scope = scope->get_prev_table();
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
    std::unordered_map<std::string, std::shared_ptr<Type>> struct_vars;
    scope = scope->create_new_table(scope);
    for (const auto& var : vars){
        var->accept(*this);
        auto var_type = current_type;
        if (var->get_type().type == TokenType::ID){
            var_type = scope->match_struct(var->get_type().value);
        } else if (var->get_type().type == TokenType::TYPE){
            var_type = default_types.at(var->get_type().value);
        }
        for (const auto& init_declorator : var->get_init_declarators()){
            init_declorator->accept(*this);
            auto name = init_declorator->get_declarator()->get_id().value;
            struct_vars[name] = current_type;
        }
    }
    scope = scope->get_prev_table();
    auto struc = std::make_shared<StructType>(struct_vars);
    scope->push_struct(id.value, struc);
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
    throw std::runtime_error("hello kitty");
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
    throw std::runtime_error("hello kerropi1");
}

void Analyzer::visit(UnaryExpression* node) {// ++ -- (int) 
    const auto& base = node->get_base();
    auto op = node->get_op();
    base->accept(*this);
    auto base_type = current_type;
    switch(op.type) {
        case TokenType::INCREMENT : {
            if (dynamic_cast<Arithmetic*>(base_type.get())) {
                current_type = base_type;
                return;
            } else {
                throw std::runtime_error("hello kerropi");
            }
        } break;
        case TokenType::DECREMENT : {
            if (dynamic_cast<Arithmetic*>(base_type.get())) {
                current_type = base_type;
                return;
            } else {
                throw std::runtime_error("hello kerropi");
            }
        } break;
        case TokenType::PLUS : {
            if (dynamic_cast<Arithmetic*>(base_type.get())) {
                current_type = base_type;
                return;
            } else {
                throw std::runtime_error("hello kerropi");
            }
        } break;
        case TokenType::MINUS : {
            if (dynamic_cast<Arithmetic*>(base_type.get())) {
                current_type = base_type;
            } else {
                throw std::runtime_error("hello kerropi");
            }
        } break;
        case TokenType::TYPE : {
            if (dynamic_cast<Arithmetic*>(base_type.get())) {
                current_type = default_types.at(op.value);
                return;
            } else {
                throw std::runtime_error("hello kerropi");
            }
        }
        case TokenType::BIT_NOT : {
            if (!dynamic_cast<Arithmetic*>(base_type.get())) {
                throw std::runtime_error("hello kerropi");
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
        throw std::runtime_error("hello melodi");
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
    for (const auto& i : indexes){
        i->accept(*this);
        if (!dynamic_cast<IntegerType*>(current_type.get())){
            throw std::runtime_error("hello maru");
        }
    }
    auto* pointer = dynamic_cast<PointerType*>(expression_type.get());
    if (!pointer) {
        throw std::runtime_error("hello maru");    
    }
    if (indexes.size() >= pointer->get_star_count()) {
        throw std::runtime_error("hello maru");
    }
    current_type = pointer->get_type_by_star_count(indexes.size());
}

void Analyzer::visit(CallExpression* node) { // (
    const auto& expression = node->get_expression();
    auto op = node->get_op();
    const auto& args = node->get_args();
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
}

void Analyzer::visit(AccessExpression* node) { // ->
    const auto& expression = node->get_expression();
    auto member_token = node->get_member();

    expression->accept(*this);
    auto expression_type = current_type;

    // Check if the left-hand side is a pointer to a struct
    if (auto pointer_type = dynamic_cast<PointerType*>(expression_type.get())) {
        auto base_type = pointer_type->get_base();
        if (!base_type) {
            throw std::runtime_error("Error: Pointer dereference of incomplete type.");
        }

        if (auto struct_type = dynamic_cast<StructType*>(base_type.get())) {
            auto members = struct_type->get_members();
            auto member_name = member_token.value;

            // Check if the member exists in the struct
            if (members.find(member_name) != members.end()) {
                current_type = members.at(member_name); // Set current_type to the type of the member
            } else {
                throw std::runtime_error("Error: Member '" + member_name + "' not found in struct.");
            }
        } else {
            throw std::runtime_error("Error: Member access on non-struct type.");
        }
    }
     else if (auto struct_type = dynamic_cast<StructType*>(expression_type.get())) {
            auto members = struct_type->get_members();
            auto member_name = member_token.value;

            // Check if the member exists in the struct
            if (members.find(member_name) != members.end()) {
                current_type = members.at(member_name); // Set current_type to the type of the member
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
    
}

void Analyzer::visit(IDexpression* node) {
    current_type = scope->match_variable(node->get_token().value);
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
        return scope->match_struct(token.value);
    } else {
        return default_types.at(token.value);
    }
}

