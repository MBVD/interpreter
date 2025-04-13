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
    const auto& declorators = node->get_init_declarators();
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
    auto* decl = node->get_declarator().get();
    auto* expr = node->get_expression().get();
    this->visit(decl);
    if (expr != nullptr){
        std::cout<<" = ";
    }
    this->visit(expr);
}

void Printer::visit(IdDeclorator* node) {
    std::cout << node->get_id();
    auto type = node->get_declorator_type();
    auto* expr = node->get_expression().get();
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
    this->visit(expr);
    if (type == IDDeclaratorType::ARRAY) {
        std::cout << "]";
    }
}

void Printer::visit(FuncDeclarator* node) {
    auto type = node->get_returnable_type();
    auto id = node->get_name();
    const auto& param_decls = node->get_params();
    auto* block = node->get_block().get();
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
    this->visit(block);
}

void Printer::visit(ParamDeclarator* node) {
    auto id = node->get_type();
    auto* decl = node->get_declorator().get();
    std::cout << id << " ";
    this->visit(decl);
}

void Printer::visit(StructDeclarator* node) {
    auto id = node->get_id();
    const auto& vars = node->get_vars();
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
    auto* left = node->get_left().get();
    auto* right = node->get_right().get();
    auto op = node->get_op();
    this->visit(left);
    std::cout << " " << op << " ";
    this->visit(right);
}

void Printer::visit(TernaryExpression* node) {
    auto* cond_expr = node->get_cond_expression().get();
    auto* true_expr = node->get_true_expression().get();
    auto* false_expr = node->get_false_expression().get();
    this->visit(cond_expr);
    std::cout << " ? ";
    this->visit(true_expr);
    std::cout << " : ";
    this->visit(false_expr);
}

void Printer::visit(BinaryExpression* node) {
    auto* left = node->get_left().get();
    auto* right = node->get_right().get();
    auto op = node->get_op();
    this->visit(left);
    std::cout << " " << op << " ";
    this->visit(right);
}

void Printer::visit(UnaryExpression* node) {
    auto op = node->get_op();
    auto* base = node->get_base().get();
    std::cout << op;
    this->visit(base);
}

void Printer::visit(PostfixExpression* node){
    auto* base = node->get_expression().get();
    auto op = node->get_op();
    this->visit(base);
    std::cout<<op;
}

void Printer::visit(SubscriptExpression* node) {
    auto* expr = node->get_expression().get();
    const auto& indexes = node->get_indexes();
    this->visit(expr);
    for (auto& i : indexes) {
        std::cout << "[";
        this->visit(i.get());
        std::cout << "]";
    }
}

void Printer::visit(CallExpression* node) {
    auto* expr = node->get_expression().get();
    const auto& args = node->get_args();
    this->visit(expr);
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
    auto* expr = node->get_expression().get();
    auto member = node->get_member();
    this->visit(expr);
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
    auto* base = node->get_base().get();
    std::cout<<"(";
    this->visit(base);
    std::cout<<")";
}

void Printer::visit(Statement* node){
    if (node == nullptr){
        return;
    }
    node->accept(*this);
}

void Printer::visit(ExpressionStatement* node) {
    auto* base = node->get_expression().get();
    this->visit(base);
    std::cout<<";";
}

void Printer::visit(DeclarationStatement* node) {
    auto* var_decl = node->get_declaration().get();
    this->visit(var_decl);
}

void Printer::visit(ReturnStatement* node) {
    auto* expr = node->get_expression().get();
    std::cout<<"return ";
    if (expr != nullptr)
        this->visit(expr);
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
    for (auto& i : statements) {
        this->visit(i.get());
    }
    std::cout << "}";
}

void Printer::visit(ConditionalStatement* node) {
    auto* cond_expr = node->get_conditional().get();
    auto* true_statement = node->get_true_statement().get();
    auto* false_statement = node->get_false_statement().get();
    std::cout << "if (";
    this->visit(cond_expr);
    std::cout << ")";
    this->visit(true_statement);
    if (false_statement != nullptr) {
        std::cout << "else ";
        this->visit(false_statement);
    }
}

void Printer::visit(LoopStatement* node) {
    if (node == nullptr){
        return;
    }
    node->accept(*this);
}

void Printer::visit(WhileStatement* node) {
    auto* cond_expr = node->get_conditional().get();
    auto* statement = node->get_statement().get();
    std::cout << "while (";
    this->visit(cond_expr);
    std::cout << ")";
    this->visit(statement);
}

void Printer::visit(DoWhileStatement* node) {
    auto* statement = node->get_statement().get();
    auto* expression = node->get_expression().get();
    std::cout<<"do ";
    this->visit(statement);
    std::cout<<"while (";
    this->visit(expression);
    std::cout<<");";
}

void Printer::visit(ForStatement* node) {
    auto* var_decl = node->get_var().get();
    auto* expr = node->get_init_expr().get();
    auto* cond_expr = node->get_cond_expr().get();
    auto* iter_expr = node->get_iter_expr().get();
    auto* statement = node->get_statement().get();
    std::cout << "for (";
    if (var_decl != nullptr) {
        this->visit(var_decl);
    } else {
        this->visit(expr);
    }
    this->visit(cond_expr);
    std::cout << ";";
    this->visit(iter_expr);
    std::cout << ")";
    this->visit(statement);
}