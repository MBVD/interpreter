#pragma once
#include <vector>
#include <unordered_map>
#include <set>
#include<functional>
#include "token.hpp"
class Lexer {
public:
    std::vector<Token> operator() ();
    Lexer(std::string input) : input(input) {};
private:
    std::string input;
    int index = 0;
    static std::set<std::string> types;
    static std::unordered_map<std::string, TokenType> operators;
    static std::unordered_map<std::string, TokenType> punctuators;
    static std::unordered_map<std::string, TokenType> keywords;
    static std::string spec_symbols;

    Token extract();
    Token extract_literal();
    Token extract_type();
    Token extract_operator();
    Token extract_puctuator();
    Token extract_id();
    Token extract_keyword();
};