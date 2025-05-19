#ifndef STATEMENT_H
#define STATEMENT_H

#include <memory>
#include <string>
#include <vector>
#include <utility>
#include "expression.h"

// Forward declarations
class StatementVisitor;

// Base statement class with visitor pattern
class Statement {
public:
    virtual ~Statement() = default;
    virtual void accept(StatementVisitor* visitor) = 0;
};

// Expression statement (e.g., a + b;)
class ExpressionStatement : public Statement {
public:
    ExpressionStatement(std::unique_ptr<Expression> expression)
        : expression(std::move(expression)) {}
    
    void accept(StatementVisitor* visitor) override;
    
    std::unique_ptr<Expression> expression;
};

// Print statement (e.g., print a + b;)
class PrintStatement : public Statement {
public:
    PrintStatement(std::unique_ptr<Expression> expression)
        : expression(std::move(expression)) {}
    
    void accept(StatementVisitor* visitor) override;
    
    std::unique_ptr<Expression> expression;
};

// Class declaration
class ClassStatement : public Statement {
public:
    ClassStatement(const std::string& name, 
                   std::vector<std::unique_ptr<Statement>> methods)
        : name(name), methods(std::move(methods)) {}
    
    void accept(StatementVisitor* visitor) override;
    
    std::string name;
    std::vector<std::unique_ptr<Statement>> methods;
};

// Task declaration
class TaskStatement : public Statement {
public:
    TaskStatement(const std::string& name,
                  std::vector<std::pair<std::string, std::string>> params,
                  std::string returnType,
                  std::vector<std::unique_ptr<Statement>> body)
        : name(name), params(params), returnType(returnType), 
          body(std::move(body)) {}
    
    void accept(StatementVisitor* visitor) override;
    
    std::string name;
    std::vector<std::pair<std::string, std::string>> params;  // (name, type)
    std::string returnType;
    std::vector<std::unique_ptr<Statement>> body;
};

// Visitor for statements
class StatementVisitor {
public:
    virtual ~StatementVisitor() = default;
    virtual void visitExpressionStatement(ExpressionStatement* stmt) = 0;
    virtual void visitPrintStatement(PrintStatement* stmt) = 0;
    virtual void visitClassStatement(ClassStatement* stmt) = 0;
    virtual void visitTaskStatement(TaskStatement* stmt) = 0;
};

// Implementations of accept methods
inline void ExpressionStatement::accept(StatementVisitor* visitor) {
    visitor->visitExpressionStatement(this);
}

inline void PrintStatement::accept(StatementVisitor* visitor) {
    visitor->visitPrintStatement(this);
}

inline void ClassStatement::accept(StatementVisitor* visitor) {
    visitor->visitClassStatement(this);
}

inline void TaskStatement::accept(StatementVisitor* visitor) {
    visitor->visitTaskStatement(this);
}

#endif // STATEMENT_H