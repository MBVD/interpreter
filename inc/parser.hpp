#include <vector>
#include <unordered_map>
#include "token.hpp"
#include "expression.hpp"
#include "statement.hpp"
#include "declaration.hpp"

class Parser {
public:
    Parser(const std::vector<Token>& tokens) : tokens(tokens) {};
    std::unique_ptr<ASTNode> parse();
    using decl_ptr = std::unique_ptr<Declaration>;
    using var_ptr = std::unique_ptr<VarDeclaration>;
    using init_ptr = std::unique_ptr<InitDeclarator>;
    using id_ptr = std::unique_ptr<IdDeclorator>;

    using param_ptr = std::unique_ptr<ParamDeclarator>;
    using struct_ptr = std::unique_ptr<StructDeclaration>;
    

    
    using expr_ptr = std::unique_ptr<Expression>;

    using statement_ptr = std::unique_ptr<Statement>;
    using expr_st_ptr = std::unique_ptr<ExpressionStatement>;
    using cond_st_ptr = std::unique_ptr<ConditionalStatement>;
    using loop_st_ptr = std::unique_ptr<LoopStatement>;
    using return_st_ptr = std::unique_ptr<ReturnStatement>;
    using cont_st_ptr = std::unique_ptr<ConditionalStatement>;
    using break_st_ptr = std::unique_ptr<BreakStatement>;
    using decl_st_ptr = std::unique_ptr<DeclorationStatement>;

    using block_st_ptr = std::unique_ptr<BlockStatement>;
private:
    static std::unordered_map<Token, IDDeclaratorType> id_modifiers;

    
    std::vector<Token> tokens;
    size_t index;
    decl_ptr parse_declaration();
    var_ptr parse_var_declaration();
    init_ptr parse_init_declaration();
    id_ptr parse_id_declaration();

    decl_ptr parse_func_declaration();
    param_ptr parse_param_declaration();
    struct_ptr parse_struct_declaration();
    
    
    expr_ptr parse_expression();

    
    statement_ptr parse_statement();
    expr_st_ptr parse_expression_stetement();
    cond_st_ptr parse_conditional_statement();
    loop_st_ptr parse_loop_statement();
    return_st_ptr parse_return_statement();
    cont_st_ptr parse_continue_statement();
    break_st_ptr parse_break_statement();
    decl_st_ptr parse_decl_statement();
    block_st_ptr parse_block_statement();
};