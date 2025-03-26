#pragma once
#include <stdexcept>
#include <string>

class erroneous_extract_exception : std::exception{
public:
    explicit erroneous_extract_exception(const char* message) : _msg(message) {};
    explicit erroneous_extract_exception(std::string message) : _msg(message) {};
    virtual const char* what() const noexcept override;
private:
    std::string _msg;
};

class syntax_error_exception : std::exception {
public:
    explicit syntax_error_exception(const char* message) : _msg(message) {};
    explicit syntax_error_exception(std::string message) : _msg(message) {};
    virtual const char* what() const noexcept override;
private:
    std::string _msg;
};




class parsing_errors : std::exception {
public:
    explicit parsing_errors(const char* message) : _msg(message) {};
    explicit parsing_errors(std::string message) : _msg(message) {};
    virtual const char* what() const noexcept override;
private:
    std::string _msg;
};

class declaration_parsing_error : parsing_errors{
public:
    explicit declaration_parsing_error(const char* message) : parsing_errors(message) {};
    explicit declaration_parsing_error(std::string message) : parsing_errors(message) {};
    virtual const char* what() const noexcept override;
};

class parse_var_decl_error : declaration_parsing_error {
public:
    explicit parse_var_decl_error(const char* message) : declaration_parsing_error(message) {};
    explicit parse_var_decl_error(std::string message) : declaration_parsing_error(message) {};
    virtual const char* what() const noexcept override;
};

class parse_func_decl_error : declaration_parsing_error {
public:
    explicit parse_func_decl_error(const char* message) : declaration_parsing_error(message) {};
    explicit parse_func_decl_error(std::string message) : declaration_parsing_error(message) {};
    virtual const char* what() const noexcept override;
};

class parse_struct_decl_error : declaration_parsing_error {
public:
    explicit parse_struct_decl_error(const char* message) : declaration_parsing_error(message) {};
    explicit parse_struct_decl_error(std::string message) : declaration_parsing_error(message) {};
    virtual const char* what() const noexcept override;
};

