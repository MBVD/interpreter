#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <any>
#include <memory>

// Базовый класс для всех типов
class Type {
public:
    virtual ~Type() = default; // Виртуальный деструктор для корректного удаления
};

// Фундаментальные типы
class Fundamental : public Type {
};

class VoidType : public Fundamental {
};

class NullPtrType : public Fundamental {
};

class Arithmetic : public Fundamental {
};

class Integral : public Arithmetic {
};

// Все целочисленные типы
class BoolType : public Integral {
public:
    explicit BoolType(std::any value);
    BoolType();
    std::any& get_value();
private:
    std::any value;
};

class CharType : public Integral {
public:
    explicit CharType(std::any value);
    CharType();
    std::any& get_value();
private:
    std::any value;
};

class IntegerType : public Integral {
public:
    explicit IntegerType(std::any value);
    IntegerType();
    std::any& get_value();
private:
    std::any value;
};

// Все типы с плавающей точкой
class FloatType : public Arithmetic {
public:
    explicit FloatType(std::any value);
    FloatType();
    std::any& get_value();
private:
    std::any value;
};

// Составные типы
class Composite : public Type {
};

// Тип функции
class FuncType : public Composite {
public:
    FuncType(const Type& returnableType, const std::vector<Type>& args);
    const Type& get_returnable_type() const;
    const std::vector<Type>& get_args() const;
private:
    const Type& returnableType;
    std::vector<Type> args;
};

// Тип записи
class Record : public Composite {
};

// Все типы записей
class StructType : public Record {
public:
    explicit StructType(const std::unordered_map<std::string, Type>& members);
    const std::unordered_map<std::string, Type>& get_members() const;
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
    explicit PointerType(const Type& base);
    PointerType(); // nullptr
    const Type& get_base() const;
private:
    const Type* base; // Используем указатель для избежания копирования
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