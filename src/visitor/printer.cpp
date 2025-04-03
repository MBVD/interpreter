#include <iostream>
#include "printer.hpp"

void Printer::visit(const std::unique_ptr<VarDeclarator>& node ) {
    std::cout<<node->get_type();
    const std::vector<std::unique_ptr<InitDeclarator>>& declorators = node->get_init_declarators();
    int sz = declorators.size(), cnt = 0;
    for (auto& i : declorators){
        this->visit(i);
        if (cnt != sz - 1){
            std::cout<<", ";
        }
        cnt++;
    }
    std::cout<<";";
}
void Printer::visit(const std::unique_ptr<InitDeclarator>& node ) {
    auto& decl = node->get_declarator();
    auto& expr = node->get_expression();
    this -> visit(decl);
    this->visit(expr);
    std::cout << "\n";
}
void Printer::visit(const std::unique_ptr<Expression>& node){
    if (node == nullptr){
        return;
    }
    node->accept(*this);   
}
void Printer::visit(const std::unique_ptr<IdDeclorator>& node ) {
    std::cout << node->get_id();
    auto type = node->get_declorator_type();
    auto& expr = node->get_expression();
    switch (type){
        case IDDeclaratorType::REF: {
            std::cout<<"&";
        }
        case IDDeclaratorType::POINTER: {
            std::cout<<"*";
        }
        default: {
            std::cout<<"[";
        }
    }    
    this->visit(expr);
    if (type == IDDeclaratorType::ARRAY){
        std::cout<<"]";
    }
    std::cout<<"\n";
    return;
}
void Printer::visit(const std::unique_ptr<FuncDeclarator>& node ) {
    auto type = node->get_returnable_type();
    auto id = node->get_name();
    auto param_decls = node->get_params();
    auto& block = node->get_block();
    std::cout<<type<<" ";
    std::cout<<id<<" ";
    std::cout<<"(";
    int sz = param_decls.size(), cnt = 0;
    for (auto& i : param_decls){
        this->visit(i);
        if (cnt != sz -1) {
            std::cout<<", ";
        }
        cnt++;
    }
    this->visit(block);
    return;
}
void Printer::visit(const std::unique_ptr<ParamDeclarator>& node ) {
    auto id = node->get_type();
    auto& decl = node->get_declorator();
    std::cout<<id<<" ";
    this->visit(decl);
    return;
}

void Printer::visit(const std::unique_ptr<StructDeclarator>& node ) {
    auto id = node->get_id();
    auto vars = node->get_vars();
    std::cout<<"struct "<<id<<"{ ";
    for (auto& i : vars){
        this->visit(i);
    }
    std::cout<<"}";
}


////

void Printer::visit(const std::unique_ptr<Expression>& node){
    if (node == nullptr){
        return;
    }
    node->accept(*this);
}

void Printer::visit(const std::unique_ptr<BinaryExpression>& node) {
    auto& left = node->get_left();
    auto& right = node->get_right();
    auto op = node->get_op();
    this->visit(left);
    std::cout<<" "<<op<<" ";
    this->visit(right);
}
void Printer::visit(const std::unique_ptr<TernaryExpression>& node) {
    auto& cond_expr = node->get_cond_expression();
    auto& true_expr = node->get_true_expression();
    auto& false_expr = node->get_false_expression();
    this->visit(cond_expr);
    std::cout<<" ? ";
    this->visit(true_expr);
    std::cout<<" : ";
    this->visit(false_expr);
}
void Printer::visit(const std::unique_ptr<UnaryExpression>& node) {
    auto op = node->get_op();
    auto& base = node->get_base();
    std::cout<<op;
    this->visit(base);
}
void Printer::visit(const std::unique_ptr<PostfixExpression>& node) {
    if (node == nullptr){
        return;
    }
    node->accept(*this);
}
void Printer::visit(const std::unique_ptr<SubscriptExpression>& node) {
    auto& expr = node->get_expression();
    auto indexes = node->get_indexes();
    this->visit(expr);
    for (auto& i : indexes){
        std::cout<<"[";
        this->visit(i);
        std::cout<<"]";
    }
}
void Printer::visit(const std::unique_ptr<CallExpression>& node) {
    auto& expr = node->get_expression();
    auto args = node->get_args();
    this->visit(expr);
    std::cout<<"(";
    int cnt = 0, sz = args.size();
    for (auto& i : args){
        this->visit(i);
        if (cnt != sz - 1){
            std::cout<<", ";
        }
    }
    std::cout<<")";
}
void Printer::visit(const std::unique_ptr<AccessExpression>& node) {
    auto& expr = node->get_expression();
    auto& access_expr = node->get_expression_to_access();
    this->visit(expr);
    std::cout<<"->";
    this->visit(access_expr);
}
void Printer::visit(const std::unique_ptr<LiteralExpression>& node) {
    auto token = node->get_token();
    std::cout<<token;
}
void Printer::visit(const std::unique_ptr<IDexpression>& node) {
    auto token = node->get_token();
    std::cout<<token;
}
void Printer::visit(const std::unique_ptr<GroupExpression>& node) {
    auto& base = node->get_base();
    std::cout<<"(";
    this->visit(base);
    std::cout<<")";
}

