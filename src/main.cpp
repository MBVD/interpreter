#include<bits/stdc++.h>
#include "lexer.hpp"
#include "token.hpp"
#include "parser.hpp"

int main(){
    std::string source_code((std::istreambuf_iterator<char>(std::cin)),
                            std::istreambuf_iterator<char>());
    std::cout<<source_code<<"\n";
    Lexer lexer = Lexer(source_code);
    std::vector<Token>tokens = lexer();
    for (auto i : tokens){
        std::cout<<"["<<i.value<<"]";
    }

    std::cout<<"\n Start parsing ... \n";
    Parser parser = Parser(tokens);
    parser.parse();
}
