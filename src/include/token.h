#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum class TokenType {
    // Keywords
    CLASS, DEF, TASK, PARALLEL, ASYNC, AWAIT,
    IF, ELSE, FOR, WHILE, RETURN, AND, OR, NOT,

    // Literals
    IDENTIFIER, STRING, NUMBER,

    // Operators
    PLUS, MINUS, STAR, SLASH,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,
    EQUAL, EQUAL_EQUAL,
    BANG_EQUAL, // for !=

    // Symbols
    COLON, COMMA, DOT, SEMICOLON,
    ASSIGN, // '='

    // Grouping
    LEFT_PAREN, RIGHT_PAREN,
    LEFT_BRACE, RIGHT_BRACE,
    LEFT_BRACKET, RIGHT_BRACKET,

    // Whitespace
    INDENT, DEDENT, NEWLINE,

    // End of File
    EOF_TOKEN
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
};

#endif
