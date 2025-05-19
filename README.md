# Fusoin Programming Language

Fusoin is a statically-typed, object-oriented, compiled programming language designed for simplicity, performance, and modern concurrency. It features task parallelism, garbage collection, and a Python-inspired easy syntax. Fusoin source files use the `.fs` extension.

## Project Structure

- **Lexer**: The lexer (tokenizer) reads Fusoin source code and converts it into a stream of tokens. This is the first step in the compilation process, breaking down the raw text into meaningful symbols for the parser.
- **Parser**: The parser takes the token stream from the lexer and builds an Abstract Syntax Tree (AST), representing the grammatical structure of the program.
- **Fusoin Virtual Machine (Fusoin VM)**: The Fusoin VM executes the compiled bytecode. It manages memory (including garbage collection), handles task parallelism, and provides a runtime environment for Fusoin programs.

## Building Fusoin

To build Fusoin, ensure you have a C++17-compatible compiler (like `g++`). Then run:

```sh
make
```

To clean the build artifacts:

```sh
make clean
```

To run an example Fusoin program:

```sh
make run
```

## Example Fusoin Program (`example.fs`)

```fs
// Example Fusoin program: Hello World and simple parallel task

class Greeter {
    fun greet() {
        print("Hello, Fusoin!")
    }
}

fun main() {
    let g = Greeter()
    task t = spawn g.greet()
    wait t
}
```

## Features

- **Object Oriented**: Classes and objects for modular code.
- **Static Typing**: Type safety at compile time.
- **Garbage Collection**: Automatic memory management.
- **Easy Syntax**: Inspired by Python for readability and simplicity.

## File Extensions

- Fusoin source files: `.fs`

---