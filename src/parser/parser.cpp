#include "parser.hpp"
#include "exceptions.hpp"
#include <iostream>

std::unordered_map<Token, IDDeclaratorType> Parser::id_modifiers = {
    {{TokenType::MULTIPLY}, IDDeclaratorType::POINTER},
    {{TokenType::BIT_AND}, IDDeclaratorType::REF},
    {{TokenType::INDEX_LEFT}, IDDeclaratorType::ARRAY}
};

Parser::decl_ptr Parser::parse_declaration() {
    try {
        return parse_var_declaration();
    } catch (const parse_var_decl_error&) {
        std::cout<<"failed to parse var_decl \n";
    }
    try {
        return parse_func_declaration();
    } catch (const parse_func_decl_error&) {
        std::cout<<"failed to parse func_decl \n";
    }
    try {
        return parse_struct_declaration();
    } catch (const parse_struct_decl_error&) {
        std::cout<<"failed to parse struct_decl \n";
    }
    throw declaration_parsing_error("no decalarations");
}

Parser::statement_ptr Parser::parse_statement() {
    try {
        return parse_expression_stetement();
    } catch (const parse_expression_st_error&) {
        std::cout<<"";
    }

    try {
        return parse_decl_statement();
    } catch (const parse_decl_st_error&) {
        std::cout<<"";
    }

    try {
        return parse_conditional_statement();
    } catch (const parse_conditional_st_error&) {
        std::cout<<"";
    }

    try {
        return parse_loop_statement();
    } catch (const parse_loop_st_error&) {
        std::cout<<"";
    }

    try {
        return parse_return_statement();
    } catch (const parse_return_st_error&) {
        std::cout<<"";
    }

    try {
        return parse_continue_statement();
    } catch (const parse_continue_st_error&) {
        std::cout<<"";
    }

    try {
        return parse_break_statement();
    } catch (const parse_break_st_error&) {
        std::cout<<"";
    }

    try {
        return parse_decl_statement();
    } catch (const parse_decl_st_error&) {
        std::cout<<"";
    }

    try {
        return parse_block_statement();
    } catch (const parse_block_st_error&) {
        std::cout<<"";
    }
}



