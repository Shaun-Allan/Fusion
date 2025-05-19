#include "../../include/compiler.h"
#include <iostream>
#include <string>
#include <cstdlib>

bool Compiler::compile(const std::string& source, Chunk& chunk) {
    hadError = false;
    compilingChunk = &chunk;
    currentLine = 1;
    
    // Lexical analysis
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    
    // Parsing
    Parser parser(tokens);
    std::vector<std::unique_ptr<Statement>> statements = parser.parse();
    
    if (hadError) return false;
    
    // Code generation
    for (auto& stmt : statements) {
        stmt->accept(this);
    }
    
    emitReturn();
    
    #ifdef DEBUG_PRINT_CODE
    if (!hadError) {
        Disassembler::disassembleChunk(*currentChunk(), "code");
    }
    #endif
    
    return !hadError;
}

// Expression visitor methods

void Compiler::visitLiteral(Literal* expr) {
    if (expr->value == "null") {
        emitConstant(nullptr);
    } else if (expr->value == "true") {
        emitConstant(true);
    } else if (expr->value == "false") {
        emitConstant(false);
    } else if (expr->value[0] == '"' && expr->value[expr->value.length() - 1] == '"') {
        // String literal (remove the quotes)
        std::string str = expr->value.substr(1, expr->value.length() - 2);
        emitConstant(str);
    } else {
        // Assume it's a number
        try {
            double value = std::stod(expr->value);
            emitConstant(value);
        } catch (const std::exception& e) {
            error("Invalid literal: " + expr->value);
        }
    }
}

void Compiler::visitGroupingExpression(GroupingExpression* expr) {
    // Just compile the contained expression
    expr->expression->accept(this);
}

void Compiler::visitUnaryExpression(UnaryExpression* expr) {
    // Compile the operand first
    expr->right->accept(this);
    
    // Then emit the unary operator
    switch (expr->op.type) {
        case TokenType::MINUS:
            emitByte(OpCode::NEGATE);
            break;
        case TokenType::BANG:
            emitByte(OpCode::NOT);
            break;
        default:
            error("Unknown unary operator: " + expr->op.lexeme);
            return;
    }
}

void Compiler::visitBinaryExpression(BinaryExpression* expr) {
    // Compile the left operand
    expr->left->accept(this);
    
    // Compile the right operand
    expr->right->accept(this);
    
    // Then emit the binary operator
    switch (expr->op.type) {
        case TokenType::PLUS:
            emitByte(OpCode::ADD);
            break;
        case TokenType::MINUS:
            emitByte(OpCode::SUBTRACT);
            break;
        case TokenType::STAR:
            emitByte(OpCode::MULTIPLY);
            break;
        case TokenType::SLASH:
            emitByte(OpCode::DIVIDE);
            break;
        case TokenType::EQUAL_EQUAL:
            emitByte(OpCode::EQUALS);
            break;
        case TokenType::BANG_EQUAL:
            // a != b is the same as !(a == b)
            emitByte(OpCode::EQUALS);
            emitByte(OpCode::NOT);
            break;
        case TokenType::GREATER:
            emitByte(OpCode::GREATER);
            break;
        case TokenType::GREATER_EQUAL:
            // a >= b is the same as !(a < b)
            emitByte(OpCode::LESS);
            emitByte(OpCode::NOT);
            break;
        case TokenType::LESS:
            emitByte(OpCode::LESS);
            break;
        case TokenType::LESS_EQUAL:
            // a <= b is the same as !(a > b)
            emitByte(OpCode::GREATER);
            emitByte(OpCode::NOT);
            break;
        default:
            error("Unknown binary operator: " + expr->op.lexeme);
            return;
    }
}

void Compiler::visitVariableExpression(VariableExpression* expr) {
    // For now, we don't support variables, so this is an error
    error("Variables not supported yet: " + expr->name.lexeme);
}

// Statement visitor methods

void Compiler::visitExpressionStatement(ExpressionStatement* stmt) {
    stmt->expression->accept(this);
    emitByte(OpCode::POP); // Discard the result
}

void Compiler::visitPrintStatement(PrintStatement* stmt) {
    stmt->expression->accept(this);
    emitByte(OpCode::PRINT);
}

void Compiler::visitClassStatement(ClassStatement* stmt) {
    // Not implementing class compilation yet
    error("Class declarations not supported in bytecode compiler yet.");
}

void Compiler::visitTaskStatement(TaskStatement* stmt) {
    // Not implementing task compilation yet
    error("Task declarations not supported in bytecode compiler yet.");
}

// Helper methods

void Compiler::emitByte(OpCode byte) {
    currentChunk()->write(byte, currentLine);
}

void Compiler::emitBytes(OpCode byte1, uint8_t byte2) {
    emitByte(byte1);
    currentChunk()->code.push_back(byte2);
    currentChunk()->lines.push_back(currentLine);
}

void Compiler::emitConstant(const Value& value) {
    int index = currentChunk()->addConstant(value);
    emitBytes(OpCode::CONSTANT, index);
}

void Compiler::emitReturn() {
    emitByte(OpCode::RETURN);
}

void Compiler::error(const std::string& message) {
    hadError = true;
    std::cerr << "Compiler error: " << message << std::endl;
}

Chunk* Compiler::currentChunk() {
    return compilingChunk;
}