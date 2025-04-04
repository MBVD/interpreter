#include<bits/stdc++.h>
#include "lexer.hpp"
#include "token.hpp"
#include "parser.hpp"
#include "printer.hpp"

void test_binary_expression_printer() {
    // Создаём фиктивные токены
    Token left_token(TokenType::LITERAL_NUM, "5");
    Token op_token(TokenType::PLUS, "+");
    Token right_token(TokenType::LITERAL_NUM, "3");

    // Создаём бинарное выражение
    auto binary_expression = std::make_unique<BinaryExpression>(
        std::make_unique<LiteralExpression>(left_token), op_token, std::make_unique<LiteralExpression>(right_token));

    // Создаём объект Printer
    Printer printer;

    // Печатаем бинарное выражение
    std::cout << "BinaryExpression Output:\n";
    printer.print(binary_expression.get());
}

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
    test_binary_expression_printer();
    parser.parse();
}
