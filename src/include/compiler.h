#ifndef COMPILER_H
#define COMPILER_H

#include <string>
#include "bytecode.h"
#include "expression.h"
#include "statement.h"
#include "lexer.h"
#include "parser.h"

// Compiler class that turns source code into bytecode
class Compiler : public ExpressionVisitor, public StatementVisitor {
public:
    bool compile(const std::string& source, Chunk& chunk);
    
    // Expression visitor methods
    void visitLiteral(Literal* expr) override;
    void visitGroupingExpression(GroupingExpression* expr) override;
    void visitUnaryExpression(UnaryExpression* expr) override;
    void visitBinaryExpression(BinaryExpression* expr) override;
    void visitVariableExpression(VariableExpression* expr) override;
    
    // Statement visitor methods
    void visitExpressionStatement(ExpressionStatement* stmt) override;
    void visitPrintStatement(PrintStatement* stmt) override;
    void visitClassStatement(ClassStatement* stmt) override;
    void visitTaskStatement(TaskStatement* stmt) override;
    
private:
    Chunk* compilingChunk;
    bool hadError;
    int currentLine;
    
    // Helper methods for emitting bytecode
    void emitByte(OpCode byte);
    void emitBytes(OpCode byte1, uint8_t byte2);
    void emitConstant(const Value& value);
    void emitReturn();
    
    // Error handling
    void error(const std::string& message);
    
    // Current chunk being compiled
    Chunk* currentChunk();
};

#endif // COMPILER_H