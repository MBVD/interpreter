#include<bits/stdc++.h>
#include"lexer.hpp"
#include"token.hpp"

int main(){
    Lexer lexer = Lexer("int a = \"c\"");
    std::vector<Token>tokens = lexer();
    for (auto i : tokens){
        std::cout<<"["<<i.value<<"]";
    }
}
