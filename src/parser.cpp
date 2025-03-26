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

Parser::var_ptr Parser::parse_var_declaration() {
    if (this->tokens[index] != TokenType::TYPE && this->tokens[index] != TokenType::ID){
        throw parse_var_decl_error("no var decl");
    }
    auto type = this->tokens[index++];
    auto first_declared = parse_init_declaration();
    std::vector<std::unique_ptr<InitDeclarator>> declared;
    while(this->tokens[index] == TokenType::COMMA){
        declared.push_back(parse_init_declaration());
    }
    // проверка на ;
    if (this->tokens[index++] != TokenType::SEMICOLON){

    }

    return std::make_unique<VarDeclaration>(type, declared);
    //new VarDeclaration(type, declared);
}

Parser::init_ptr Parser::parse_init_declaration() {
    auto declorator = parse_id_declaration();
    if (this->tokens[index] == TokenType::EQUAL) {
        this->index++;
        auto expr = parse_expression();
        return std::make_unique<InitDeclarator>(declorator, expr);
    }
    return std::make_unique<InitDeclarator>(declorator);
}

Parser::id_ptr Parser::parse_id_declaration() {
    auto id = this->tokens[index++];
    if (id_modifiers.contains(id)){
        auto id_type = id_modifiers[id];
        if (id == TokenType::INDEX_LEFT) {
            auto expr = parse_expression();
            return std::make_unique<IdDeclorator>(id, id_type, expr);
        }
        return std::make_unique<IdDeclorator>(id, id_type);
    }
    return std::make_unique<IdDeclorator>(id);
}



