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
    virtual void print();
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
    explicit BoolType();
    void print();
private:
    bool value;
};

class CharType : public Integral {
public:
    explicit CharType();
    void print();
private:
    char16_t value;
};

class IntegerType : public Integral {
public:
    explicit IntegerType();
    void print();
private:
    int8_t value;
};

// Все типы с плавающей точкой
class FloatType : public Arithmetic {
public:
    FloatType();
    void print();
private:
    double value;
};

// Составные типы
class Composite : public Type {
};

// Тип функции
class FuncType : public Composite {
public:
    FuncType(std::shared_ptr<Type>, std::vector<std::shared_ptr<Type>>, const std::unique_ptr<BlockStatement>&);
    std::shared_ptr<Type> get_returnable_type() const;
    std::vector<std::shared_ptr<Type>> get_args() const;
    const std::unique_ptr<FuncDeclarator>& get_func_declarator();
    void print();
private:
    std::shared_ptr<Type> returnable_type;
    std::vector<std::shared_ptr<Type>> args;
    const std::unique_ptr<FuncDeclarator>& func_declarator;
};

// Тип записи
class RecordType : public Composite {
};

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
    ArrayType(std::shared_ptr<Type> base); // Тип и размер массива
    void print();
};

std::shared_ptr<Type> compare_types(std::shared_ptr<Type> left, std::shared_ptr<Type>right);
int getTypeRank(std::shared_ptr<Type> type);