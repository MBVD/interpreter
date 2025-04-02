#include "parser.hpp"
#include "exceptions.hpp"
#include <iostream>

std::unordered_map<TokenType, IDDeclaratorType> Parser::id_modifiers = {
    {TokenType::MULTIPLY, IDDeclaratorType::POINTER},
    {TokenType::BIT_AND, IDDeclaratorType::REF},
    {TokenType::INDEX_LEFT, IDDeclaratorType::ARRAY}
};

std::unique_ptr<ASTNode> Parser::parse() {
    try {
        return parse_declaration();
        return parse_statement();
        return parse_expression();
    } catch (declaration_parsing_error&) {
        std::cout<<"no decl\n";
        throw;
    }
    throw parsing_errors(""); // TODO make message
}

Parser::decl_ptr Parser::parse_declaration() {
    try {
        return parse_var_declaration();
        return parse_func_declaration();
        return parse_struct_declaration();
    } catch (const declaration_parsing_error&) {
        std::cout<<"failed to parse var_decl \n";
        throw;
    }
    throw declaration_parsing_error("no decalarations");
}

Parser::statement_ptr Parser::parse_statement() {
    try {
        return parse_expression_stetement();
        return parse_decl_statement();
        return parse_conditional_statement();
        return parse_loop_statement();
        return parse_return_statement();
        return parse_break_statement();
        return parse_continue_statement();
        return parse_decl_statement();
        return parse_block_statement();
    } catch (const statement_parsing_error&) {
        throw; 
    }
    throw statement_parsing_error(""); //TODO mesasage
}

Parser::expr_ptr Parser::parse_expression(){
    try {
        return parse_binary_expression();
        return parse_ternary_expression();
    } catch (const expression_parsing_error&){
        throw;
    }
    throw expression_parsing_error(""); //TODO mesasage
}



