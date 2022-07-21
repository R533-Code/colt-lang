## Testing:
As for every project of more than a couple thousands lines of code, testing ensures that no changes cause a breaking change in the correct behavior of the program.
As most functions in the Colt repo are intertwined, testing each would not be meaningful.

#### Solution:
Through CMake's testing facilities, small sample of codes contained in this directory are interpreted and their output tested against a regular expression.
This allow to check for error and warning generation, but also valid runs.
To simplify the process of creating those tests, `CMakeLists.txt` contains some logic to recurse in the `tests` directory, saving all the files that end with `.ct`, and creating a test for each.
Passing the regular expression is also simplified: the first line of each of the `.ct` files is a comment starting with `//` and followed by the regular expression to test for.

### Creating Tests:
- Add new file ending with `.ct` in `tests` or its subdirectories
- Write code in that file
- On the first line of that file, write `//` followed by the regular expression to which the output of the interpretation should match