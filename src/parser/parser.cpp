#include "parser.hpp"
#include "exceptions.hpp"
#include <iostream>

std::unordered_map<TokenType, IDDeclaratorType> Parser::id_modifiers = {
    {TokenType::MULTIPLY, IDDeclaratorType::POINTER},
    {TokenType::BIT_AND, IDDeclaratorType::REF},
    {TokenType::INDEX_LEFT, IDDeclaratorType::ARRAY}
};

std::unordered_set<TokenType> Parser::comp_ops = {
    TokenType::GREATER, TokenType::LESS, TokenType::GREATER_EQUAL, TokenType::LESS_EQUAL
};

std::unordered_set<TokenType> Parser::unary_ops = {
    TokenType::INCREMENT, TokenType::DECREMENT, TokenType::PLUS, TokenType::MINUS, TokenType::TYPE, TokenType::BIT_NOT
};

std::unordered_set<TokenType> Parser::asssign_ops = {
    TokenType::ASSIGN, TokenType::PLUS_ASSIGN, TokenType::MINUS_ASSIGN, TokenType::MULTIPLY_ASSIGN, TokenType::DIVIDE_ASSIGN, TokenType::MODULO_ASSIGN, 
    TokenType::RIGHT_SHIFT_ASSIGN, TokenType::LEFT_SHIFT_ASSIGN, TokenType::AND_ASSIGN, TokenType::XOR_ASSIGN, TokenType::OR_ASSIGN
};

std::unordered_set<TokenType> Parser::post_ops = {
    TokenType::INCREMENT, TokenType::DECREMENT, TokenType::ARROW, TokenType::INDEX_LEFT, TokenType::PARENTHESIS_LEFT
};

TranslationUnit Parser::parse() {
    std::vector<std::unique_ptr<ASTNode>>nodes;
    while (this->tokens[index] != TokenType::END){
        nodes.push_back(parse_declaration());
    }
    return TranslationUnit(std::move(nodes));
}

Parser::decl_ptr Parser::parse_declaration() {
    auto decl_index = index;
    try {
        return parse_var_declaration();
    } catch (parse_var_decl_error&) {}
    try {
        return parse_func_declaration();
    } catch (parse_func_decl_error&){}

    try {
        return parse_struct_declaration();
    } catch (parse_struct_decl_error&){}
    throw declaration_parsing_error("no decalarations");
}

Parser::statement_ptr Parser::parse_statement() {
    auto statement_index = index;
    try {
        return parse_decl_statement();
    } catch (parse_decl_st_error&) {}
    try {
        return parse_conditional_statement();
    } catch (parse_conditional_st_error&) {}
    try {
        return parse_loop_statement();
    } catch (parse_loop_st_error&) {}
    try {
        return parse_return_statement();
    } catch (parse_return_st_error&){}
    try {
        return parse_break_statement();
    } catch (parse_break_st_error&) {}
    try {
        return parse_continue_statement();
    } catch (parse_continue_st_error& ) {}
    try {
        return parse_decl_statement();
    } catch (parse_decl_st_error&) {}
    try {
        return parse_block_statement();
    } catch (parse_block_st_error&) {}
    try {
        return parse_empty_statement();
    } catch (statement_parsing_error&){}
    try {
        return parse_expression_stetement();  
    } catch (parse_expression_st_error&) {}
    throw statement_parsing_error("");
}

Parser::expr_ptr Parser::parse_expression(){
    auto expression = parse_comma_expression();
    return expression;
}



