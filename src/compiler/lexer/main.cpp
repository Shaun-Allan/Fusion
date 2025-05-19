#include "../../include/lexer.h"
#include <iostream>
#include <fstream>
#include <sstream>

// Function to print token type as string
std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::LEFT_PAREN: return "LEFT_PAREN";
        case TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
        case TokenType::PLUS: return "PLUS";
        case TokenType::MINUS: return "MINUS";
        case TokenType::STAR: return "STAR";
        case TokenType::SLASH: return "SLASH";
        case TokenType::COLON: return "COLON";
        case TokenType::NEWLINE: return "NEWLINE";
        case TokenType::STRING: return "STRING";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::CLASS: return "CLASS";
        case TokenType::DEF: return "DEF";
        case TokenType::TASK: return "TASK";
        case TokenType::PARALLEL: return "PARALLEL";
        case TokenType::ASYNC: return "ASYNC";
        case TokenType::AWAIT: return "AWAIT";
        case TokenType::IF: return "IF";
        case TokenType::ELSE: return "ELSE";
        case TokenType::FOR: return "FOR";
        case TokenType::WHILE: return "WHILE";
        case TokenType::RETURN: return "RETURN";
        case TokenType::EOF_TOKEN: return "EOF";
        default: return "UNKNOWN";
    }
}

int main() {
    // Example source code (can also be read from a file)
    std::string source = R"(
        class MyClass
            task doSomething(param1: int): void {
                // task body could go here
                pass
            }
        )";

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    std::cout << "Tokens:\n";
    for (const auto& token : tokens) {
        std::cout << "[" << tokenTypeToString(token.type) << "] "
                  << "\"" << token.lexeme << "\" at line " << token.line << "\n";
    }

    return 0;
}
