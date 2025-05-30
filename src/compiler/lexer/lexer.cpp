#include "../../include/lexer.h"
#include <unordered_map>
#include <iostream>  // for std::cerr

static std::unordered_map<std::string, TokenType> keywords = {
    {"class", TokenType::CLASS},
    {"def", TokenType::DEF},
    {"task", TokenType::TASK},
    {"parallel", TokenType::PARALLEL},
    {"async", TokenType::ASYNC},
    {"await", TokenType::AWAIT},
    {"if", TokenType::IF},
    {"else", TokenType::ELSE},
    {"for", TokenType::FOR},
    {"while", TokenType::WHILE},
    {"return", TokenType::RETURN},
    {"and", TokenType::AND},
    {"or", TokenType::OR},
    {"not", TokenType::NOT},
    {"pass", TokenType::PASS},
    {"print", TokenType::PRINT},
};

Lexer::Lexer(const std::string& source) : source(source) {}

std::vector<Token> Lexer::scanTokens() {
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }
    
    tokens.push_back({TokenType::EOF_TOKEN, "", line});
    return tokens;
}

void Lexer::scanToken() {
    if (atLineStart) {
        int indentCount = 0;

        // Count spaces or tabs
        while (peek() == ' ' || peek() == '\t') {
            if (peek() == ' ') indentCount += 1;
            else if (peek() == '\t') indentCount += 4; // or your tab size
            advance();
        }

        int currentIndent = indentStack.back();
        if (indentCount > currentIndent) {
            indentStack.push_back(indentCount);
            addToken(TokenType::INDENT);
        } else {
            while (indentCount < indentStack.back()) {
                indentStack.pop_back();
                addToken(TokenType::DEDENT);
            }
            if (indentCount != indentStack.back()) {
                error(line, "Inconsistent indentation.");
            }
        }

        atLineStart = false;
    }

    start = current;
    if (isAtEnd()) return;

    char c = advance();

    // skip spaces/tabs inside lines
    if (c == ' ' || c == '\t') {
        // just skip without emitting tokens
        return;
    }
    
    switch (c) {
        // Grouping symbols
        case '(': addToken(TokenType::LEFT_PAREN); break;
        case ')': addToken(TokenType::RIGHT_PAREN); break;
        case '{': addToken(TokenType::LEFT_BRACE); break;
        case '}': addToken(TokenType::RIGHT_BRACE); break;
        case '[': addToken(TokenType::LEFT_BRACKET); break;
        case ']': addToken(TokenType::RIGHT_BRACKET); break;
        case ',': addToken(TokenType::COMMA); break;
        case '.': addToken(TokenType::DOT); break;
        case ';': addToken(TokenType::SEMICOLON); break;
        case ':': addToken(TokenType::COLON); break;

        // Operators
        case '+': addToken(TokenType::PLUS); break;
        case '-': addToken(TokenType::MINUS); break;
        case '*': addToken(TokenType::STAR); break;
        case '/':
            if (match('/')) {
                while (peek() != '\n' && !isAtEnd()) advance(); // line comment
            } else if (match('*')) {
                while (!(peek() == '*' && peekNext() == '/') && !isAtEnd()) advance(); // multiline comment
                if (!isAtEnd()) {
                    advance(); // consume '*'
                    advance(); // consume '/'
                }
            } else {
                addToken(TokenType::SLASH);
            }
            break;

        case '=': addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::ASSIGN); break;
        case '!': addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::IDENTIFIER); break;
        case '<': addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS); break;
        case '>': addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER); break;

        // Whitespace (skip)
        case ' ':
        case '\r':
            break;

        case '\n':
            addToken(TokenType::NEWLINE);
            line++;
            atLineStart = true;
            break;

        case '"':
            string();
            break;

        default:
            if (isDigit(c)) {
                number();
            } else if (isAlpha(c)) {
                identifier();
            } else {
                error(line, std::string("Unexpected character: ") + c);
            }
            break;
    }
}



char Lexer::advance() {
    return source[current++];
}

void Lexer::addToken(TokenType type) {
    std::string text = source.substr(start, current - start);
    tokens.push_back({type, text, line});
}

bool Lexer::match(char expected) {
    if (isAtEnd() || source[current] != expected) return false;
    current++;
    return true;
}

char Lexer::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

char Lexer::peekNext() {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

bool Lexer::isAtEnd() {
    return current >= source.length();
}

void Lexer::string() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') line++;
        advance();
    }
    
    if (isAtEnd()) {
        error(line, "Unterminated string.");
        return;
    }
    
    advance();  // Consume closing "
    addToken(TokenType::STRING);
}

void Lexer::number() {
    while (isDigit(peek())) advance();
    
    if (peek() == '.' && isDigit(peekNext())) {
        advance(); // consume '.'
        while (isDigit(peek())) advance();
    }
    
    addToken(TokenType::NUMBER);
}

void Lexer::identifier() {
    while (isAlphaNumeric(peek())) advance();
    
    std::string text = source.substr(start, current - start);
    auto it = keywords.find(text);
    if (it != keywords.end()) {
        addToken(it->second);
    } else {
        addToken(TokenType::IDENTIFIER);
    }
}

bool Lexer::isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool Lexer::isAlpha(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           c == '_';
}  

bool Lexer::isAlphaNumeric(char c) {
    return isAlpha(c) || isDigit(c);
}

void Lexer::error(int line, const std::string& message) {
    std::cerr << "[Line " << line << "] Error: " << message << std::endl;
}
