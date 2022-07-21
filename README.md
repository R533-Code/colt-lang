# Colt
Contains the Colt Compiler and Interpreter, which are both still in early development.
### The Colt Interpreter:
An interpreter in development for the `colt` language.
Uses byte-code, and a stack-based virtual machine.

### The Colt Compiler:
Not yet implemented: will use LLVM as a backend.


---


## Generating the Project:
You can either manually create a `build` directory and run CMake:
```
mkdir build && cd build && cmake ..
```
Or you can run the `generate_project.py` script.

## Building the Executable:
You can use the `build_colt.py`, or run in the source directory:
```
cmake --build build
```


---


## Implemented Features:
- Basic type system
- Global Variables
- Local Variables and scopes
- If-elif-else