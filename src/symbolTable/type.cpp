#include "type.hpp"
#include <vector>
#include <unordered_map>
#include <string>
#include <any>
#include <memory>

// Arithmetic
Arithmetic::Arithmetic(std::any value) : value(value) {}

std::any Arithmetic::get_any_value() {
    return this->value;
}

// Helper function to assign a rank to each type
int getTypeRank(const Type& type) {
    if (dynamic_cast<const IntegerType*>(&type)) return 1;
    if (dynamic_cast<const FloatType*>(&type)) return 2;
    if (dynamic_cast<const CharType*>(&type)) return 0;
    if (dynamic_cast<const BoolType*>(&type)) return -1;
    return -2; // Default rank for unknown types
}

// Integral
Integral::Integral(std::any value) : Arithmetic(value){}

// BoolType
BoolType::BoolType() : Integral(0) {}

// CharType
CharType::CharType() : Integral(0) {}

// IntegerType
IntegerType::IntegerType() : Integral(value) {}

// FloatType
FloatType::FloatType() : Arithmetic(value) {}

// FuncType
FuncType::FuncType(Type returnable_type, std::vector<Type> args) : returnable_type(returnable_type), args(args) {}

Type FuncType::get_returnable_type() const {
    return this->returnable_type;
}

std::vector<Type> FuncType::get_args() const {
    return this->args;
}

// StructType
StructType::StructType(const std::unordered_map<std::string, Type> members) : members(members) {}

std::unordered_map<std::string, Type> StructType::get_members() const {
    return this->members;
}

// PointerType
PointerType::PointerType(Type* base) : base(base) {}

Type* PointerType::get_base() const {
    return base;
}

int PointerType::get_star_count() {
    int count = 0;
    Type* current = base;
    while (current) {
        if (auto pointer = dynamic_cast<PointerType*>(current)) {
            count++;
            current = pointer->get_base();
        } else {
            break;
        }
    }
    return count;
}

Type PointerType::get_type_by_star_count(int star_count) {
    Type* current = base;
    for (int i = 0; i < star_count; ++i) {
        if (auto pointer = dynamic_cast<PointerType*>(current)) {
            current = pointer->get_base();
        } else {
            throw std::runtime_error("Invalid pointer dereference");
        }
    }
    return *current;
}