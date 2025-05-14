#include <iostream>
#include "printer.hpp"

void Printer::print(TranslationUnit& unit){
    for (auto& i : unit.get_nodes()){
        this->visit(i.get());
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
    auto declorators = node->get_init_declarators();
    int sz = declorators.size(), cnt = 0;
    for (auto& i : declorators) {
        this->visit(i.get());
        if (cnt != sz - 1) {
            std::cout << ", ";
        }
        cnt++;
    }
    std::cout << ";";
}

void Printer::visit(InitDeclarator* node) {
    auto decl = node->get_declarator();
    auto expr = node->get_expression();
    this->visit(decl.get());
    if (expr != nullptr){
        std::cout<<" = ";
    }
    this->visit(expr.get());
}

void Printer::visit(IdDeclorator* node) {
    std::cout << node->get_id();
    auto type = node->get_declorator_type();
    auto expr = node->get_expression();
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
    this->visit(expr.get());
    if (type == IDDeclaratorType::ARRAY) {
        std::cout << "]";
    }
}

void Printer::visit(FuncDeclarator* node) {
    auto type = node->get_returnable_type();
    auto id = node->get_name();
    auto param_decls = node->get_params();
    auto block = node->get_block();
    std::cout << type << " " << id << " (";
    int sz = param_decls.size(), cnt = 0;
    for (auto& i : param_decls) {
        this->visit(i.get());
        if (cnt != sz - 1) {
            std::cout << ", ";
        }
        cnt++;
    }
    std::cout << ")";
    this->visit(block.get());
}

void Printer::visit(ParamDeclarator* node) {
    auto id = node->get_type();
    auto decl = node->get_declorator();
    std::cout << id << " ";
    this->visit(decl.get());
}

void Printer::visit(StructDeclarator* node) {
    auto id = node->get_id();
    auto vars = node->get_vars();
    std::cout << "struct " << id << "{ ";
    for (auto& i : vars) {
        this->visit(i.get());
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
    auto left = node->get_left();
    auto right = node->get_right();
    auto op = node->get_op();
    this->visit(left.get());
    std::cout << " " << op << " ";
    this->visit(right.get());
}

void Printer::visit(TernaryExpression* node) {
    auto cond_expr = node->get_cond_expression();
    auto true_expr = node->get_true_expression();
    auto false_expr = node->get_false_expression();
    this->visit(cond_expr.get());
    std::cout << " ? ";
    this->visit(true_expr.get());
    std::cout << " : ";
    this->visit(false_expr.get());
}

void Printer::visit(BinaryExpression* node) {
    auto left = node->get_left();
    auto right = node->get_right();
    auto op = node->get_op();
    this->visit(left.get());
    std::cout << " " << op << " ";
    this->visit(right.get());
}

void Printer::visit(UnaryExpression* node) {
    auto op = node->get_op();
    auto base = node->get_base();
    std::cout << op;
    this->visit(base.get());
}

void Printer::visit(PostfixExpression* node){
    auto base = node->get_expression();
    auto op = node->get_op();
    this->visit(base.get());
    std::cout<<op;
}

void Printer::visit(SubscriptExpression* node) {
    auto expr = node->get_expression();
    auto indexes = node->get_indexes();
    this->visit(expr.get());
    for (auto& i : indexes) {
        std::cout << "[";
        this->visit(i.get());
        std::cout << "]";
    }
}

void Printer::visit(CallExpression* node) {
    auto expr = node->get_expression();
    auto args = node->get_args();
    this->visit(expr.get());
    std::cout << "(";
    int cnt = 0, sz = args.size();
    for (auto& i : args) {
        this->visit(i.get());
        if (cnt != sz - 1) {
            std::cout << ", ";
        }
        cnt++;
    }
    std::cout << ")";
}

void Printer::visit(AccessExpression* node) {
    auto expr = node->get_expression();
    auto member = node->get_member();
    this->visit(expr.get());
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
    auto base = node->get_base();
    std::cout<<"(";
    this->visit(base.get());
    std::cout<<")";
}

void Printer::visit(Statement* node){
    if (node == nullptr){
        return;
    }
    node->accept(*this);
}

void Printer::visit(ExpressionStatement* node) {
    auto base = node->get_expression();
    this->visit(base.get());
    std::cout<<";";
}

void Printer::visit(DeclarationStatement* node) {
    auto var_decl = node->get_declaration();
    this->visit(var_decl.get());
}

void Printer::visit(ReturnStatement* node) {
    auto expr = node->get_expression();
    std::cout<<"return ";
    if (expr != nullptr)
        this->visit(expr.get());
    std::cout<<";";
}

void Printer::visit(BreakStatement*) {
    std::cout<<"break; ";
}

void Printer::visit(ContinueStatement*) {
    std::cout<<"continue; ";
}

void Printer::visit(BlockStatement* node) {
    auto statements = node->get_statements();
    std::cout << "{";
    for (auto& i : statements) {
        this->visit(i.get());
    }
    std::cout << "}";
}

void Printer::visit(ConditionalStatement* node) {
    auto cond_expr = node->get_conditional();
    auto true_statement = node->get_true_statement();
    auto false_statement = node->get_false_statement();
    std::cout << "if (";
    this->visit(cond_expr.get());
    std::cout << ")";
    this->visit(true_statement.get());
    if (false_statement != nullptr) {
        std::cout << "else ";
        this->visit(false_statement.get());
    }
}

void Printer::visit(LoopStatement* node) {
    if (node == nullptr){
        return;
    }
    node->accept(*this);
}

void Printer::visit(WhileStatement* node) {
    auto cond_expr = node->get_conditional();
    auto statement = node->get_statement();
    std::cout << "while (";
    this->visit(cond_expr.get());
    std::cout << ")";
    this->visit(statement.get());
}

void Printer::visit(DoWhileStatement* node) {
    auto statement = node->get_statement();
    auto expression = node->get_expression();
    std::cout<<"do ";
    this->visit(statement.get());
    std::cout<<"while (";
    this->visit(expression.get());
    std::cout<<");";
}

void Printer::visit(ForStatement* node) {
    auto var_decl = node->get_var();
    auto expr = node->get_init_expr();
    auto cond_expr = node->get_cond_expr();
    auto iter_expr = node->get_iter_expr();
    auto statement = node->get_statement();
    std::cout << "for (";
    if (var_decl != nullptr) {
        this->visit(var_decl.get());
    } else {
        this->visit(expr.get());
    }
    this->visit(cond_expr.get());
    std::cout << ";";
    this->visit(iter_expr.get());
    std::cout << ")";
    this->visit(statement.get());
}

void Printer::visit(EmptyStatement* node){
    std::cout<<";";
    return;
}