#include <string>
#include <vector>
#include <memory>

// Forward declarations
class Expression;
class Statement;

// Base AST node class
class ASTNode {
public:
    virtual ~ASTNode() = default;
};

// Expression class hierarchy
class Expression : public ASTNode {
public:
    virtual ~Expression() = default;
};

class Literal : public Expression {
public:
    Literal(const std::string& value) : value(value) {}
    std::string value;
};

// Statement class hierarchy
class Statement : public ASTNode {
public:
    virtual ~Statement() = default;
};

class ClassStatement : public Statement {
public:
    ClassStatement(const std::string& name, 
                   std::vector<std::unique_ptr<Statement>> methods)
        : name(name), methods(std::move(methods)) {}
    
    std::string name;
    std::vector<std::unique_ptr<Statement>> methods;
};

class TaskStatement : public Statement {
public:
    TaskStatement(const std::string& name,
                  std::vector<std::pair<std::string, std::string>> params,
                  std::string returnType,
                  std::vector<std::unique_ptr<Statement>> body)
        : name(name), params(params), returnType(returnType), 
          body(std::move(body)) {}
    
    std::string name;
    std::vector<std::pair<std::string, std::string>> params;  // (name, type)
    std::string returnType;
    std::vector<std::unique_ptr<Statement>> body;
};