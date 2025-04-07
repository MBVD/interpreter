#pragma once
#include <stdexcept>
#include <string>

class erroneous_extract_exception : std::exception{
public:
    explicit erroneous_extract_exception(const char* message) : _msg(message) {};
    explicit erroneous_extract_exception(std::string message) : _msg(message) {};
    virtual const char* what() const noexcept override;
    std::string _msg;
};

class syntax_error_exception : std::exception {
public:
    explicit syntax_error_exception(const char* message) : _msg(message) {};
    explicit syntax_error_exception(std::string message) : _msg(message) {};
    virtual const char* what() const noexcept override;
    std::string _msg;
};




class parsing_errors : std::exception {
public:
    explicit parsing_errors(const char* message) : _msg(message) {};
    explicit parsing_errors(std::string message) : _msg(message) {};
    virtual const char* what() const noexcept override;
    std::string _msg;
};

class declaration_parsing_error : public parsing_errors{
public:
    explicit declaration_parsing_error(const char* message) : parsing_errors(message) {};
    explicit declaration_parsing_error(std::string message) : parsing_errors(message) {};
    virtual const char* what() const noexcept override;
};

class parse_var_decl_error : public declaration_parsing_error {
public:
    explicit parse_var_decl_error(const char* message) : declaration_parsing_error(message) {};
    explicit parse_var_decl_error(std::string message) : declaration_parsing_error(message) {};
    virtual const char* what() const noexcept override;
};

class parse_init_decl_error : public parse_var_decl_error {
public:
    explicit parse_init_decl_error(const char* message) : parse_var_decl_error(message) {};
    explicit parse_init_decl_error(std::string message) : parse_var_decl_error(message) {};
    virtual const char* what() const noexcept override;
};

class parse_id_decl_error : public parse_var_decl_error {
public:
    explicit parse_id_decl_error(const char* message) : parse_var_decl_error(message) {};
    explicit parse_id_decl_error(std::string message) : parse_var_decl_error(message) {};
    virtual const char* what() const noexcept override;
};

class parse_func_decl_error : public declaration_parsing_error {
public:
    explicit parse_func_decl_error(const char* message) : declaration_parsing_error(message) {};
    explicit parse_func_decl_error(std::string message) : declaration_parsing_error(message) {};
    virtual const char* what() const noexcept override;
};


class parse_param_decl_error : public parse_func_decl_error{
public:
    explicit parse_param_decl_error(const char* message) : parse_func_decl_error(message) {};
    explicit parse_param_decl_error(std::string message) : parse_func_decl_error(message) {};
    virtual const char* what() const noexcept override;
};

class parse_struct_decl_error : public declaration_parsing_error {
public:
    explicit parse_struct_decl_error(const char* message) : declaration_parsing_error(message) {};
    explicit parse_struct_decl_error(std::string message) : declaration_parsing_error(message) {};
    virtual const char* what() const noexcept override;
};




///


class expression_parsing_error : public parsing_errors{
public:
    explicit expression_parsing_error(const char* message) : parsing_errors(message) {};
    explicit expression_parsing_error(std::string message) : parsing_errors(message) {};
    virtual const char* what() const noexcept override;
};

class parse_comparison_expr_error : public expression_parsing_error {
public:
    explicit parse_comparison_expr_error(const char* message) : expression_parsing_error(message) {};
    explicit parse_comparison_expr_error(std::string message) : expression_parsing_error(message) {};
    virtual const char* what() const noexcept override;
};

class parse_binary_expr_error : public expression_parsing_error {
public:
    explicit parse_binary_expr_error(const char* message) : expression_parsing_error(message) {};
    explicit parse_binary_expr_error(std::string message) : expression_parsing_error(message) {};
    virtual const char* what() const noexcept override;
};


