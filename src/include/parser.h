#ifndef PARSER_H
#define PARSER_H

#include <memory>
#include <vector>
#include <string>
#include "token.h"
#include "expression.h"
#include "statement.h"

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::vector<std::unique_ptr<Statement>> parse();

private:
    const std::vector<Token>& tokens;
    int current;
    int indentLevel;

    // Error handling
    void synchronize();
    
    // Statement parsing methods
    std::unique_ptr<Statement> declaration();
    std::unique_ptr<ClassStatement> classDeclaration();
    std::unique_ptr<TaskStatement> taskDeclaration();
    std::unique_ptr<Statement> printStatement();
    std::unique_ptr<Statement> expressionStatement();
    void consumeEndOfStatement();

    // Expression parsing methods using recursive descent
    std::unique_ptr<Expression> expression();
    std::unique_ptr<Expression> equality();
    std::unique_ptr<Expression> comparison();
    std::unique_ptr<Expression> term();
    std::unique_ptr<Expression> factor();
    std::unique_ptr<Expression> unary();
    std::unique_ptr<Expression> primary();

    // Helper methods
    void skipNewlines();
    bool match(TokenType type);
    bool check(TokenType type);
    Token advance();
    Token peek();
    Token previous();
    bool isAtEnd();
    Token consume(TokenType type, const std::string& message);
};

#endif // PARSER_H