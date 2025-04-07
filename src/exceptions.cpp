#include "exceptions.hpp"

const char* erroneous_extract_exception::what() const noexcept {
    return ("erroneous_extract_errir: error to extract " + _msg).c_str();
}

const char* syntax_error_exception::what() const noexcept {
    return ("syntax_error_exception::error with " + _msg).c_str();
}

const char* parsing_errors::what() const noexcept {
    return ("syntax_error_exception::error with " + _msg).c_str();
}

const char* declaration_parsing_error::what() const noexcept {
    return ("syntax_error_exception::error with " + _msg).c_str();
}

const char* parse_var_decl_error::what() const noexcept {
    return ("syntax_error_exception::error with " + _msg).c_str();
}

const char* parse_init_decl_error::what() const noexcept {
    return ("syntax_error_exception::error with " + _msg).c_str();
}

const char* parse_id_decl_error::what() const noexcept {
    return ("syntax_error_exception::error with " + _msg).c_str();
}

const char* parse_func_decl_error::what() const noexcept {
    return ("syntax_error_exception::error with " + _msg).c_str();
}

const char* parse_param_decl_error::what() const noexcept {
    return ("syntax_error_exception::error with " + _msg).c_str();
}

const char* parse_struct_decl_error::what() const noexcept {
    return ("syntax_error_exception::error with " + _msg).c_str();
}

const char* expression_parsing_error::what() const noexcept {
    return ("syntax_error_exception::error with " + _msg).c_str();
}

const char* parse_comparison_expr_error::what() const noexcept {
    return ("parse_comparison_expr_error" + _msg).c_str();
}

const char* parse_binary_expr_error::what() const noexcept {
    return ("syntax_error_exception::error with " + _msg).c_str();
}

const char* parse_ternary_expr_error::what() const noexcept {
    return ("syntax_error_exception::error with " + _msg).c_str();
}

const char* parse_base_expr_error::what() const noexcept {
    return ("syntax_error_exception::error with " + _msg).c_str();
}







const char* statement_parsing_error::what() const noexcept {
    return ("syntax_error_exception::error with " + _msg).c_str();
}
const char* parse_expression_st_error::what() const noexcept {
    return ("syntax_error_exception::error with " + _msg).c_str();
}
const char* parse_conditional_st_error::what() const noexcept {
    return ("syntax_error_exception::error with " + _msg).c_str();
}
const char* parse_loop_st_error::what() const noexcept {
    return ("syntax_error_exception::error with " + _msg).c_str();
}
const char* parse_while_statement_error::what() const noexcept {
    return ("syntax_error_exception::error with " + _msg).c_str();
}
const char* parse_do_while_statement_error::what() const noexcept {
    return ("syntax_error_exception::error with " + _msg).c_str();
}
const char* parse_for_statement_error::what() const noexcept {
    return ("syntax_error_exception::error with " + _msg).c_str();
}
const char* parse_return_st_error::what() const noexcept {
    return ("syntax_error_exception::error with " + _msg).c_str();
}
const char* parse_continue_st_error::what() const noexcept {
    return ("syntax_error_exception::error with " + _msg).c_str();
}
const char* parse_break_st_error::what() const noexcept {
    return ("syntax_error_exception::error with " + _msg).c_str();
}
const char* parse_decl_st_error::what() const noexcept {
    return ("syntax_error_exception::error with " + _msg).c_str();
}
const char* parse_block_st_error::what() const noexcept {
    return ("syntax_error_exception::error with " + _msg).c_str();
}