class parse_ternary_expr_error : public expression_parsing_error {
public:
    explicit parse_ternary_expr_error(const char* message) : expression_parsing_error(message) {};
    explicit parse_ternary_expr_error(std::string message) : expression_parsing_error(message) {};
    virtual const char* what() const noexcept override;
};


class parse_base_expr_error : public parse_binary_expr_error {
public:
    explicit parse_base_expr_error(const char* message) : parse_binary_expr_error(message) {};
    explicit parse_base_expr_error(std::string message) : parse_binary_expr_error(message) {};
    virtual const char* what() const noexcept override;
};
















////
class statement_parsing_error : public parsing_errors {
public:
    explicit statement_parsing_error(const char* message) : parsing_errors(message) {};
    explicit statement_parsing_error(std::string message) : parsing_errors(message) {};
    virtual const char* what() const noexcept override;
};

class parse_expression_st_error : public statement_parsing_error {
public:
    explicit parse_expression_st_error(const char* message) : statement_parsing_error(message) {};
    explicit parse_expression_st_error(std::string message) : statement_parsing_error(message) {};
    virtual const char* what() const noexcept override;
};

class parse_conditional_st_error : public statement_parsing_error {
public:
    explicit parse_conditional_st_error(const char* message) : statement_parsing_error(message) {};
    explicit parse_conditional_st_error(std::string message) : statement_parsing_error(message) {};
    virtual const char* what() const noexcept override;
};

class parse_loop_st_error : public statement_parsing_error {
public:
    explicit parse_loop_st_error(const char* message) : statement_parsing_error(message) {};
    explicit parse_loop_st_error(std::string message) : statement_parsing_error(message) {};
    virtual const char* what() const noexcept override;
};

class parse_while_statement_error : public parse_loop_st_error {
public:
    explicit parse_while_statement_error(const char* message) : parse_loop_st_error(message) {};
    explicit parse_while_statement_error(std::string message) : parse_loop_st_error(message) {};
    virtual const char* what() const noexcept override;
};

class parse_do_while_statement_error : public parse_loop_st_error {
public:
    explicit parse_do_while_statement_error(const char* message) : parse_loop_st_error(message) {};
    explicit parse_do_while_statement_error(std::string message) : parse_loop_st_error(message) {};
    virtual const char* what() const noexcept override;   
};

class parse_for_statement_error : public parse_loop_st_error {
public:
    explicit parse_for_statement_error(const char* message) : parse_loop_st_error(message) {};
    explicit parse_for_statement_error(std::string message) : parse_loop_st_error(message) {};
    virtual const char* what() const noexcept override;  
};

class parse_return_st_error : public statement_parsing_error {
public:
    explicit parse_return_st_error(const char* message) : statement_parsing_error(message) {};
    explicit parse_return_st_error(std::string message) : statement_parsing_error(message) {};
    virtual const char* what() const noexcept override;
};

class parse_continue_st_error : public statement_parsing_error {
public:
    explicit parse_continue_st_error(const char* message) : statement_parsing_error(message) {};
    explicit parse_continue_st_error(std::string message) : statement_parsing_error(message) {};
    virtual const char* what() const noexcept override;
};

class parse_break_st_error : public statement_parsing_error {
public:
    explicit parse_break_st_error(const char* message) : statement_parsing_error(message) {};
    explicit parse_break_st_error(std::string message) : statement_parsing_error(message) {};
    virtual const char* what() const noexcept override;
};
    
class parse_decl_st_error : public statement_parsing_error {
public:
    explicit parse_decl_st_error(const char* message) : statement_parsing_error(message) {};
    explicit parse_decl_st_error(std::string message) : statement_parsing_error(message) {};
    virtual const char* what() const noexcept override;
};

class parse_block_st_error : public statement_parsing_error {
public:
    explicit parse_block_st_error(const char* message) : statement_parsing_error(message) {};
    explicit parse_block_st_error(std::string message) : statement_parsing_error(message) {};
    virtual const char* what() const noexcept override;
};