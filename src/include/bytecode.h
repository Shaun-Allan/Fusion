#ifndef BYTECODE_H
#define BYTECODE_H

#include <vector>
#include <string>
#include <unordered_map>
#include <variant>
#include <cstdint>

// Bytecode instruction opcodes
enum class OpCode : uint8_t {
    CONSTANT, // Push constant value onto stack
    ADD,      // Add top two values on stack
    SUBTRACT, // Subtract top value from second value on stack
    MULTIPLY, // Multiply top two values on stack
    DIVIDE,   // Divide second value by top value on stack
    NEGATE,   // Negate top value on stack
    NOT,      // Logical not of top value
    EQUALS,   // Compare top two values for equality
    GREATER,  // Compare second value > top value
    LESS,     // Compare second value < top value
    PRINT,    // Print top value on stack
    POP,      // Remove top value from stack
    RETURN    // End execution
};

// Runtime value types
using Value = std::variant<double, bool, std::string, std::nullptr_t>;

// Representation of a compiled bytecode chunk
class Chunk {
public:
    void write(OpCode byte, int line);
    void writeConstant(const Value& value, int line);
    int addConstant(const Value& value);
    
    std::vector<uint8_t> code;
    std::vector<Value> constants;
    std::vector<int> lines;  // Line numbers for debugging
};

// Disassembler for bytecode chunks
class Disassembler {
public:
    static void disassembleChunk(const Chunk& chunk, const std::string& name);
    static int disassembleInstruction(const Chunk& chunk, int offset);

private:
    static int simpleInstruction(const std::string& name, int offset);
    static int constantInstruction(const std::string& name, const Chunk& chunk, int offset);
};

#endif // BYTECODE_H