#include <vector>
#include <memory>
#include "token.h"
#include "ast.h"

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::vector<std::unique_ptr<Statement>> parse();

private:
    std::vector<Token> tokens;
    int current = 0;
    
    std::unique_ptr<Statement> declaration();
    std::unique_ptr<ClassStatement> classDeclaration();
    std::unique_ptr<TaskStatement> taskDeclaration();
    
    bool match(TokenType type);
    bool check(TokenType type);
    Token advance();
    Token peek();
    Token previous();
    bool isAtEnd();
    Token consume(TokenType type, const std::string& message);
};