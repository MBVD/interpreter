#include "type.hpp"
#include <vector>
#include <unordered_map>
#include <string>
#include <any>
#include <memory>

Arithmetic::Arithmetic(std::any value = 0){
    this->value = value;
}

std::any Arithmetic::get_any_value(){
    return this->value;
}

Integral::Integral(std::any value) : Arithmetic(value){}

BoolType::BoolType(std::any value) : Integral(value) {
    this->value = std::any_cast<bool>(get_any_value());
}

CharType::CharType(std::any value) : Integral(value) {
    this->value = std::any_cast<char16_t>(get_any_value());
}

IntegerType::IntegerType(std::any value) : Integral(value) {
    this->value = std::any_cast<int8_t>(get_any_value());
}

FloatType::FloatType(std::any value) : Arithmetic(value) {
    this->value = std::any_cast<double>(get_any_value());
}

FuncType::FuncType(Type returnable_type, std::vector<Type> args) : returnable_type(returnable_type), args(args){}

Type FuncType::get_returnable_type() const {
    return this->returnable_type;
}

std::vector<Type> FuncType::get_args() const {
    return this->args;
}

StructType::StructType(const std::unordered_map<std::string, Type> members) : members(members) {};

std::unordered_map<std::string, Type> StructType::get_members() const {
    return this->members;
}

PointerType::PointerType(const Type* base = nullptr) {
    base = base;
}

Type& PointerType::get_base() const {
    return *base;
}


