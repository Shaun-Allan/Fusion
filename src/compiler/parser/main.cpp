#include <iostream>
#include <memory>
#include "../../include/lexer.h"
#include "../../include/parser.h"


// Simple helper to print Statements (expand as needed)
void printStatement(const Statement* stmt, int indent = 0) {
    std::string indentStr(indent, ' ');
    if (const ClassStatement* cls = dynamic_cast<const ClassStatement*>(stmt)) {
        std::cout << indentStr << "Class: " << cls->name << "\n";
        for (const auto& method : cls->methods) {
            printStatement(method.get(), indent + 2);
        }
    } else if (const TaskStatement* task = dynamic_cast<const TaskStatement*>(stmt)) {
        std::cout << indentStr << "Task: " << task->name << ", Return Type: " << task->returnType << "\n";
        std::cout << indentStr << "Params:\n";
        for (auto& p : task->params) {
            std::cout << indentStr << "  " << p.first << ": " << p.second << "\n";
        }
        std::cout << indentStr << "Body Statements:\n";
        for (const auto& bodyStmt : task->body) {
            printStatement(bodyStmt.get(), indent + 2);
        }
    } else {
        std::cout << indentStr << "Unknown Statement\n";
    }
}

int main() {
    // Sample source code string to test
    std::string source = R"(
        class MyClass
            task doSomething(param1: int): void {
                pass
            }
        )";

    // Step 1: Tokenize using Lexer
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    // Optional: print tokens for debugging
    std::cout << "Tokens:\n";
    for (const auto& token : tokens) {
        std::cout << "  " << token.lexeme << " (" << static_cast<int>(token.type) << ")\n";
    }
    std::cout << "-----\n";

    // Step 2: Parse tokens using Parser
    Parser parser(tokens);
    std::vector<std::unique_ptr<Statement>> statements = parser.parse();

    // Step 3: Print parsed AST
    std::cout << "Parsed AST Statements:\n";
    for (const auto& stmt : statements) {
        printStatement(stmt.get());
    }

    return 0;
}
