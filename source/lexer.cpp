#include "lexer.hpp"
#include "token.hpp"
#include "exceptions.hpp"
#include<iostream>

std::set<std::string> Lexer::types = {"int", "float", "double", "char", "bool", "size_t"};
std::unordered_map<std::string, TokenType> Lexer::operators = {{"+", TokenType::LOPERATOR},
                                                        {"-", TokenType::LOPERATOR},
                                                        {"*", TokenType::LOPERATOR},
                                                        {"/", TokenType::LOPERATOR},
                                                        {"%", TokenType::LOPERATOR},
                                                        {"^^", TokenType::ROPERATOR},
                                                        {"=", TokenType::ROPERATOR},
                                                        {"+=", TokenType::ROPERATOR},
                                                        {"-=", TokenType::ROPERATOR},
                                                        {"*=", TokenType::ROPERATOR},
                                                        {"/=", TokenType::ROPERATOR},
                                                        {"%=", TokenType::ROPERATOR},
                                                        {">>=", TokenType::ROPERATOR},
                                                        {"<<=", TokenType::ROPERATOR},
                                                        {"&=", TokenType::ROPERATOR},
                                                        {"^=", TokenType::ROPERATOR},
                                                        {"|=", TokenType::ROPERATOR},
                                                        {"==", TokenType::ROPERATOR},
                                                        {"!=", TokenType::ROPERATOR},
                                                        {">", TokenType::ROPERATOR},
                                                        {"<", TokenType::ROPERATOR},
                                                        {">=", TokenType::ROPERATOR},
                                                        {"<=", TokenType::ROPERATOR},
                                                        {"!", TokenType::LOPERATOR},
                                                        {"&&", TokenType::LOPERATOR},
                                                        {"||", TokenType::LOPERATOR},
                                                        {"?", TokenType::LOPERATOR},
                                                        {"&", TokenType::LOPERATOR},
                                                        {"|", TokenType::LOPERATOR},
                                                        {"^", TokenType::LOPERATOR},
                                                        {"~", TokenType::LOPERATOR},
                                                        {"<<", TokenType::ROPERATOR},
                                                        {">>", TokenType::LOPERATOR},
                                                        {"++", TokenType::LOPERATOR},
                                                        {"--", TokenType::LOPERATOR},
                                                        {"[]", TokenType::ROPERATOR},
                                                        {".", TokenType::LOPERATOR},
                                                        {"->", TokenType::LOPERATOR},};
std::set<std::string> Lexer::punctuators = {",", ".", ";"};
std::set<std::string> Lexer::keywords = {"if", "else", "for", "while", "struct", "break", "continue", "const", "do", "false", "true", "return"};
std::string Lexer::spec_symbols = "()-=+*&-><%^[]?";

std::vector<Token> Lexer::operator() (){
    std::vector<Token> tokens;

    while (index < input.size()) {
        Token token = extract();
        std::cout<<"TOKEN \n"<<token.value<<"\n";
        tokens.push_back(token);
        std::cout<<"extracted token "<< index<<"\n";
    }
    tokens.push_back({TokenType::END, ""});
    return tokens;
}

Token Lexer::extract(){
    while(std::isspace(input[index]) || input[index] == '\n') {++index;}
    
    if (index >= input.size()){
        return {TokenType::END, ""};
    }

    try {
        return extract_literal();
    } catch (const erroneous_extract_exception&) {}

    try {
        std::cout<<"ENTER IN EXTRACT TYPE IN "<<index<<"\n";
        return extract_type();
    } catch (const erroneous_extract_exception&) {}

    try {
        return extract_operator();
    } catch (const erroneous_extract_exception&) {}

    try {
        return extract_puctuator();
    } catch (const erroneous_extract_exception&) {}

    try {
        extract_keyword();
    } catch (const erroneous_extract_exception&) {}

    try {
        return extract_id();
    } catch (const erroneous_extract_exception&) {}
    
    return {TokenType::END, ""};
};

Token Lexer::extract_literal() {
    if (std::isdigit(input[index])){
        std::size_t size = 0;

        while (std::isdigit(input[index + size])) ++size;

        if (input[index + size] == '.') {
            ++size;
            while (std::isdigit(input[index + size])) ++size;
        }

        std::string value(input, index, size);
        index += size;
        return {TokenType::LITERAL, value};
    }

    if (input[index] == '\'') {
        if (index + 2 >= input.size() || input[index + 2] != '\''){
            throw syntax_error_exception("unclosed \' for char");
        }
        int prev = index;
        index += 3;
        return {TokenType::LITERAL, std::string {input, prev, 3}};
    }

    if (input[index] == '\"'){
        int tmp = index + 1;
        while (input[tmp] != '\"'){
            if (tmp + 1>= input.size()){
                throw syntax_error_exception("unclosed \" for string");
            }
            tmp++;
        }
        int size = tmp - index + 1;
        int prev = index;
        index += tmp + 1;
        return {TokenType::LITERAL, std::string {input, prev, size}};
    }

    throw erroneous_extract_exception("literal");
}

Token Lexer::extract_type() {
    for (auto type : types){
        if (input.find(type, index) == index){
            int prev = index;
            index += type.size();
            return {TokenType::TYPE, std::string{input, prev, type.size()}};
        }
    }
    throw erroneous_extract_exception("type");
}

Token Lexer::extract_puctuator() {
    if (punctuators.contains({input[index]})){
        return {TokenType::PUCTUATOR, {input[index++]}};
    }
    throw erroneous_extract_exception("puctuator");
}

Token Lexer::extract_keyword() {
    for (auto word : keywords){
        if (input.find(word, index) == index){
            int prev = index;
            index += word.size();

            return {TokenType::KEYWORD, std::string{input, prev, word.size()}};
        }
    }
    throw erroneous_extract_exception("keyword");
}

Token Lexer::extract_operator(){
    for (auto opearator_data : operators){
        if (input.find(opearator_data.first, index) == index){
            int prev = index;
            index += opearator_data.first.size();
            return {opearator_data.second, std::string{input, prev, opearator_data.first.size()}};
        }
    }
    throw erroneous_extract_exception("operator");
}

Token Lexer::extract_id(){
    int tmp = index;
    while (spec_symbols.find(input[tmp]) != std::string::npos && !std::isspace(input[tmp])){++tmp;}
    int size = tmp - index + 1;
    int prev = index;
    index += size;
    if (tmp == index) {throw erroneous_extract_exception("id");}
    return {TokenType::ID, {input, prev, size}};
}









