#include "parser.hpp"
#include "exceptions.hpp"
#include <iostream>

using decl_ptr = std::shared_ptr<Declaration>;
decl_ptr Parser::parse_declaration() {
    try {
        return parse_var_declaration();
    } catch (const parse_var_decl_error&) {
        std::cout<<"failed to parse var_decl expr \n";
    }
    try {
        return parse_init_declaration();
    } catch (const parse_init_decl_error&) {
        std::cout<<"failed to parse init_decl expr \n";
    }
    
}

