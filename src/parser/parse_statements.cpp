#include "exceptions.hpp"
#include "parser.hpp"
#include <iostream>


Parser::expr_st_ptr Parser::parse_expression_stetement(){
    auto expr_st_index = index;
    try {
        auto expr = parse_expression();
        if (this->tokens[index] != TokenType::SEMICOLON){
            throw parse_expression_st_error("");
        }
        index++;
        return std::make_unique<ExpressionStatement>(expr);
    } catch (const expression_parsing_error&) {
        index = expr_st_index;
        throw parse_expression_st_error("");
    }
}

Parser::decl_st_ptr Parser::parse_decl_statement() {
    auto decl_st_index = index;
    Parser::var_ptr var_decl;
    try {
        var_decl = parse_var_declaration();
        return std::make_unique<DeclorationStatement>(var_decl);
    } catch (const parse_var_decl_error&) {
        index = decl_st_index;
        throw parse_decl_st_error("");
    }
}

Parser::cond_st_ptr Parser::parse_conditional_statement() {
    
}

Parser::loop_st_ptr Parser::parse_loop_statement() {

}

Parser::return_st_ptr Parser::parse_return_statement() {

}

Parser::cont_st_ptr Parser::parse_continue_statement() {

}

Parser::break_st_ptr Parser::parse_break_statement() {

}

Parser::decl_st_ptr Parser::parse_decl_statement() {

}

Parser::block_st_ptr Parser::parse_block_statement() {

}