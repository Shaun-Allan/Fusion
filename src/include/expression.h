#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <memory>
#include <string>
#include "token.h"

// Forward declarations
class ExpressionVisitor;

// Base expression class with visitor pattern
class Expression {
public:
    virtual ~Expression() = default;
    virtual void accept(ExpressionVisitor* visitor) = 0;
};

// Literal expression (numbers, strings, booleans, null)
class Literal : public Expression {
public:
    Literal(const std::string& value) : value(value) {}
    
    void accept(ExpressionVisitor* visitor) override;
    
    std::string value;
};

// Grouping expression (parenthesized expressions)
class GroupingExpression : public Expression {
public:
    GroupingExpression(std::unique_ptr<Expression> expression)
        : expression(std::move(expression)) {}
    
    void accept(ExpressionVisitor* visitor) override;
    
    std::unique_ptr<Expression> expression;
};

// Unary expression (e.g., -a, !b)
class UnaryExpression : public Expression {
public:
    UnaryExpression(Token op, std::unique_ptr<Expression> right)
        : op(op), right(std::move(right)) {}
    
    void accept(ExpressionVisitor* visitor) override;
    
    Token op;
    std::unique_ptr<Expression> right;
};

// Binary expression (e.g., a + b, c * d)
class BinaryExpression : public Expression {
public:
    BinaryExpression(std::unique_ptr<Expression> left, Token op, std::unique_ptr<Expression> right)
        : left(std::move(left)), op(op), right(std::move(right)) {}
    
    void accept(ExpressionVisitor* visitor) override;
    
    std::unique_ptr<Expression> left;
    Token op;
    std::unique_ptr<Expression> right;
};

// Variable expression (identifier reference)
class VariableExpression : public Expression {
public:
    VariableExpression(Token name) : name(name) {}
    
    void accept(ExpressionVisitor* visitor) override;
    
    Token name;
};

// Visitor for expressions
class ExpressionVisitor {
public:
    virtual ~ExpressionVisitor() = default;
    virtual void visitLiteral(Literal* expr) = 0;
    virtual void visitGroupingExpression(GroupingExpression* expr) = 0;
    virtual void visitUnaryExpression(UnaryExpression* expr) = 0;
    virtual void visitBinaryExpression(BinaryExpression* expr) = 0;
    virtual void visitVariableExpression(VariableExpression* expr) = 0;
};

// Implementations of accept methods
inline void Literal::accept(ExpressionVisitor* visitor) {
    visitor->visitLiteral(this);
}

inline void GroupingExpression::accept(ExpressionVisitor* visitor) {
    visitor->visitGroupingExpression(this);
}

inline void UnaryExpression::accept(ExpressionVisitor* visitor) {
    visitor->visitUnaryExpression(this);
}

inline void BinaryExpression::accept(ExpressionVisitor* visitor) {
    visitor->visitBinaryExpression(this);
}

inline void VariableExpression::accept(ExpressionVisitor* visitor) {
    visitor->visitVariableExpression(this);
}

#endif // EXPRESSION_H