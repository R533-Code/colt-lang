# Colt
Contains the Colt Compiler and Interpreter, which are both still in early development.
### The 'colti', the Colt interpreter:
An interpreter in development for the `colt` language.
Uses byte-code, and a stack-based virtual machine.

## TODO:
- [ ] Add AST
- [ ] Add basic code generation

## Error Codes:
- 0: Successful
- 1: Assertion related failure
- 2: OS related failure (Ex: `malloc` returned `NULL`, `fread` couldn't read all the file)
- 3: Invalid input related error (Ex: invalid filepath...)

## Coding Style:
- Header include guards should use `HG_COLTI_{FILENAME}`, no `pragma once`

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

- Globals should start with `g_`

- Enums should be `UPPERCASE_SNAKE`

- When reporting errors, the output should use: `CONSOLE_FOREGROUND_BRIGHT_RED"Error: "CONSOLE_COLOR_RESET` followed by the message
  - If a message is printed before exiting, it should be followed by a newline