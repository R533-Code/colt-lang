## Coding Style:
- Header include guards should use `HG_COLT_{FILENAME}`, no `pragma once`

- Free Functions that do not expect preconditions should be in `camelCase`
- Functions that are in `snake_case` are implementation helpers (private function which are not recommended to be used)
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
  - Exception are for the `Colt.*` built-in types
  - Try to minimize their use as much as possible

- Enums value should be `UPPERCASE_SNAKE`

- When reporting errors, the output should use: `CONSOLE_FOREGROUND_BRIGHT_RED "Error: " CONSOLE_COLOR_RESET` followed by the message
  - `print_error_string` and `print_error_format` macros can be used to follow this convention
  - If a message is printed before exiting, it should be followed by a newline

- When using `exit`, use the `ExitCode` enum values rather than arbitrary values