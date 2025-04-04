#include<bits/stdc++.h>
#include "lexer.hpp"
#include "token.hpp"
#include "parser.hpp"
#include "printer.hpp"

void test_binary_expression_printer() {
    Token left_token(TokenType::LITERAL_NUM, "5");
    Token op_token(TokenType::PLUS, "+");
    Token right_token(TokenType::LITERAL_NUM, "3");

    auto binary_expression = std::make_unique<BinaryExpression>(
        std::make_unique<LiteralExpression>(left_token), op_token, std::make_unique<LiteralExpression>(right_token));
    
        Printer printer;
    printer.print(binary_expression.get());
}

int main(){
    // test_binary_expression_printer();
    // std::cout<<"\n";
    // std::string source_code((std::istreambuf_iterator<char>(std::cin)),
    //                         std::istreambuf_iterator<char>());
    std::string source_code = "int x = 1;";
    std::cout<<source_code<<"\n";
    Lexer lexer = Lexer(source_code);
    std::vector<Token>tokens = lexer();
    for (auto i : tokens){
        std::cout<<"["<<i<<"] ";
    }
    Parser parser = Parser(tokens);
    auto root = parser.parse();
    Printer printer;
    printer.print(root.get());
}
