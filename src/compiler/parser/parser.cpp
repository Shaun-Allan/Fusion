#include "../../include/parser.h"
#include <stdexcept>
#include <iostream>

Parser::Parser(const std::vector<Token> &tokens) : tokens(tokens), current(0), indentLevel(0) {}

std::vector<std::unique_ptr<Statement>> Parser::parse()
{
    std::vector<std::unique_ptr<Statement>> statements;
    while (!isAtEnd())
    {
        skipNewlines();
        if (isAtEnd())
            break;
        statements.push_back(declaration());
    }
    return statements;
}

std::unique_ptr<Statement> Parser::declaration()
{
    skipNewlines();

    if (match(TokenType::CLASS))
        return classDeclaration();
    if (match(TokenType::TASK))
        return taskDeclaration();
    if (match(TokenType::IF))
        return ifStatement();
    if (match(TokenType::WHILE))
        return whileStatement();
    if (match(TokenType::FOR))
        return forStatement();
    if (match(TokenType::RETURN))
        return returnStatement();
    if (match(TokenType::PASS))
        return passStatement();
    if (match(TokenType::BREAK))
        return breakStatement();
    if (match(TokenType::CONTINUE))
        return continueStatement();
    if (match(TokenType::PRINT))
        return printStatement();

    // Could be assignment or expression statement
    return statement();
}

// Parse class and task declarations as before
// ...

// New Statement parsing functions:

std::unique_ptr<Statement> Parser::ifStatement()
{
    auto condition = expression();

    std::vector<std::unique_ptr<Statement>> thenBranch;
    std::vector<std::unique_ptr<Statement>> elseBranch;

    if (match(TokenType::LEFT_BRACE))
    {
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd())
        {
            thenBranch.push_back(declaration());
        }
        consume(TokenType::RIGHT_BRACE, "Expect '}' after if block.");
    }
    else if (match(TokenType::NEWLINE))
    {
        consume(TokenType::INDENT, "Expect indented block after if condition.");
        while (!check(TokenType::DEDENT) && !isAtEnd())
        {
            thenBranch.push_back(declaration());
        }
        consume(TokenType::DEDENT, "Expect dedent after if block.");
    }
    else
    {
        throw std::runtime_error("Expect '{' or indentation after if condition.");
    }

    if (match(TokenType::ELSE))
    {
        if (match(TokenType::LEFT_BRACE))
        {
            while (!check(TokenType::RIGHT_BRACE) && !isAtEnd())
            {
                elseBranch.push_back(declaration());
            }
            consume(TokenType::RIGHT_BRACE, "Expect '}' after else block.");
        }
        else if (match(TokenType::NEWLINE))
        {
            consume(TokenType::INDENT, "Expect indented block after else.");
            while (!check(TokenType::DEDENT) && !isAtEnd())
            {
                elseBranch.push_back(declaration());
            }
            consume(TokenType::DEDENT, "Expect dedent after else block.");
        }
        else
        {
            throw std::runtime_error("Expect '{' or indentation after else.");
        }
    }

    return std::make_unique<IfStatement>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

std::unique_ptr<Statement> Parser::whileStatement()
{
    auto condition = expression();

    std::vector<std::unique_ptr<Statement>> body;
    if (match(TokenType::LEFT_BRACE))
    {
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd())
        {
            body.push_back(declaration());
        }
        consume(TokenType::RIGHT_BRACE, "Expect '}' after while body.");
    }
    else if (match(TokenType::NEWLINE))
    {
        consume(TokenType::INDENT, "Expect indented block after while condition.");
        while (!check(TokenType::DEDENT) && !isAtEnd())
        {
            body.push_back(declaration());
        }
        consume(TokenType::DEDENT, "Expect dedent after while body.");
    }
    else
    {
        throw std::runtime_error("Expect '{' or indentation after while condition.");
    }

    return std::make_unique<WhileStatement>(std::move(condition), std::move(body));
}

std::unique_ptr<Statement> Parser::forStatement()
{
    auto initializer = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after for initializer.");
    auto condition = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after for condition.");
    auto increment = expression();

    std::vector<std::unique_ptr<Statement>> body;
    if (match(TokenType::LEFT_BRACE))
    {
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd())
        {
            body.push_back(declaration());
        }
        consume(TokenType::RIGHT_BRACE, "Expect '}' after for body.");
    }
    else if (match(TokenType::NEWLINE))
    {
        consume(TokenType::INDENT, "Expect indented block after for header.");
        while (!check(TokenType::DEDENT) && !isAtEnd())
        {
            body.push_back(declaration());
        }
        consume(TokenType::DEDENT, "Expect dedent after for body.");
    }
    else
    {
        throw std::runtime_error("Expect '{' or indentation after for header.");
    }

    return std::make_unique<ForStatement>(std::move(initializer), std::move(condition), std::move(increment), std::move(body));
}

std::unique_ptr<Statement> Parser::returnStatement()
{
    std::unique_ptr<Expression> value = nullptr;
    if (!check(TokenType::NEWLINE) && !check(TokenType::EOF_TOKEN))
    {
        value = expression();
    }
    // Optionally consume trailing newline
    match(TokenType::NEWLINE);
    return std::make_unique<ReturnStatement>(std::move(value));
}

