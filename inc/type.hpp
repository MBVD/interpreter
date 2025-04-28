#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <any>
#include <memory>

// Базовый класс для всех типов
class Type {
public:
    virtual ~Type() = default;
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
private:
    std::any value;
};

class Integral : public Arithmetic {
public:
    Integral(std::any);
};

// Все целочисленные типы
class BoolType : public Integral {
public:
    explicit BoolType(std::any value = 0);
private:
    bool value;
};

class CharType : public Integral {
public:
    explicit CharType(std::any value = 0);
private:
    char16_t value;
};

class IntegerType : public Integral {
public:
    explicit IntegerType(std::any value = 0);
private:
    int8_t value;
};

// Все типы с плавающей точкой
class FloatType : public Arithmetic {
public:
    explicit FloatType(std::any value = 0);
private:
    double value;
};

// Составные типы
class Composite : public Type {
};

// Тип функции
class FuncType : public Composite {
public:
    FuncType(Type, std::vector<Type>);
    Type get_returnable_type() const;
    std::vector<Type> get_args() const;
private:
    Type returnable_type;
    std::vector<Type> args;
};

// Тип записи
class Record : public Composite {
};

// Все типы записей
class StructType : public Record {
public:
    explicit StructType(const std::unordered_map<std::string, Type>);
    std::unordered_map<std::string, Type> get_members() const;
private:
    std::unordered_map<std::string, Type> members;
};

class ClassType : public Record {
};

class Union : public Record {
};

// Перечисления
class EnumType : public Composite {
};

// Указатели
class PointerType : public Composite {
public:
    explicit PointerType(const Type* base); // int* a;
    Type& get_base() const;
private:
    Type* base;
};

// Ссылки
class RefType : public Composite {
};

class RValueType : public RefType {
};

class LValueType : public RefType {
};

// Массивы
class ArrayType : public Composite {
public:
    ArrayType(const Type& base, int size); // Тип и размер массива
    const std::vector<Type>& get_array() const;
private:
    int size;
    std::vector<Type> array;
};