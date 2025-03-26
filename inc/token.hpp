#pragma once
#include <string>

enum class TokenType {
    LITERAL, TYPE, PLUS, MINUS, MULTIPLY, DIVIDE, MODULO, POWER, ASSIGN, PLUS_ASSIGN, MINUS_ASSIGN, MULTIPLY_ASSIGN, DIVIDE_ASSIGN, MODULO_ASSIGN, RIGHT_SHIFT_ASSIGN, LEFT_SHIFT_ASSIGN, AND_ASSIGN, XOR_ASSIGN, OR_ASSIGN, EQUAL, NOT_EQUAL, GREATER, LESS, GREATER_EQUAL, LESS_EQUAL, NOT, AND, OR, QUESTION, BIT_AND, BIT_OR, BIT_XOR, BIT_NOT, LEFT_SHIFT, RIGHT_SHIFT, INCREMENT, DECREMENT, INDEX_LEFT, INDEX_RIGHT, DOT, ARROW, COMMA, SEMICOLON, BRACE_LEFT, BRACE_RIGHT, PUNCTUATOR, ID, KEYWORD, END
};

struct Token {
    TokenType type;
    std::string value;

    bool operator == (const Token& other) {
        return *this == other.type;
    }
    
    bool operator== (const TokenType type){
        return this->type == type;
    }

    bool operator != (const TokenType type) {
        return !(*this == type);
    }

    Token(const Token& other) : type(other.type), value(other.value) {};

    Token(Token&& other) : type(type), value(std::move(value)) {};

    Token(TokenType type) : type(type) {};

    Token(TokenType type, std::string value) : type(type), value(value) {};
};