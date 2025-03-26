#include "exceptions.hpp"

const char* erroneous_extract_exception::what() const noexcept {
    return ("erroneous_extract_errir: error to extract " + _msg).c_str();
}

const char* syntax_error_exception::what() const noexcept {
    return ("syntax_error_exception::error with " + _msg).c_str();
}