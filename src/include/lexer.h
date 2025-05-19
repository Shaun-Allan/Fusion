#include <string>
#include <vector>
#include "token.h"

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> scanTokens();

private:
    std::string source;
    std::vector<Token> tokens;
    int start = 0;
    int current = 0;
    int line = 1;
    std::vector<int> indentStack = {0};  // starts with 0 indentation
    bool atLineStart = true;             // track start of line

    
    bool isAtEnd();
    void scanToken();
    char advance();
    void addToken(TokenType type);
    bool match(char expected);
    char peek();
    char peekNext();
    
    void string();
    void number();
    void identifier();
    bool isDigit(char c);
    bool isAlpha(char c);
    bool isAlphaNumeric(char c);

    void error(int line, const std::string& message);
};