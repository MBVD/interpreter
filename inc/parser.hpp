#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include "token.hpp"
#include "expression.hpp"
#include "statement.hpp"
#include "declaration.hpp"

class Parser {
public:
    Parser(const std::vector<Token>& tokens) : tokens(tokens) {};
    std::unique_ptr<ASTNode> parse();
    using decl_ptr = std::unique_ptr<Declarator>;
    using var_ptr = std::unique_ptr<VarDeclarator>;
    using init_ptr = std::unique_ptr<InitDeclarator>;
    using id_ptr = std::unique_ptr<IdDeclorator>;
    using param_ptr = std::unique_ptr<ParamDeclarator>;
    using struct_ptr = std::unique_ptr<StructDeclarator>;
    
    using expr_ptr = std::unique_ptr<Expression>;
    using ternary_expr_ptr = std::unique_ptr<TernaryExpression>;
    using binary_expr_ptr = std::unique_ptr<BinaryExpression>;
    using unary_expr_ptr = std::unique_ptr<UnaryExpression>;
    using post_expr_ptr = std::unique_ptr<PostfixExpression>;
    using access_expr_ptr = std::unique_ptr<AccessExpression>;
    using subscript_expr_ptr = std::unique_ptr<SubscriptExpression>;
    using call_expr_ptr = std::unique_ptr<CallExpression>;
    using literal_expr_ptr = std::unique_ptr<LiteralExpression>;
    using id_expr_ptr = std::unique_ptr<IDexpression>;
    using group_expr_ptr = std::unique_ptr<GroupExpression>;

    using statement_ptr = std::unique_ptr<Statement>;
    using expr_st_ptr = std::unique_ptr<ExpressionStatement>;
    using cond_st_ptr = std::unique_ptr<ConditionalStatement>;
    using loop_st_ptr = std::unique_ptr<LoopStatement>;
    using while_st_ptr = std::unique_ptr<WhileStatement>;
    using do_while_st_ptr = std::unique_ptr<DoWhileStatement>;
    using for_st_prt = std::unique_ptr<ForStatement>;
    using return_st_ptr = std::unique_ptr<ReturnStatement>;
    using cont_st_ptr = std::unique_ptr<ContinueStatement>;
    using break_st_ptr = std::unique_ptr<BreakStatement>;
    using decl_st_ptr = std::unique_ptr<DeclarationStatement>;

    using block_st_ptr = std::unique_ptr<BlockStatement>;
private:
    static std::unordered_map<TokenType, IDDeclaratorType> id_modifiers;
    static std::unordered_set<TokenType> unary_ops;

    
    std::vector<Token> tokens;
    size_t index = 0;
    decl_ptr parse_declaration();
    var_ptr parse_var_declaration();
    init_ptr parse_init_declaration();
    id_ptr parse_id_declaration();

    decl_ptr parse_func_declaration();
    param_ptr parse_param_declaration();
    struct_ptr parse_struct_declaration();
    
    
    expr_ptr parse_expression();
    ternary_expr_ptr parse_ternary_expression();
    expr_ptr parse_binary_expression();
    expr_ptr parse_sum_expression();
    expr_ptr parse_mul_expression();
    expr_ptr parse_pow_expression();
    expr_ptr parse_unary_expression();
    expr_ptr parse_base();
    expr_ptr parse_post_expression();
    access_expr_ptr parse_access_expression(expr_ptr& base);
    subscript_expr_ptr parse_subscript_expression(expr_ptr& base);
    call_expr_ptr parse_call_expression(expr_ptr& base);


    
    statement_ptr parse_statement();
    expr_st_ptr parse_expression_stetement();
    cond_st_ptr parse_conditional_statement();
    loop_st_ptr parse_loop_statement();
    while_st_ptr parse_while_statement();
    do_while_st_ptr parse_do_while_statement();
    for_st_prt parse_for_statement();
    return_st_ptr parse_return_statement();
    cont_st_ptr parse_continue_statement();
    break_st_ptr parse_break_statement();
    decl_st_ptr parse_decl_statement();
    block_st_ptr parse_block_statement();
};