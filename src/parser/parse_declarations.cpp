#include "parser.hpp"
#include "exceptions.hpp"
#include <iostream>

Parser::decl_ptr Parser::parse_func_declaration() {
    auto func_index = this->index;
    auto returnable_type = this->tokens[index];
    if (returnable_type != TokenType::ID && returnable_type != TokenType::TYPE) {
        index = func_index;
        throw parse_func_decl_error("");
    }
    index++;
    auto name = this->tokens[index];
    if (name != TokenType::ID) {
        index = func_index;
        throw parse_func_decl_error("");
    }
    index++;

    if (auto i = this->tokens[index]; i != TokenType::PARENTHESIS_LEFT) {
        index = func_index;
        throw parse_func_decl_error("");
    }
    index++;

    std::vector<Parser::param_ptr> params;

    while (this->tokens[index] != TokenType::PARENTHESIS_RIGHT && this->tokens[index] != TokenType::END) {
        if (this->tokens[index] == TokenType::COMMA) {
            index++;
        }
        try {
            params.push_back(parse_param_declaration());
        } catch (parse_param_decl_error&) {
            throw parse_func_decl_error("");
        }
    }
    if (this->tokens[++index] == TokenType::SEMICOLON) {
        return std::make_unique<FuncDeclarator>(returnable_type, name, std::move(params));
    } else {
        try {
            Parser::block_st_ptr block = parse_block_statement();
            return std::make_unique<FuncDeclarator>(returnable_type, name, std::move(params), std::move(block));
        } catch (parse_block_st_error&) {
            throw parse_func_decl_error("");
        }
    }
}

Parser::param_ptr Parser::parse_param_declaration() {
    auto param_index = index;
    auto type = this->tokens[index];
    if (type != TokenType::ID && type != TokenType::TYPE) {
        index = param_index;
        throw parse_param_decl_error("");
    }
    index++;
    try {
        auto decl = parse_init_declaration();
        return std::make_unique<ParamDeclarator>(type, std::move(decl));
    } catch (declaration_parsing_error&) {
        throw parse_param_decl_error("");
    }
}

Parser::struct_ptr Parser::parse_struct_declaration() {
    auto struct_index = index;
    if (this->tokens[index].value != "struct") {
        index = struct_index;
        throw parse_struct_decl_error("");
    }
    index++;
    auto id = this->tokens[index++];
    if (id != TokenType::ID) {
        index = struct_index;
        throw parse_struct_decl_error("");
    }
    if (this->tokens[index] != TokenType::BRACE_LEFT) {
        index = struct_index;
        throw parse_struct_decl_error("");
    }
    index++;

    std::vector<Parser::var_ptr> vars;
    while (this->tokens[index] != TokenType::BRACE_RIGHT && this->tokens[index] != TokenType::END) {
        try {
            vars.push_back(parse_var_declaration());
        } catch (parse_var_decl_error&) {
            index = struct_index;
            throw parse_struct_decl_error("");
        }
    }
    if (this->tokens[index++] != TokenType::BRACE_RIGHT) {
        index = struct_index;
        throw parse_struct_decl_error("");
    }
    return std::make_unique<StructDeclarator>(id, std::move(vars));
}

Parser::var_ptr Parser::parse_var_declaration() {
    auto var_index = index;
    if (this->tokens[index] != TokenType::TYPE && this->tokens[index] != TokenType::ID) {
        index = var_index;
        throw parse_var_decl_error("no var decl");
    }
    auto type = this->tokens[index++];
    std::unique_ptr<InitDeclarator> first_declared;
    try {
        first_declared = parse_init_declaration();
    } catch (parse_init_decl_error&) {
        index = var_index;
        throw parse_var_decl_error("no var decl");
    }

    std::vector<std::unique_ptr<InitDeclarator>> declared;
    declared.push_back(std::move(first_declared));
    while (this->tokens[index] == TokenType::COMMA) {
        index++;
        try {
            declared.push_back(parse_init_declaration());
        } catch (parse_init_decl_error&){
            throw parse_var_decl_error("no var decl");
        } 
    }

    if (this->tokens[index++] != TokenType::SEMICOLON) {
        index = var_index;
        throw parse_var_decl_error("expected semicolon");
    }

    return std::make_unique<VarDeclarator>(type, std::move(declared));
}

Parser::init_ptr Parser::parse_init_declaration() {
    auto init_index = index;
    std::unique_ptr<IdDeclorator> declorator;
    try {
        declorator = parse_id_declaration();
    } catch (parse_id_decl_error&) {
        index = init_index;
        throw parse_init_decl_error("");
    }

    if (this->tokens[index] == TokenType::ASSIGN) {
        index++;
        std::unique_ptr<Expression> expr;
        try {
            expr = parse_expression();
        } catch (expression_parsing_error&) {
            throw parse_init_decl_error("");
        }
        return std::make_unique<InitDeclarator>(std::move(declorator), std::move(expr));
    }
    return std::make_unique<InitDeclarator>(std::move(declorator));
}

Parser::id_ptr Parser::parse_id_declaration() {
    auto id = this->tokens[index];
    if (id != TokenType::ID) {
        throw parse_id_decl_error("");
    }
    index++;
    if (id_modifiers.contains(id.type)) {
        auto id_type = id_modifiers[id.type];
        if (this->tokens[index] == TokenType::INDEX_LEFT) {
            index++;
            auto expr = parse_expression();
            return std::make_unique<IdDeclorator>(id, id_type, std::move(expr));
        }
        return std::make_unique<IdDeclorator>(id, id_type);
    }
    return std::make_unique<IdDeclorator>(id);
}