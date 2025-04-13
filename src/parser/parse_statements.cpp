#include "exceptions.hpp"
#include "parser.hpp"
#include <iostream>

// ExpressionStatement
Parser::expr_st_ptr Parser::parse_expression_stetement() {
    auto expr_st_index = index;
    try {
        auto expr = parse_expression();
        if (this->tokens[index] != TokenType::SEMICOLON) {
            throw parse_expression_st_error("");
        }
        index++;
        return std::make_unique<ExpressionStatement>(std::move(expr));
    } catch (expression_parsing_error&) {
        index = expr_st_index;
        throw parse_expression_st_error("");
    }
}

// DeclarationStatement
Parser::decl_st_ptr Parser::parse_decl_statement() {
    auto decl_st_index = index;
    try {
        auto var_decl = parse_var_declaration();
        return std::make_unique<DeclarationStatement>(std::move(var_decl));
    } catch (parse_var_decl_error&) {
        index = decl_st_index;
        throw parse_decl_st_error("");
    }
}

// ConditionalStatement
Parser::cond_st_ptr Parser::parse_conditional_statement() {
    auto cond_index = index;
    if (this->tokens[index++] != TokenType::IF) {
        index = cond_index;
        throw parse_conditional_st_error("");
    }
    if (this->tokens[index++] != TokenType::PARENTHESIS_LEFT) {
        index = cond_index;
        throw parse_conditional_st_error("Expected '(' after 'if'");
    }

    Parser::expr_ptr condition;
    try {
        condition = parse_expression();
    } catch (expression_parsing_error&) {
        index = cond_index;
        throw parse_conditional_st_error("Error parsing condition");
    }

    if (this->tokens[index++] != TokenType::PARENTHESIS_RIGHT) {
        index = cond_index;
        throw parse_conditional_st_error("Expected ')' after condition");
    }

    Parser::statement_ptr true_statement;
    try {
        true_statement = parse_block_statement();
    } catch (statement_parsing_error&) {
        index = cond_index;
        throw parse_conditional_st_error("Error parsing true statement");
    }

    if (this->tokens[index] == TokenType::ELSE) {
        index++;
        Parser::statement_ptr else_statement;
        try {
            else_statement = parse_block_statement();
            return std::make_unique<ConditionalStatement>(std::move(condition), std::move(true_statement), std::move(else_statement));
        } catch (statement_parsing_error&) {
            index = cond_index;
            throw parse_conditional_st_error("Error parsing else statement");
        }
    }

    return std::make_unique<ConditionalStatement>(std::move(condition), std::move(true_statement));
}

// LoopStatement
Parser::loop_st_ptr Parser::parse_loop_statement() {
    auto loop_index = index;
    try {
        return parse_while_statement();
    } catch (parse_loop_st_error&) {}
    try {
        return parse_do_while_statement();
    } catch (parse_loop_st_error&) {}
    try {
        return parse_for_statement();
    } catch (parse_loop_st_error&) {}
    index = loop_index;
    throw parse_loop_st_error("Error parsing loop statement");
}

// WhileStatement
Parser::while_st_ptr Parser::parse_while_statement() {
    auto while_index = index;
    if (this->tokens[index++] != TokenType::WHILE) {
        index = while_index;
        throw parse_while_statement_error("");
    }
    if (this->tokens[index++] != TokenType::PARENTHESIS_LEFT) {
        throw parse_while_statement_error("Expected '(' after 'while'");
    }

    Parser::expr_ptr condition;
    try {
        condition = parse_expression();
    } catch (expression_parsing_error&) {
        throw parse_while_statement_error("Error parsing condition");
    }

    if (this->tokens[index++] != TokenType::PARENTHESIS_RIGHT) {
        throw parse_while_statement_error("Expected ')' after condition");
    }

    Parser::statement_ptr statement;
    try {
        statement = parse_block_statement();
    } catch (statement_parsing_error&) {
        throw parse_while_statement_error("Error parsing statement");
    }

    return std::make_unique<WhileStatement>(std::move(condition), std::move(statement));
}

// DoWhileStatement
Parser::do_while_st_ptr Parser::parse_do_while_statement() {
    auto do_while_index = index;
    if (this->tokens[index++] != TokenType::DO) {
        index = do_while_index;
        throw parse_do_while_statement_error("");
    }

    Parser::statement_ptr statement;
    try {
        statement = parse_statement();
    } catch (statement_parsing_error&) {
        throw parse_do_while_statement_error("Error parsing statement");
    }

    if (this->tokens[index++] != TokenType::WHILE) {
        index = do_while_index;
        throw parse_do_while_statement_error("Expected 'while' after statement");
    }
    if (this->tokens[index++] != TokenType::PARENTHESIS_LEFT) {
        index = do_while_index;
        throw parse_do_while_statement_error("Expected '(' after 'while'");
    }

    Parser::expr_ptr condition;
    try {
        condition = parse_expression();
    } catch (expression_parsing_error&) {
        throw parse_do_while_statement_error("Error parsing condition");
    }

    if (this->tokens[index++] != TokenType::PARENTHESIS_RIGHT) {
        index = do_while_index;
        throw parse_do_while_statement_error("Expected ')' after condition");
    }
    if (this->tokens[index++] != TokenType::SEMICOLON) {
        index = do_while_index;
        throw parse_do_while_statement_error("Expected ';' after do-while statement");
    }

    return std::make_unique<DoWhileStatement>(std::move(statement), std::move(condition));
}

