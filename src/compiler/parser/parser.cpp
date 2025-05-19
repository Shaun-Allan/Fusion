#include "../../include/parser.h"
#include <stdexcept>
#include <iostream>

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), current(0), indentLevel(0) {}

std::vector<std::unique_ptr<Statement>> Parser::parse() {
    std::vector<std::unique_ptr<Statement>> statements;
    while (!isAtEnd()) {
        skipNewlines();
        if (isAtEnd()) break;
        try {
            statements.push_back(declaration());
        } catch (const std::runtime_error& e) {
            std::cerr << e.what() << std::endl;
            // Synchronize for error recovery
            synchronize();
        }
    }
    return statements;
}

void Parser::synchronize() {
    advance();
    
    while (!isAtEnd()) {
        if (previous().type == TokenType::NEWLINE) return;
        
        switch (peek().type) {
            case TokenType::CLASS:
            case TokenType::TASK:
            case TokenType::FOR:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::PRINT:
            case TokenType::RETURN:
                return;
        }
        
        advance();
    }
}

std::unique_ptr<Statement> Parser::declaration() {
    skipNewlines();

    if (match(TokenType::CLASS)) return classDeclaration();
    if (match(TokenType::TASK)) return taskDeclaration();
    if (match(TokenType::PRINT)) return printStatement();
    
    return expressionStatement();
}

std::unique_ptr<ClassStatement> Parser::classDeclaration() {
    Token nameToken = consume(TokenType::IDENTIFIER, "Expect class name.");
    std::string className = nameToken.lexeme;

    std::vector<std::unique_ptr<Statement>> methods;

    if (match(TokenType::LEFT_BRACE)) {
        // Go-style block
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            skipNewlines();
            if (check(TokenType::RIGHT_BRACE)) break;
            methods.push_back(declaration());
        }
        consume(TokenType::RIGHT_BRACE, "Expect '}' after class body.");
    } else if (match(TokenType::NEWLINE)) {
        // Python-style indentation block
        consume(TokenType::INDENT, "Expect indented block after class declaration.");
        while (!check(TokenType::DEDENT) && !isAtEnd()) {
            skipNewlines();
            if (check(TokenType::DEDENT)) break;
            methods.push_back(declaration());
        }
        consume(TokenType::DEDENT, "Expect dedent after class body.");
    } else {
        throw std::runtime_error("Expect '{' or indentation after class declaration.");
    }

    return std::make_unique<ClassStatement>(className, std::move(methods));
}

std::unique_ptr<TaskStatement> Parser::taskDeclaration() {
    Token nameToken = consume(TokenType::IDENTIFIER, "Expect task name.");
    std::string taskName = nameToken.lexeme;

    consume(TokenType::LEFT_PAREN, "Expect '(' after task name.");

    std::vector<std::pair<std::string, std::string>> params;
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            Token paramName = consume(TokenType::IDENTIFIER, "Expect parameter name.");
            consume(TokenType::COLON, "Expect ':' after parameter name.");
            Token paramType = consume(TokenType::IDENTIFIER, "Expect parameter type.");
            params.emplace_back(paramName.lexeme, paramType.lexeme);
        } while (match(TokenType::COMMA));
    }

    consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");

    std::string returnType = "void";
    if (match(TokenType::COLON)) {
        Token returnTypeToken = consume(TokenType::IDENTIFIER, "Expect return type.");
        returnType = returnTypeToken.lexeme;
    }

    std::vector<std::unique_ptr<Statement>> body;

    if (match(TokenType::LEFT_BRACE)) {
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            skipNewlines();
            if (check(TokenType::RIGHT_BRACE)) break;
            body.push_back(declaration());
        }
        consume(TokenType::RIGHT_BRACE, "Expect '}' after task body.");
    } else if (match(TokenType::NEWLINE)) {
        consume(TokenType::INDENT, "Expect indented block after task declaration.");
        while (!check(TokenType::DEDENT) && !isAtEnd()) {
            skipNewlines();
            if (check(TokenType::DEDENT)) break;
            body.push_back(declaration());
        }
        consume(TokenType::DEDENT, "Expect dedent after task body.");
    } else {
        throw std::runtime_error("Expect '{' or indentation after task declaration.");
    }

    return std::make_unique<TaskStatement>(taskName, params, returnType, std::move(body));
}

std::unique_ptr<Statement> Parser::printStatement() {
    auto value = expression();
    consumeEndOfStatement();
    return std::make_unique<PrintStatement>(std::move(value));
}

std::unique_ptr<Statement> Parser::expressionStatement() {
    auto expr = expression();
    consumeEndOfStatement();
    return std::make_unique<ExpressionStatement>(std::move(expr));
}

void Parser::consumeEndOfStatement() {
    // Allow optional semicolon
    match(TokenType::SEMICOLON);
    
    // If we're at a newline or end of file, we're good
    if (check(TokenType::NEWLINE) || check(TokenType::EOF_TOKEN)) {
        match(TokenType::NEWLINE); // Consume it if it's a newline
        return;
    }
    
    throw std::runtime_error("Expect newline or semicolon after expression.");
}

void Parser::skipNewlines() {
    while (match(TokenType::NEWLINE)) {
        // Consume all newlines
    }
}

std::unique_ptr<Expression> Parser::expression() {
    return equality();
}

std::unique_ptr<Expression> Parser::equality() {
    auto expr = comparison();
    
    while (match(TokenType::EQUAL_EQUAL) || match(TokenType::BANG_EQUAL)) {
        Token op = previous();
        auto right = comparison();
        expr = std::make_unique<BinaryExpression>(std::move(expr), op, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::comparison() {
    auto expr = term();
    
    while (match(TokenType::GREATER) || match(TokenType::GREATER_EQUAL) ||
           match(TokenType::LESS) || match(TokenType::LESS_EQUAL)) {
        Token op = previous();
        auto right = term();
        expr = std::make_unique<BinaryExpression>(std::move(expr), op, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::term() {
    auto expr = factor();
    
    while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
        Token op = previous();
        auto right = factor();
        expr = std::make_unique<BinaryExpression>(std::move(expr), op, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::factor() {
    auto expr = unary();
    
    while (match(TokenType::STAR) || match(TokenType::SLASH)) {
        Token op = previous();
        auto right = unary();
        expr = std::make_unique<BinaryExpression>(std::move(expr), op, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::unary() {
    if (match(TokenType::BANG) || match(TokenType::MINUS)) {
        Token op = previous();
        auto right = unary();
        return std::make_unique<UnaryExpression>(op, std::move(right));
    }
    
    return primary();
}

std::unique_ptr<Expression> Parser::primary() {
    if (match(TokenType::FALSE)) return std::make_unique<Literal>("false");
    if (match(TokenType::TRUE)) return std::make_unique<Literal>("true");
    if (match(TokenType::NULL_TOKEN)) return std::make_unique<Literal>("null");
    
    if (match(TokenType::NUMBER) || match(TokenType::STRING)) {
        return std::make_unique<Literal>(previous().lexeme);
    }
    
    if (match(TokenType::IDENTIFIER)) {
        return std::make_unique<VariableExpression>(previous());
    }
    
    if (match(TokenType::LEFT_PAREN)) {
        auto expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_unique<GroupingExpression>(std::move(expr));
    }
    
    throw std::runtime_error("Expect expression.");
}

// Token helpers

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

Token Parser::peek() {
    return tokens[current];
}

Token Parser::previous() {
    return tokens[current - 1];
}

bool Parser::isAtEnd() {
    return peek().type == TokenType::EOF_TOKEN;
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw std::runtime_error("Error at line " + std::to_string(peek().line) + ": " + message);
}