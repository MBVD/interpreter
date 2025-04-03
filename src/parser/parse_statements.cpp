#include "exceptions.hpp"
#include "parser.hpp"
#include <iostream>
#include "exceptions.hpp"


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
    try {
        auto var_decl = parse_var_declaration();
        return std::make_unique<DeclorationStatement>(var_decl);
    } catch (const parse_var_decl_error&) {
        index = decl_st_index;
        throw parse_decl_st_error("");
    }
}

Parser::cond_st_ptr Parser::parse_conditional_statement() {
    auto cond_index = index;
    auto if_token = this->tokens[index];
    if (if_token != TokenType::IF){
        auto left_parenthesis = this->tokens[index++];
        if (left_parenthesis != TokenType::PARENTHESIS_LEFT){
            throw parse_conditional_st_error(""); //TODO message
        }
        Parser::expr_ptr condition;
        try {
            condition = parse_expression();
        } catch (const expression_parsing_error&){
            index = cond_index;
            throw parse_conditional_st_error(""); //TODO message
        }
        auto right_parenthesis = this->tokens[index++];
        if (right_parenthesis != TokenType::PARENTHESIS_RIGHT){
            throw parse_conditional_st_error("unclosed )");
        }
         
        Parser::statement_ptr true_statement;
        try {
            true_statement = parse_statement();
        } catch (const statement_parsing_error&) {
            index = cond_index;
            throw parse_conditional_st_error(""); //TODO message
        }
        auto else_token = this->tokens[index];
        if (else_token == TokenType::ELSE){
            index++;
            Parser::statement_ptr else_statement;
            try {
                else_statement = parse_statement();
                return std::make_unique<ConditionalStatement>(condition, true_statement, else_statement);
            } catch (const statement_parsing_error&) {
                index = cond_index;
                throw parse_conditional_st_error(""); //TODO message
            }
        } 
        return std::make_unique<ConditionalStatement>(condition, true_statement);
    }
    throw parse_conditional_st_error(""); //TODO message
}

Parser::loop_st_ptr Parser::parse_loop_statement() {
    auto loop_index = index;
    try {
        return parse_while_statement();
        return parse_do_while_statement();
        return parse_for_statement();
    } catch (const parse_loop_st_error&){
        throw;
    }
    throw parse_loop_st_error(""); //TODO message
}

Parser::while_st_ptr Parser::parse_while_statement() {
    auto while_index = index;
    if (this->tokens[index++] == TokenType::WHILE){
        auto parenthesis_left = this->tokens[index++];
        if (parenthesis_left != TokenType::PARENTHESIS_LEFT){
            throw parse_while_statement_error("");
        }
        try {
            auto expression = parse_expression();
            if (this->tokens[index] != TokenType::PARENTHESIS_RIGHT){
                throw parse_while_statement_error("");
            }
            auto statement = parse_statement();
            return std::make_unique<WhileStatement>(expression, statement);
        } catch (const expression_parsing_error&){
            index = while_index;
            throw parse_while_statement_error("");  
        }
    } else {
        index = while_index;
        throw parse_while_statement_error("");
    }
}

Parser::do_while_st_ptr Parser::parse_do_while_statement() {
    auto do_while_index = index;
    if (this->tokens[index++] == TokenType::DO){
        Parser::statement_ptr statement;
        try {
            statement = parse_statement();
        } catch (const statement_parsing_error&) {
            index = do_while_index;
            throw parse_do_while_statement_error("");
        }

        if (this->tokens[index++] != TokenType::WHILE){
            index = do_while_index;
            throw parse_do_while_statement_error("");
        }
        if (this->tokens[index++] != TokenType::PARENTHESIS_LEFT){
            index = do_while_index;
            throw parse_do_while_statement_error("");
        }
        Parser::expr_ptr expression;
        try {
            expression = parse_expression();
        } catch (const expression_parsing_error&){
            index = do_while_index;
            throw parse_do_while_statement_error("");
        }
        if (this->tokens[index++] != TokenType::PARENTHESIS_RIGHT){
            index = do_while_index;
            throw parse_do_while_statement_error("");
        }
        if (this->tokens[index++] != TokenType::SEMICOLON){
            index = do_while_index;
            throw parse_do_while_statement_error("");
        }
        return std::make_unique<DoWhileStatement>(statement, expression);
    } else {
        index = do_while_index;
        throw parse_do_while_statement_error("");
    }
}

