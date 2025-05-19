#include "../../include/bytecode.h"
#include <iostream>
#include <iomanip>

void Chunk::write(OpCode byte, int line) {
    code.push_back(static_cast<uint8_t>(byte));
    lines.push_back(line);
}

void Chunk::writeConstant(const Value& value, int line) {
    int constant = addConstant(value);
    
    // Write the CONSTANT instruction
    write(OpCode::CONSTANT, line);
    
    // Write the constant index as the operand
    code.push_back(constant);
    lines.push_back(line);
}

int Chunk::addConstant(const Value& value) {
    constants.push_back(value);
    return constants.size() - 1;
}

// Disassembler implementation
void Disassembler::disassembleChunk(const Chunk& chunk, const std::string& name) {
    std::cout << "== " << name << " ==" << std::endl;
    
    for (int offset = 0; offset < chunk.code.size();) {
        offset = disassembleInstruction(chunk, offset);
    }
}

int Disassembler::disassembleInstruction(const Chunk& chunk, int offset) {
    std::cout << std::setw(4) << std::setfill('0') << offset << " ";
    
    if (offset > 0 && chunk.lines[offset] == chunk.lines[offset - 1]) {
        std::cout << "   | ";
    } else {
        std::cout << std::setw(4) << chunk.lines[offset] << " ";
    }
    
    uint8_t instruction = chunk.code[offset];
    switch (static_cast<OpCode>(instruction)) {
        case OpCode::CONSTANT:
            return constantInstruction("CONSTANT", chunk, offset);
        case OpCode::ADD:
            return simpleInstruction("ADD", offset);
        case OpCode::SUBTRACT:
            return simpleInstruction("SUBTRACT", offset);
        case OpCode::MULTIPLY:
            return simpleInstruction("MULTIPLY", offset);
        case OpCode::DIVIDE:
            return simpleInstruction("DIVIDE", offset);
        case OpCode::NEGATE:
            return simpleInstruction("NEGATE", offset);
        case OpCode::NOT:
            return simpleInstruction("NOT", offset);
        case OpCode::EQUALS:
            return simpleInstruction("EQUALS", offset);
        case OpCode::GREATER:
            return simpleInstruction("GREATER", offset);
        case OpCode::LESS:
            return simpleInstruction("LESS", offset);
        case OpCode::PRINT:
            return simpleInstruction("PRINT", offset);
        case OpCode::POP:
            return simpleInstruction("POP", offset);
        case OpCode::RETURN:
            return simpleInstruction("RETURN", offset);
        default:
            std::cout << "Unknown opcode " << instruction << std::endl;
            return offset + 1;
    }
}

int Disassembler::simpleInstruction(const std::string& name, int offset) {
    std::cout << name << std::endl;
    return offset + 1;
}

int Disassembler::constantInstruction(const std::string& name, const Chunk& chunk, int offset) {
    uint8_t constantIndex = chunk.code[offset + 1];
    std::cout << name << " " << static_cast<int>(constantIndex) << " '";
    
    // Print the constant value
    const Value& value = chunk.constants[constantIndex];
    if (std::holds_alternative<double>(value)) {
        std::cout << std::get<double>(value);
    } else if (std::holds_alternative<bool>(value)) {
        std::cout << (std::get<bool>(value) ? "true" : "false");
    } else if (std::holds_alternative<std::string>(value)) {
        std::cout << std::get<std::string>(value);
    } else if (std::holds_alternative<std::nullptr_t>(value)) {
        std::cout << "null";
    }
    
    std::cout << "'" << std::endl;
    return offset + 2;
}