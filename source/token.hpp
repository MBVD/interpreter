#pragma once
#include <string>

enum class TokenType {
    LITERAL, TYPE, LOPERATOR, ROPERATOR, PUCTUATOR, ID, KEYWORD, END
};

struct Token {
    TokenType type;
    std::string value;
    
    bool operator== (const TokenType type){
        return this->type == type;
    }

    Token(TokenType type, std::string value) : type(type), value(value) {};
};