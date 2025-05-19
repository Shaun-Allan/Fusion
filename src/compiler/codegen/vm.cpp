#include "../../include/vm.h"
#include "../../include/compiler.h"
#include <iostream>
#include <sstream>

VM::VM() : ip(0) {}

InterpretResult VM::interpret(const std::string& source) {
    Compiler compiler;
    if (!compiler.compile(source, chunk)) {
        return InterpretResult::COMPILE_ERROR;
    }
    
    ip = 0;
    return run();
}

InterpretResult VM::run() {
    #define READ_BYTE() (chunk.code[ip++])
    #define READ_CONSTANT() (chunk.constants[READ_BYTE()])
    
    while (true) {
        #ifdef DEBUG_TRACE_EXECUTION
        std::cout << "Stack: ";
        for (const auto& value : stack) {
            std::cout << "[ " << valueToString(value) << " ]";
        }
        std::cout << std::endl;
        Disassembler::disassembleInstruction(chunk, ip);
        #endif
        
        uint8_t instruction = READ_BYTE();
        switch (static_cast<OpCode>(instruction)) {
            case OpCode::CONSTANT: {
                Value constant = READ_CONSTANT();
                push(constant);
                break;
            }
            case OpCode::ADD: {
                if (isString(peek(0)) && isString(peek(1))) {
                    // String concatenation
                    std::string b = asString(pop());
                    std::string a = asString(pop());
                    push(a + b);
                } else if (isNumber(peek(0)) && isNumber(peek(1))) {
                    // Numeric addition
                    double b = asNumber(pop());
                    double a = asNumber(pop());
                    push(a + b);
                } else {
                    runtimeError("Operands must be two numbers or two strings.");
                    return InterpretResult::RUNTIME_ERROR;
                }
                break;
            }
            case OpCode::SUBTRACT: {
                if (!isNumber(peek(0)) || !isNumber(peek(1))) {
                    runtimeError("Operands must be numbers.");
                    return InterpretResult::RUNTIME_ERROR;
                }
                double b = asNumber(pop());
                double a = asNumber(pop());
                push(a - b);
                break;
            }
            case OpCode::MULTIPLY: {
                if (!isNumber(peek(0)) || !isNumber(peek(1))) {
                    runtimeError("Operands must be numbers.");
                    return InterpretResult::RUNTIME_ERROR;
                }
                double b = asNumber(pop());
                double a = asNumber(pop());
                push(a * b);
                break;
            }
            case OpCode::DIVIDE: {
                if (!isNumber(peek(0)) || !isNumber(peek(1))) {
                    runtimeError("Operands must be numbers.");
                    return InterpretResult::RUNTIME_ERROR;
                }
                double b = asNumber(pop());
                if (b == 0) {
                    runtimeError("Division by zero.");
                    return InterpretResult::RUNTIME_ERROR;
                }
                double a = asNumber(pop());
                push(a / b);
                break;
            }
            case OpCode::NEGATE: {
                if (!isNumber(peek(0))) {
                    runtimeError("Operand must be a number.");
                    return InterpretResult::RUNTIME_ERROR;
                }
                push(-asNumber(pop()));
                break;
            }
            case OpCode::NOT:
                push(!isTruthy(pop()));
                break;
            case OpCode::EQUALS: {
                Value b = pop();
                Value a = pop();
                push(valuesEqual(a, b));
                break;
            }
            case OpCode::GREATER: {
                if (!isNumber(peek(0)) || !isNumber(peek(1))) {
                    runtimeError("Operands must be numbers.");
                    return InterpretResult::RUNTIME_ERROR;
                }
                double b = asNumber(pop());
                double a = asNumber(pop());
                push(a > b);
                break;
            }
            case OpCode::LESS: {
                if (!isNumber(peek(0)) || !isNumber(peek(1))) {
                    runtimeError("Operands must be numbers.");
                    return InterpretResult::RUNTIME_ERROR;
                }
                double b = asNumber(pop());
                double a = asNumber(pop());
                push(a < b);
                break;
            }
            case OpCode::PRINT: {
                std::cout << valueToString(pop()) << std::endl;
                break;
            }
            case OpCode::POP:
                pop();
                break;
            case OpCode::RETURN:
                return InterpretResult::OK;
        }
    }
    
    #undef READ_BYTE
    #undef READ_CONSTANT
}

void VM::push(Value value) {
    stack.push_back(value);
}

Value VM::pop() {
    Value value = stack.back();
    stack.pop_back();
    return value;
}

Value VM::peek(int distance) {
    return stack[stack.size() - 1 - distance];
}

bool VM::isTruthy(const Value& value) {
    if (std::holds_alternative<std::nullptr_t>(value)) {
        return false;
    }
    if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value);
    }
    return true;
}

bool VM::valuesEqual(const Value& a, const Value& b) {
    if (a.index() != b.index()) return false;
    
    if (std::holds_alternative<std::nullptr_t>(a)) {
        return true;  // null == null
    }
    if (std::holds_alternative<bool>(a)) {
        return std::get<bool>(a) == std::get<bool>(b);
    }
    if (std::holds_alternative<double>(a)) {
        return std::get<double>(a) == std::get<double>(b);
    }
    if (std::holds_alternative<std::string>(a)) {
        return std::get<std::string>(a) == std::get<std::string>(b);
    }
    
    return false;  // Shouldn't reach here
}

std::string VM::valueToString(const Value& value) {
    if (std::holds_alternative<double>(value)) {
        std::ostringstream ss;
        ss << std::get<double>(value);
        return ss.str();
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? "true" : "false";
    } else if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    } else if (std::holds_alternative<std::nullptr_t>(value)) {
        return "null";
    }
    
    return "unknown";
}

void VM::runtimeError(const std::string& message) {
    std::cerr << message << std::endl;
    
    size_t instruction = ip - 1;
    int line = chunk.lines[instruction];
    std::cerr << "[line " << line << "] in script" << std::endl;
    
    stack.clear();
}

bool VM::isNumber(const Value& value) {
    return std::holds_alternative<double>(value);
}

bool VM::isString(const Value& value) {
    return std::holds_alternative<std::string>(value);
}

double VM::asNumber(const Value& value) {
    return std::get<double>(value);
}

std::string VM::asString(const Value& value) {
    return std::get<std::string>(value);
}