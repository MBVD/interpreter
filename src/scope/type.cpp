#include "type.hpp"
#include <memory>
#include "declaration.hpp"
#include <vector>
#include <unordered_map>
#include <string>
#include <any>
#include <memory>
#include <iostream>

void Type::print() {
    std::cout << "Type\n";
}
// Arithmetic
Arithmetic::Arithmetic(std::any value) : value(value) {}

std::any Arithmetic::get_any_value() {
    return this->value;
}

int getTypeRank(std::shared_ptr<Type> type) {
    if (dynamic_cast<const FloatType*>(type.get())) return 2;
    if (dynamic_cast<const IntegerType*>(type.get())) return 1;
    if (dynamic_cast<const CharType*>(type.get())) return 0;
    if (dynamic_cast<const BoolType*>(type.get())) return -1;
    return -2;
}

std::shared_ptr<Type> compare_types(std::shared_ptr<Type> left, std::shared_ptr<Type> right) {
    int left_rank = getTypeRank(left);
    int right_rank = getTypeRank(right);
    
    if (left_rank >= right_rank) {
        return left;
    } else {
        return right;
    }
}

// Integral
Integral::Integral(std::any value = 0) : Arithmetic(value){}

// BoolType
BoolType::BoolType() : Integral(value) {}

void BoolType::print() {
    std::cout << "BoolType\n";
}

// CharType
CharType::CharType() : Integral(value) {}

void CharType::print() {
    std::cout << "CharType\n";
}

// IntegerType
IntegerType::IntegerType() : Integral(value) {}

void IntegerType::print() {
    std::cout << "IntegerType\n";
}

// FloatType
FloatType::FloatType() : Arithmetic(value) {}

void FloatType::print() {
    std::cout << "FloatType\n";
}

// FuncType
FuncType::FuncType(std::shared_ptr<Type> returnable_type, std::vector<std::shared_ptr<Type>> args, const std::unique_ptr<BlockStatement>& block) : returnable_type(returnable_type), args(args), block(block) {}

std::shared_ptr<Type> FuncType::get_returnable_type() const {
    return this->returnable_type;
}

std::vector<std::shared_ptr<Type>> FuncType::get_args() const {
    return this->args;
}

void FuncType::print() {
    std::cout << "FuncType with returnable type: ";
    if (returnable_type) {
        returnable_type->print();
    } else {
        std::cout << "nullptr\n";
    }
    std::cout << "Args: ";
    for (const auto& arg : args) {
        if (arg) {
            arg->print();
        } else {
            std::cout << "nullptr\n";
        }
    }
}

// StructType
StructType::StructType(const std::unordered_map<std::string, std::shared_ptr<Type>> members) : members(members) {}

std::unordered_map<std::string, std::shared_ptr<Type>> StructType::get_members() const {
    return this->members;
}

void StructType::print() {
    std::cout << "FloatType\n";
}

// PointerType
PointerType::PointerType(std::shared_ptr<Type> base) : base(base) {}

std::shared_ptr<Type> PointerType::get_base() const {
    return base;
}

int PointerType::get_star_count() {
    int count = 0;
    auto current = base;
    while (current) {
        if (auto pointer = dynamic_cast<PointerType*>(current.get())) {
            count++;
            current = pointer->get_base();
        } else {
            break;
        }
    }
    return count;
}

std::shared_ptr<Type> PointerType::get_type_by_star_count(int star_count) {
    auto current = base;
    for (int i = 0; i < star_count; ++i) {
        if (auto pointer = dynamic_cast<PointerType*>(current.get())) {
            current = pointer->get_base();
        } else {
            throw std::runtime_error("Invalid pointer dereference");
        }
    }
    return current;
}

void PointerType::print() {
    std::cout << "PointerType to ";
    if (base) {
        base->print();
    } else {
        std::cout << "nullptr\n";
    }
}

ArrayType::ArrayType(std::shared_ptr<Type> base) : PointerType(base) {}

void ArrayType::print() {
    std::cout << "ArrayType of ";
    if (get_base()) {
        get_base()->print();
    } else {
        std::cout << "nullptr\n";
    }
}