// ForStatement
Parser::for_st_prt Parser::parse_for_statement() {
    auto for_index = index;
    if (this->tokens[index++] != TokenType::FOR) {
        index = for_index;
        throw parse_for_statement_error("");
    }
    if (this->tokens[index++] != TokenType::PARENTHESIS_LEFT) {
        index = for_index;
        throw parse_for_statement_error("Expected '(' after 'for'");
    }

    bool is_var = true;
    Parser::var_ptr var;
    Parser::expr_ptr init_expression;
    try {
        var = parse_var_declaration();
    } catch (parse_var_decl_error&) {
        is_var = false;
        try {
            init_expression = parse_expression();
        } catch (expression_parsing_error&) {
            throw parse_for_statement_error("Error parsing initialization");
        }
    }

    Parser::expr_ptr condition;
    try {
        condition = parse_expression();
    } catch (expression_parsing_error&) {
        throw parse_for_statement_error("Error parsing condition");
    }

    if (this->tokens[index++] != TokenType::SEMICOLON) {
        throw parse_for_statement_error("Expected ';' after condition");
    }

    Parser::expr_ptr iteration;
    try {
        iteration = parse_expression();
    } catch (expression_parsing_error&) {
        throw parse_for_statement_error("Error parsing iteration");
    }

    if (this->tokens[index++] != TokenType::PARENTHESIS_RIGHT) {
        index = for_index;
        throw parse_for_statement_error("Expected ')' after iteration");
    }

    Parser::statement_ptr body;
    try {
        body = parse_block_statement();
    } catch (statement_parsing_error&) {
        throw parse_for_statement_error("Error parsing body");
    }

    return is_var ? std::make_unique<ForStatement>(std::move(var), std::move(condition), std::move(iteration), std::move(body))
                  : std::make_unique<ForStatement>(std::move(init_expression), std::move(condition), std::move(iteration), std::move(body));
}

// ReturnStatement
Parser::return_st_ptr Parser::parse_return_statement() {
    auto return_index = index;
    if (this->tokens[index++] != TokenType::RETURN) {
        index = return_index;
        throw parse_return_st_error("");
    }

    Parser::expr_ptr expression;
    try {
        expression = parse_expression();
    } catch (expression_parsing_error&) {}

    if (this->tokens[index++] != TokenType::SEMICOLON) {
        index = return_index;
        throw parse_return_st_error("Expected ';' after return statement");
    }

    return std::make_unique<ReturnStatement>(std::move(expression));
}

// ContinueStatement
Parser::cont_st_ptr Parser::parse_continue_statement() {
    auto cont_index = index;
    if (this->tokens[index++] != TokenType::CONTINUE) {
        index = cont_index;
        throw parse_continue_st_error("");
    }
    if (this->tokens[index++] != TokenType::SEMICOLON) {
        index = cont_index;
        throw parse_continue_st_error("Expected ';' after continue statement");
    }
    return std::make_unique<ContinueStatement>();
}

// BreakStatement
Parser::break_st_ptr Parser::parse_break_statement() {
    auto break_index = index;
    if (this->tokens[index++] != TokenType::BREAK) {
        index = break_index;
        throw parse_break_st_error("");
    }
    if (this->tokens[index++] != TokenType::SEMICOLON) {
        index = break_index;
        throw parse_break_st_error("Expected ';' after break statement");
    }
    return std::make_unique<BreakStatement>();
}

// BlockStatement
Parser::block_st_ptr Parser::parse_block_statement() {
    auto block_st_index = index;
    if (this->tokens[index++] != TokenType::BRACE_LEFT) {
        index = block_st_index;
        throw parse_block_st_error("");
    }

    if (this->tokens[index] == TokenType::BRACE_RIGHT) {
        index++;
        return std::make_unique<BlockStatement>();
    }

    std::vector<Parser::statement_ptr> statements;
    while (this->tokens[index] != TokenType::BRACE_RIGHT && this->tokens[index] != TokenType::END) {
        try {
            statements.push_back(parse_statement());
        } catch (statement_parsing_error&) {
            throw parse_block_st_error("Error parsing statements inside block");
        }
    }

    if (this->tokens[index++] != TokenType::BRACE_RIGHT) {
        index = block_st_index;
        throw parse_block_st_error("Expected '}' at the end of block");
    }

    return std::make_unique<BlockStatement>(std::move(statements));
}

Parser::empty_st_ptr Parser::parse_empty_statement(){
    if (this->tokens[index] != TokenType::SEMICOLON){
        throw statement_parsing_error("");
    } else {
        index ++;
        return std::make_unique<EmptyStatement>();
    }
}