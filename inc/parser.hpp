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
private:
    static std::unordered_map<Token, IDDeclaratorType> id_modifiers;

    
    std::vector<Token> tokens;
    size_t index;
    decl_ptr parse_declaration();
    var_ptr parse_var_declaration();
    init_ptr parse_init_declaration();
    id_ptr parse_id_declaration();
    decl_ptr parse_func_declaration();
    decl_ptr parse_param_declaration();
    decl_ptr parse_struct_declaration();
    
    using expr_ptr = std::shared_ptr<Expression>;
    expr_ptr parse_expression();

    using stetement_ptr = std::shared_ptr<Statement>;
    stetement_ptr parse_statement();
};