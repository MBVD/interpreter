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
    explicit BoolType(std::any);
private:
    bool value;
};

class CharType : public Integral {
public:
    explicit CharType(std::any);
private:
    char16_t value;
};

class IntegerType : public Integral {
public:
    explicit IntegerType(std::any);
private:
    int8_t value;
};

// Все типы с плавающей точкой
class FloatType : public Arithmetic {
public:
    FloatType(std::any);
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
private:
    std::shared_ptr<Type> returnable_type;
    std::vector<std::shared_ptr<Type>> args;
};

// Тип записи
class Record : public Composite {
};

// Все типы записей
class StructType : public Record {
public:
    explicit StructType(const std::unordered_map<std::string, std::shared_ptr<Type>>);
    std::unordered_map<std::string, std::shared_ptr<Type>> get_members() const;
private:
    std::unordered_map<std::string, std::shared_ptr<Type>> members;
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
    explicit PointerType(std::shared_ptr<Type> base);
    std::shared_ptr<Type> get_base() const;
    int get_star_count();
    std::shared_ptr<Type> get_type_by_star_count(int x);
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
    ArrayType(const Type* base, int size); // Тип и размер массива
private:
    int size;
};

std::shared_ptr<Type> compare_types(std::shared_ptr<Type> left, std::shared_ptr<Type>right);