std::unique_ptr<Statement> Parser::passStatement()
{
    return std::make_unique<PassStatement>();
}

std::unique_ptr<Statement> Parser::breakStatement()
{
    return std::make_unique<BreakStatement>();
}

std::unique_ptr<Statement> Parser::continueStatement()
{
    return std::make_unique<ContinueStatement>();
}

std::unique_ptr<Statement> Parser::printStatement()
{
    auto expr = expression();
    return std::make_unique<PrintStatement>(std::move(expr));
}

std::unique_ptr<Statement> Parser::statement()
{
    // Could be assignment or expression statement
    if (check(TokenType::IDENTIFIER) && tokens.size() > current + 1 && tokens[current + 1].type == TokenType::EQUAL)
    {
        return assignmentStatement();
    }
    return expressionStatement();
}

std::unique_ptr<Statement> Parser::assignmentStatement()
{
    Token nameToken = consume(TokenType::IDENTIFIER, "Expect variable name.");
    consume(TokenType::EQUAL, "Expect '=' after variable name.");
    auto value = expression();
    return std::make_unique<AssignmentStatement>(nameToken.lexeme, std::move(value));
}

std::unique_ptr<Statement> Parser::expressionStatement()
{
    auto expr = expression();
    return std::make_unique<ExpressionStatement>(std::move(expr));
}

// Expression parsing follows typical precedence climbing or recursive descent

std::unique_ptr<Expression> Parser::expression()
{
    return logicalOr();
}

std::unique_ptr<Expression> Parser::logicalOr()
{
    auto expr = logicalAnd();

    while (match(TokenType::OR))
    {
        std::string op = previous().lexeme;
        auto right = logicalAnd();
        expr = std::make_unique<LogicalExpression>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expression> Parser::logicalAnd()
{
    auto expr = equality();

    while (match(TokenType::AND))
    {
        std::string op = previous().lexeme;
        auto right = equality();
        expr = std::make_unique<LogicalExpression>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expression> Parser::equality()
{
    auto expr = comparison();

    while (match(TokenType::BANG_EQUAL) || match(TokenType::EQUAL_EQUAL))
    {
        std::string op = previous().lexeme;
        auto right = comparison();
        expr = std::make_unique<BinaryExpression>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expression> Parser::comparison()
{
    auto expr = term();

    while (match(TokenType::GREATER) || match(TokenType::GREATER_EQUAL) || match(TokenType::LESS) || match(TokenType::LESS_EQUAL))
    {
        std::string op = previous().lexeme;
        auto right = term();
        expr = std::make_unique<BinaryExpression>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expression> Parser::term()
{
    auto expr = factor();

    while (match(TokenType::PLUS) || match(TokenType::MINUS))
    {
        std::string op = previous().lexeme;
        auto right = factor();
        expr = std::make_unique<BinaryExpression>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expression> Parser::factor()
{
    auto expr = unary();

    while (match(TokenType::STAR) || match(TokenType::SLASH))
    {
        std::string op = previous().lexeme;
        auto right = unary();
        expr = std::make_unique<BinaryExpression>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expression> Parser::unary()
{
    if (match(TokenType::BANG) || match(TokenType::MINUS))
    {
        std::string op = previous().lexeme;
        auto right = unary();
        return std::make_unique<UnaryExpression>(op, std::move(right));
    }
    return primary();
}

std::unique_ptr<Expression> Parser::primary()
{
    if (match(TokenType::FALSE))
        return std::make_unique<BooleanLiteral>(false);
    if (match(TokenType::TRUE))
        return std::make_unique<BooleanLiteral>(true);
    if (match(TokenType::NULL_TOKEN))
        return std::make_unique<NullLiteral>();
    if (match(TokenType::NUMBER))
        return std::make_unique<NumberLiteral>(std::stod(previous().lexeme));
    if (match(TokenType::STRING))
        return std::make_unique<StringLiteral>(previous().lexeme);
    if (match(TokenType::IDENTIFIER))
        return std::make_unique<VariableExpression>(previous().lexeme);
    if (match(TokenType::LEFT_PAREN))
    {
        auto expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_unique<GroupingExpression>(std::move(expr));
    }

    throw std::runtime_error("Expect expression.");
}

// Helper functions

bool Parser::match(TokenType type)
{
    if (check(type))
    {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(TokenType type)
{
    if (isAtEnd())
        return false;
    return peek().type == type;
}

const Token &Parser::advance()
{
    if (!isAtEnd())
        current++;
    return previous();
}

bool Parser::isAtEnd()
{
    return peek().type == TokenType::EOF_TOKEN;
}

const Token &Parser::peek()
{
    return tokens[current];
}

const Token &Parser::previous()
{
    return tokens[current - 1];
}

Token Parser::consume(TokenType type, const std::string &message)
{
    if (check(type))
        return advance();
    throw std::runtime_error(message);
}

void Parser::skipNewlines()
{
    while (match(TokenType::NEWLINE))
    {
    }
}
