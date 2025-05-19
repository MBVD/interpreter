#include <iostream>
#include "printer.hpp"

void Printer::print(TranslationUnit& unit){
    for (const auto& i : unit.get_nodes()){
        i->accept(*this);
    }
    std::cout<<"\n";
}

void Printer::visit(ASTNode* node){
    node->accept(*this);
}

void Printer::visit(Declarator* node){
    if (node == nullptr){
        return;
    }
    node->accept(*this);
}

void Printer::visit(VarDeclarator* node) {
    std::cout << node->get_type() << " ";
    const auto& declorators = node->get_init_declarators();
    int sz = declorators.size(), cnt = 0;
    for (const auto& i : declorators) {
        i->accept(*this);
        if (cnt != sz - 1) {
            std::cout << ", ";
        }
        cnt++;
    }
    std::cout << ";";
}

void Printer::visit(InitDeclarator* node) {
    const auto& decl = node->get_declarator();
    const auto& expr = node->get_expression();
    decl->accept(*this);
    if (expr != nullptr){
        std::cout<<" = ";
        expr->accept(*this);
    }
}

void Printer::visit(IdDeclorator* node) {
    std::cout << node->get_id();
    auto type = node->get_declorator_type();
    const auto& expr = node->get_expression();
    switch (type) {
        case IDDeclaratorType::NONE: {
            break;
        }
        case IDDeclaratorType::REF: {
            std::cout << "&";
            break;
        }
        case IDDeclaratorType::POINTER: {
            std::cout << "*";
            break;
        }
        default: {
            std::cout << "[";
            break;
        }
    }
    if (expr == nullptr) {
        return;
    }
    expr->accept(*this);
    if (type == IDDeclaratorType::ARRAY) {
        std::cout << "]";
    }
}

void Printer::visit(FuncDeclarator* node) {
    auto type = node->get_returnable_type();
    auto id = node->get_name();
    const auto& param_decls = node->get_params();
    const auto& block = node->get_block();
    std::cout << type << " " << id << " (";
    int sz = param_decls.size(), cnt = 0;
    for (const auto& i : param_decls) {
        i->accept(*this);
        if (cnt != sz - 1) {
            std::cout << ", ";
        }
        cnt++;
    }
    std::cout << ")";
    block->accept(*this);
}

void Printer::visit(ParamDeclarator* node) {
    auto id = node->get_type();
    const auto& decl = node->get_declorator();
    std::cout << id << " ";
    decl->accept(*this);
}

void Printer::visit(StructDeclarator* node) {
    auto id = node->get_id();
    const auto& vars = node->get_vars();
    std::cout << "struct " << id << "{ ";
    for (const auto& i : vars) {
        i->accept(*this);
    }
    std::cout << "}";
}

void Printer::visit(Expression* node) {
    if (node == nullptr) {
        return;
    }
    node->accept(*this);
}

void Printer::visit(ComparisonExpression* node) {
    const auto& left = node->get_left();
    const auto& right = node->get_right();
    auto op = node->get_op();
    left->accept(*this);
    std::cout << " " << op << " ";
    right->accept(*this);
}

void Printer::visit(TernaryExpression* node) {
    const auto& cond_expr = node->get_cond_expression();
    const auto& true_expr = node->get_true_expression();
    const auto& false_expr = node->get_false_expression();
    cond_expr->accept(*this);
    std::cout << " ? ";
    true_expr->accept(*this);
    std::cout << " : ";
    false_expr->accept(*this);
}

void Printer::visit(BinaryExpression* node) {
    const auto& left = node->get_left();
    const auto& right = node->get_right();
    auto op = node->get_op();
    left->accept(*this);
    std::cout << " " << op << " ";
    right->accept(*this);
}

void Printer::visit(UnaryExpression* node) {
    auto op = node->get_op();
    const auto& base = node->get_base();
    std::cout << op;
    this->visit(base.get());
}

void Printer::visit(PostfixExpression* node){
    const auto& base = node->get_expression();
    auto op = node->get_op();
    base->accept(*this);
    std::cout<<op;
}

void Printer::visit(SubscriptExpression* node) {
    const auto& expr = node->get_expression();
    const auto& indexes = node->get_indexes();
    expr->accept(*this);
    for (const auto& i : indexes) {
        std::cout << "[";
        i->accept(*this);
        std::cout << "]";
    }
}

