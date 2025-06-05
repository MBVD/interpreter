#include <type.hpp>

struct Symbol {
    Symbol(const std::shared_ptr<Type>& type, bool is_const = false)
        : type(type), is_const(is_const) {}
    virtual ~Symbol() = default;

    virtual bool is_var() {
        return false;
    }

    virtual bool is_func() {
        return false;
    }

    virtual bool is_record() {
        return false;
    }

    std::shared_ptr<Type> type;
    bool is_const;
};

struct VarSymbol : public Symbol {
    VarSymbol(const std::shared_ptr<Type>& type, bool is_const = false)
        : Symbol(type, is_const) {}
    VarSymbol(const std::shared_ptr<Type>& type, std::any value, bool is_const = false)
        : Symbol(type, is_const), value(value) {}
    bool is_var() {
        return true;
    }

    std::any value;
};

struct FuncSymbol : public Symbol {
    FuncSymbol(const std::shared_ptr<FuncType>& type, FuncDeclarator* node, bool is_const = false)
        : Symbol(type, is_const), func_declarator(node) {}
    bool is_func() {
        return true; 
    }
    FuncDeclarator* func_declarator = nullptr;
};

struct Record : public Symbol {
    Record(const std::shared_ptr<RecordType>& type, const std::unordered_map<std::string, std::shared_ptr<Symbol>>& fields ,bool is_const = false)
        : Symbol(type, is_const), fields(fields){}
    bool is_record() { 
        return true; 
    }

    std::unordered_map<std::string, std::shared_ptr<Symbol>> fields;
};