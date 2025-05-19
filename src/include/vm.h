#ifndef VM_H
#define VM_H

#include <vector>
#include "bytecode.h"

// Interpretation result codes
enum class InterpretResult {
    OK,
    COMPILE_ERROR,
    RUNTIME_ERROR
};

// Virtual machine that executes bytecode
class VM {
public:
    VM();
    
    InterpretResult interpret(const std::string& source);
    InterpretResult run();
    
private:
    Chunk chunk;
    std::vector<Value> stack;
    int ip; // Instruction pointer
    
    // Stack operations
    void push(Value value);
    Value pop();
    Value peek(int distance = 0);
    
    // Helper methods for runtime values
    bool isTruthy(const Value& value);
    bool valuesEqual(const Value& a, const Value& b);
    std::string valueToString(const Value& value);
    
    // Error handling
    void runtimeError(const std::string& message);
    
    // Type-checking utilities
    bool isNumber(const Value& value);
    bool isString(const Value& value);
    double asNumber(const Value& value);
    std::string asString(const Value& value);
};

#endif // VM_H