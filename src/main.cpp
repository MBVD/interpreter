#include<bits/stdc++.h>
#include "lexer.hpp"
#include "token.hpp"
#include "parser.hpp"

int main(){
    Lexer lexer = Lexer("int a = \"c\" ;");
    std::vector<Token>tokens = lexer();
    for (auto i : tokens){
        std::cout<<"["<<i.value<<"]";
    }

    std::cout<<"start parsing ... \n";
    Parser parser = Parser(tokens);
    parser.parse();
}
