#include <any>
#include <typeinfo>
#include <stdexcept>
#include <functional>

struct anyEvaluator : std::any {
    std::any value;
    T correct_value;
    void(*name) = nullptr;
    template<typename T>
    T evaluate() const {
        return std::any_cast<T>(value);
    }
    
    template<typename T>
    anyEvaluator(T&& value) : std::any(value) {}
};