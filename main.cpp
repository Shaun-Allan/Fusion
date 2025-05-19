#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "src/include/vm.h"

void runFile(const std::string& path);
void runPrompt();
std::string readFile(const std::string& path);

int main(int argc, char* argv[]) {
    if (argc > 2) {
        std::cout << "Usage: langlang [script]" << std::endl;
        return 64;
    } else if (argc == 2) {
        runFile(argv[1]);
    } else {
        runPrompt();
    }
    
    return 0;
}

void runFile(const std::string& path) {
    std::string source = readFile(path);
    VM vm;
    InterpretResult result = vm.interpret(source);
    
    if (result == InterpretResult::COMPILE_ERROR) {
        exit(65);
    }
    if (result == InterpretResult::RUNTIME_ERROR) {
        exit(70);
    }
}

void runPrompt() {
    VM vm;
    std::string line;
    
    std::cout << "LangLang VM v0.1" << std::endl;
    std::cout << "Type 'exit' to quit" << std::endl;
    
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line) || line == "exit") {
            std::cout << "Goodbye!" << std::endl;
            break;
        }
        
        vm.interpret(line);
    }
}

std::string readFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    
    if (!file) {
        std::cerr << "Could not open file \"" << path << "\"." << std::endl;
        exit(74);
    }
    
    std::stringstream contents;
    contents << file.rdbuf();
    
    return contents.str();
}