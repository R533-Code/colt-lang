# Colt
Contains the Colt Compiler and Interpreter, which are both still in early development.
### The 'colt', the Colt interpreter:
An interpreter in development for the `colt` language.
Uses byte-code, and a stack-based virtual machine.

## Generating Project:
You can either manually create a `build` directory and run CMake:
```
mkdir build && cd build && cmake ..
```
Or you can run the `generate_project.py` script.

## Building Executable:
You can use the `build_colt.py`, or run in the source directory:
```
cmake --build build
```

## Implemented Features:
- `Scanner`: Breaks the string to parse into lexemes
- `AST` and `Expr`: Represents the parsed string in a tree-like form
- `generateByteCode`: Generates byte-code from an AST
- `StackVM`: Runs byte-code

The language (for now) supports:
- global variables
- string literals
- a rudimentary type system
- local variables (Scopes)

## Error Codes:
- 0: Successful
- 1: Assertion related failure
- 2: OS related failure (Ex: `malloc` returned `NULL`, `fread` couldn't read all the file)
- 3: Invalid input related error (Ex: invalid filepath, or argument...)

## Coding Style:
- Header include guards should use `HG_COLT_{FILENAME}`, no `pragma once`

- Free Functions that do not expect preconditions should be in `camelCase`
  - Functions that should not be called because they expect preconditions start with `impl_{name}`
  - Functions that starts with `unsafe_` are the fastest functions which do not check any of their inputs

- Structs should be in `PascalCase`: `struct Chunk`
  - Structs should be defined in the following way `typedef struct {...} NAME;`
  - Any functions that can be seen as a method for that struct should be in `PascalCase`, starting with the struct name: `ChunkInit(Chunk*)`

- Unions should follow the same rules as Structs
  - `[DQ]WORD` and `BYTE` are the only exception

- Macros should be in all `UPPERCASE_SNAKE`
  - Only exception is for assertion and allocation macros
  - Function-like macros should use `do { /* CODE */ } while (0)` trick

- Globals should start with `g_` and be constant
  - Try to minimize their use as much as possible
  - Exception are for the `Colt.*` built-in types

- Enums should be `UPPERCASE_SNAKE`

- When reporting errors, the output should use: `CONSOLE_FOREGROUND_BRIGHT_RED "Error: " CONSOLE_COLOR_RESET` followed by the message
  - If a message is printed before exiting, it should be followed by a newline