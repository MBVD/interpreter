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