Parser::for_st_prt Parser::parse_for_statement() {
    auto for_index = index;
    if (this->tokens[index++] != TokenType::FOR){
        index = for_index;
        throw parse_for_statement_error("");  
    }
    if (this->tokens[index++] != TokenType::PARENTHESIS_LEFT){
        index = for_index;
        throw parse_for_statement_error("");  
    }
    bool is_var = true;
    Parser::var_ptr var;
    Parser::expr_ptr init_expression;
    try {
        var = parse_var_declaration();
    } catch (const parse_var_decl_error&){
        is_var = false;
        try {
            init_expression = parse_expression();
        } catch (const expression_parsing_error&){
            index = for_index;
            throw parse_for_statement_error("");
        }
    }
    if (this->tokens[index++] != TokenType::SEMICOLON){
        throw parse_for_statement_error("");
    }
    Parser::expr_ptr cond_expression;
    try {
        cond_expression = parse_expression();
    } catch (const expression_parsing_error& ){
        index = for_index;
        throw parse_for_statement_error("'");
    }

    if (this->tokens[index++] != TokenType::SEMICOLON){
        throw parse_for_statement_error("");
    }

    Parser::expr_ptr iter_expression;
    try {
        iter_expression = parse_expression();
    } catch (const expression_parsing_error& ){
        index = for_index;
        throw parse_for_statement_error("'");
    }

    if (this->tokens[index++] != TokenType::PARENTHESIS_RIGHT){
        throw parse_for_statement_error("");
    }
    Parser::statement_ptr statement;
    try {
        statement = parse_statement();
    } catch (const statement_parsing_error& ){
        index = for_index;
        throw parse_for_statement_error("");
    }
    
    return is_var ? std::make_unique<ForStatement>(var, cond_expression, iter_expression, statement) : std::make_unique<ForStatement>(init_expression, cond_expression, iter_expression, statement);
}

Parser::return_st_ptr Parser::parse_return_statement() {
    auto return_index = index;
    auto return_token = this->tokens[index++];
    if (return_token == TokenType::RETURN){
        Parser::expr_ptr expression;
        try {
            expression = parse_expression();
            return std::make_unique<ReturnStatement>(expression);   
        } catch (const expression_parsing_error&){
            index = return_index;
            throw parse_return_st_error("");
        }
    } else {
        index = return_index;
        throw parse_return_st_error("");
    }
}

Parser::cont_st_ptr Parser::parse_continue_statement() {
    auto cont_index = index;
    auto cont_token = this->tokens[index++];
    if (cont_token != TokenType::CONTINUE){
        index = cont_index;
        throw parse_continue_st_error("");
    }
    auto semicolon_token = this->tokens[index++];
    if (semicolon_token != TokenType::SEMICOLON){
        index = cont_index;
        throw parse_continue_st_error("");
    }
    return std::make_unique<ContinueStatement>();
}

Parser::break_st_ptr Parser::parse_break_statement() {
    auto break_index = index;
    auto break_token = this->tokens[index++];
    if (break_token != TokenType::BREAK){
        index = break_index;
        throw parse_break_st_error("");
    }
    auto semicolon_token = this->tokens[index++];
    if (semicolon_token != TokenType::SEMICOLON){
        index = break_index;
        throw parse_break_st_error("");
    }
    return std::make_unique<BreakStatement>();
}

Parser::block_st_ptr Parser::parse_block_statement() {
    auto block_st_index = index;
    auto brace_left = this->tokens[index++];
    if (brace_left != TokenType::BRACE_LEFT){
        throw parse_block_st_error("");
    }
    if (this->tokens[index] == TokenType::BRACE_RIGHT){
        index++;
        return std::make_unique<BlockStatement>();
    }
    std::vector<Parser::statement_ptr> statements;
    while (this->tokens[index] != TokenType::BRACE_RIGHT && this->tokens[index] != TokenType::END){
        try {
            Parser::statement_ptr statement = parse_statement();
            statements.push_back(std::move(statement));
        } catch (const statement_parsing_error&) {
            throw parse_block_st_error("smth wrong with statements inside");
        }
    }
    return std::make_unique<BlockStatement>(statements);
}