## TESTS:
This folder contains unit tests for the compiler.
The CMake target that defines them is `coltc_test`.
The unit tests make use of `Catch2`, which is included in `colt-cpp`.

When adding a new test file, add `#include <includes.h>` before any includes.