#include <vector>
#include "token.hpp"
#include "expression.hpp"
#include "statement.hpp"
#include "declaration.hpp"

class Parser {
public:
    Parser(const std::vector<Token>& tokens) : tokens(tokens) {};
    std::shared_ptr<ASTNode> parse();
private:
    using decl_ptr = std::shared_ptr<Declaration>;
    std::vector<Token> tokens;
    size_t index;
    decl_ptr parse_declaration();
    decl_ptr parse_var_declaration();
    decl_ptr parse_init_declaration();
    decl_ptr parse_id_declaration();
    decl_ptr parse_func_declaration();
    decl_ptr parse_param_declaration();
    decl_ptr parse_struct_declaration();
    
    using expr_ptr = std::shared_ptr<Expression>;
    expr_ptr parse_expression();

    using stetement_ptr = std::shared_ptr<Statement>;
    stetement_ptr parse_statement();
};