void Printer::visit(CallExpression* node) {
    const auto& expr = node->get_expression();
    const auto& args = node->get_args();
    expr->accept(*this);
    std::cout << "(";
    int cnt = 0, sz = args.size();
    for (const auto& i : args) {
        i->accept(*this);
        if (cnt != sz - 1) {
            std::cout << ", ";
        }
        cnt++;
    }
    std::cout << ")";
}

void Printer::visit(AccessExpression* node) {
    const auto& expr = node->get_expression();
    auto member = node->get_member();
    expr->accept(*this);
    std::cout<<"->";
    std::cout<<member;
}

void Printer::visit(LiteralNumExpression* node) {
    auto value = node->get_value();
    std::cout<<value;
}

void Printer::visit(LiteralFloatExpression* node){
    auto value = node->get_value();
    std::cout<<value;
}

void Printer::visit(LiteralCharExpression* node){
    auto value = node->get_value();
    std::cout<<value;
}

void Printer::visit(LiteralStringExpression* node){
    auto value = node->get_value();
    std::cout<<value;
}

void Printer::visit(IDexpression* node){
    auto token = node->get_token();
    std::cout<<token;
}

void Printer::visit(GroupExpression* node) {
    const auto& base = node->get_base();
    std::cout<<"(";
    base->accept(*this);
    std::cout<<")";
}

void Printer::visit(Statement* node){
    if (node == nullptr){
        return;
    }
    node->accept(*this);
}

void Printer::visit(ExpressionStatement* node) {
    const auto& base = node->get_expression();
    base->accept(*this);
    std::cout<<";";
}

void Printer::visit(DeclarationStatement* node) {
    const auto& var_decl = node->get_declaration();
    var_decl->accept(*this);
}

void Printer::visit(ReturnStatement* node) {
    const auto& expr = node->get_expression();
    std::cout<<"return ";
    if (expr != nullptr)
        expr->accept(*this);
    std::cout<<";";
}

void Printer::visit(BreakStatement*) {
    std::cout<<"break; ";
}

void Printer::visit(ContinueStatement*) {
    std::cout<<"continue; ";
}

void Printer::visit(BlockStatement* node) {
    const auto& statements = node->get_statements();
    std::cout << "{";
    for (const auto& i : statements) {
        i->accept(*this);
    }
    std::cout << "}";
}

void Printer::visit(ConditionalStatement* node) {
    const auto& cond_expr = node->get_conditional();
    const auto& true_statement = node->get_true_statement();
    const auto& false_statement = node->get_false_statement();
    std::cout << "if (";
    cond_expr->accept(*this);
    std::cout << ")";
    true_statement->accept(*this);
    if (false_statement != nullptr) {
        std::cout << "else ";
        false_statement->accept(*this);
    }
}

void Printer::visit(LoopStatement* node) {
    if (node == nullptr){
        return;
    }
    node->accept(*this);
}

void Printer::visit(WhileStatement* node) {
    const auto& cond_expr = node->get_conditional();
    const auto& statement = node->get_statement();
    std::cout << "while (";
    cond_expr->accept(*this);
    std::cout << ")";
    statement->accept(*this);
}

void Printer::visit(DoWhileStatement* node) {
    const auto& statement = node->get_statement();
    const auto& expression = node->get_expression();
    std::cout<<"do ";
    statement->accept(*this);
    std::cout<<"while (";
    expression->accept(*this);
    std::cout<<");";
}

void Printer::visit(ForStatement* node) {
    const auto& var_decl = node->get_var();
    const auto& expr = node->get_init_expr();
    const auto& cond_expr = node->get_cond_expr();
    const auto& iter_expr = node->get_iter_expr();
    const auto& statement = node->get_statement();
    std::cout << "for (";
    if (var_decl != nullptr) {
        var_decl->accept(*this);
    } else {
        expr->accept(*this);
    }
    cond_expr->accept(*this);
    std::cout << ";";
    iter_expr->accept(*this);
    std::cout << ")";
    statement->accept(*this);
}

void Printer::visit(EmptyStatement* node){
    std::cout<<";";
    return;
}