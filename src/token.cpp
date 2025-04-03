#include <iostream>
#include "token.hpp"

std::ostream& operator<< (std::ostream& out, const Token& token) {
    out<<token.value;
    return out;
}