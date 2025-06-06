#pragma once
#include "declaration.hpp"
#include <vector>
#include <unordered_map>
#include <string>
#include <any>
#include <memory>

// Базовый класс для всех типов
class Type {
public:
    virtual ~Type() = default;
    virtual void print();
    virtual bool is_arithmetic() const {
        return false;
    };
    virtual bool is_integral() const {
        return false;
    }
    virtual bool is_integer() const {
        return false;
    }
    virtual bool is_bool() const {
        return false;
    }
    virtual bool is_char() const {
        return false;
    }
    virtual bool is_floating() const {
        return false;
    }
    virtual bool is_composite() const {
        return false;
    }
    virtual bool is_fundamental() const {
        return false;
    }
    virtual bool is_pointer() const {
        return false;
    }
    virtual bool is_record() const {
        return false;
    }
    virtual bool is_enum() const {
        return false;
    }
    virtual bool is_array() const {
        return false;
    }
    virtual bool is_void() const {
        return false;
    }
    virtual bool is_nullptr() const {
        return false;
    }
};

// Фундаментальные типы
class Fundamental : public Type {
};

class VoidType : public Fundamental {
};

class NullPtrType : public Fundamental {
};

class Arithmetic : public Fundamental {
public:
    Arithmetic(std::any);
    std::any get_any_value();
    bool is_arithmetic() const override {
        return true;
    }
private:
    std::any value;
};

class Integral : public Arithmetic {
public:
    Integral(std::any);
    bool is_integral() const override {
        return true;
    }
};

// Все целочисленные типы
class BoolType : public Integral {
public:
    explicit BoolType();
    void print();
    bool is_bool() const override {
        return true;
    }
private:
    bool value;
};

class CharType : public Integral {
public:
    explicit CharType();
    void print();
    bool is_char() const override {
        return true;
    }
private:
    char16_t value;
};

class IntegerType : public Integral {
public:
    explicit IntegerType();
    void print();
    bool is_integer() const override {
        return true;
    }
private:
    int8_t value;
};

// Все типы с плавающей точкой
class FloatType : public Arithmetic {
public:
    FloatType();
    void print();
    bool is_floating() const override {
        return true;
    }
private:
    double value;
};

// Составные типы
class Composite : public Type {
};

// Тип функции
class FuncType : public Composite {
public:
    FuncType(std::shared_ptr<Type>, std::vector<std::shared_ptr<Type>>);
    std::shared_ptr<Type> get_returnable_type() const;
    std::vector<std::shared_ptr<Type>> get_args() const;
    void print();
private:
    std::shared_ptr<Type> returnable_type;
    std::vector<std::shared_ptr<Type>> args;
};

// Тип записи
class RecordType : public Composite {
};

class Symbol;
// Все типы записей
class StructType : public RecordType {
public:
    explicit StructType(const std::unordered_map<std::string, std::shared_ptr<Type>>);
    std::unordered_map<std::string, std::shared_ptr<Type>> get_members() const;
    void print();
private:
    std::unordered_map<std::string, std::shared_ptr<Type>> members;
    std::weak_ptr<Symbol> symbol;
};

class ClassType : public RecordType {
};

class Union : public RecordType {
};

// Перечисления
class EnumType : public Composite {
};

// Указатели
class PointerType : public Composite {
public:
    explicit PointerType(std::shared_ptr<Type> base);
    std::shared_ptr<Type> get_base() const;
    int get_star_count();
    std::shared_ptr<Type> get_type_by_star_count(int x);
    void print();
    bool is_array() const {
        return true;
    }
private:
    std::shared_ptr<Type> base;
};

// Ссылки
class RefType : public Composite {
};

class RValueType : public RefType {
};

class LValueType : public RefType {
};

// Массивы
class ArrayType : public PointerType {
public:
    ArrayType(std::shared_ptr<Type> base);
    ArrayType(std::shared_ptr<Type> base, std::vector<std::shared_ptr<Symbol>>); 
    void print();
    std::vector<std::shared_ptr<Symbol>> get_elements() const {
        return elements;
    }
    std::shared_ptr<Symbol> get_by_ids(std::vector<int>&, int);
    bool is_array() const {
        return true;
    }
private:
    std::vector<std::shared_ptr<Symbol>> elements;
};

std::shared_ptr<Type> compare_types(std::shared_ptr<Type> left, std::shared_ptr<Type>right);
int getTypeRank(std::shared_ptr<Type> type);