void Printer::visit(const std::unique_ptr<Statement>& node){
    if (node == nullptr){
        return;
    }
    node->accept(*this);
}

void Printer::visit(const std::unique_ptr<ExpressionStatement>& node) {
    auto& expr = node->get_expression();
    this->visit(expr);
    std::cout<<";";
}

void Printer::visit(const std::unique_ptr<DeclorationStatement>& node) {
    auto& var_decl = node->get_declaration();
    this->visit(var_decl);
}

void Printer::visit(const std::unique_ptr<ReturnStatement>& node) {
    std::cout<<"return ";
    auto& return_expr = node->get_expression();
    this->visit(return_expr);
    std::cout<<";";
}

void Printer::visit(const std::unique_ptr<BreakStatement>& node) {
    std::cout<<"break;";
}

void Printer::visit(const std::unique_ptr<ContinueStatement>& node) {
    std::cout<<"continue;";
}

void Printer::visit(const std::unique_ptr<BlockStatement>& node) {
    auto statements = node->get_statements();
    std::cout<<"{";
    for (auto& i : statements){
        this->visit(i);
    }
    std::cout<<"}";
}

void Printer::visit(const std::unique_ptr<ConditionalStatement>& node) {
    auto& cond_expr = node->get_conditional();
    auto& true_statement = node->get_true_statement();
    auto& false_statement = node->get_false_statement();
    std::cout<<"if (";
    this->visit(cond_expr);
    std::cout<<")";
    this->visit(true_statement);
    if (false_statement != nullptr){
        std::cout<<"else ";
        this->visit(false_statement);
    }
}

void Printer::visit(const std::unique_ptr<LoopStatement>& node) {
    if (node == nullptr){
        return;
    }
    node->accept(*this);
}

void Printer::visit(const std::unique_ptr<WhileStatement>& node) {
    auto& cond_expr = node->get_conditional();
    auto& statement = node->get_statement();
    std::cout<<"while (";
    this->visit(cond_expr);
    std::cout<<")";
    this->visit(statement);
}
void Printer::visit(const std::unique_ptr<DoWhileStatement>& node) {
    auto& statement = node->get_statement();
    auto& expr = node->get_expression();
    std::cout<<"do ";
    this->visit(statement);
    std::cout<<"while (";
    this->visit(expr);
    std::cout<<");";
}
void Printer::visit(const std::unique_ptr<ForStatement>& node) {
    auto& var_decl = node->get_var();
    auto& expr = node->get_init_expr();
    auto& cond_expr = node->get_cond_expr();
    auto &iter_expr = node->get_iter_expr();
    auto& statement = node->get_statement();
    std::cout<<"for (";
    if (var_decl != nullptr){
        this->visit(var_decl);
    } else {
        this->visit(expr);
    }
    std::cout<<";";
    this->visit(cond_expr); 
    std::cout<<";";
    this->visit(iter_expr);
    std::cout<<")";
    this->visit(statement);
}