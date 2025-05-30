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
    bool is_var() {
        return true;
    }
};

struct FuncSymbol : public Symbol {
    FuncSymbol(const std::shared_ptr<FuncType>& type, bool is_const = false)
        : Symbol(type, is_const) {}
    bool is_func() {
        return true; 
    }
};

struct Record : public Symbol {
    Record(const std::shared_ptr<RecordType>& type, const std::unordered_map<std::string, std::shared_ptr<Symbol>>& ,bool is_const = false)
        : Symbol(type, is_const), fields(fields){}
    bool is_record() { 
        return true; 
    }

    std::unordered_map<std::string, std::shared_ptr<Symbol>> fields;
};