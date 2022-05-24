/** @file parse_args.h
* Contains command-line argument parsing tools.
* To parse arguments, call `ParseArguments`.
* If the arguments are informational (like --version, --help...), the function
* `ParseArguments` will itself exit.
* If not, then a ParseResult will be returned, containing the needed data.
* To avoid having to deal with strings in most functions, impl_string_to_arg converts
* a string to an enum from CommandLineArgument.
* To add new possible arguments:
* - Add enum value
* - Add case in impl_string_to_arg
* - Add help documentation function `impl_help_...`
* - Add case in impl_help
* - Add function `impl_...` that handles parsing and error of the rest of the arguments
* - Add case in ParseArguments
*/

#ifndef HG_COLTI_PARSE_ARGS
#define HG_COLTI_PARSE_ARGS

#include "common.h"
#include "chunk.h"
#include "disassemble.h"

/// @brief The result of parsing command line arguments.
/// This struct is one of the only that will never hold any heap-allocated value.
/// This is the reason why 'file_path_out' defaults to 'a.out'
typedef struct
{
	/// @brief The path to the file to interpret/compile
	const char* file_path_in;
	/// @brief The output executable file
	const char* file_path_out;
	/// @brief The output file to where to write the byte-code
	const char* byte_code_out;
} ParseResult;

/// @brief Enum representing valid arguments
typedef enum
{
	/// @brief -h, or --help
	ARG_HELP,
	/// @brief -e, or --enum
	ARG_ENUM,
	/// @brief -v, or --version
	ARG_VERSION,
	/// @brief -d, or --disassemble
	ARG_DISASSEMBLE,
	/// @brief -o, or --out
	ARG_EXEC_OUTPUT,
	/// @brief -b or --byte-out
	ARG_BYTE_CODE_OUTPUT,
	/// @brief --test-color
	ARG_TEST_COLOR_CONSOLE,
	/// @brief Any invalid argument
	ARG_INVALID
} CommandLineArgument;

/// @brief Parses the command line arguments and returns a corresponding result
/// @param argc The argument count
/// @param argv The argument values
/// @return A ParseResult containing results
ParseResult ParseArguments(int argc, const char** argv);

/// @brief Check if a path is a valid file
/// @param path The path to check for
/// @return True if the path is valid and points to a file
bool checkIfValidFile(const char* path);

/*****************************************
IMPLEMENTATION HELPERS
*****************************************/

/// @brief Transform a string to an enum value, which makes it easier to deal with
/// @param str The string to convert, which SHOULD start with '-'
/// @return The argument type, which can be ARG_INVALID when the 'str' is not recognized
CommandLineArgument impl_string_to_arg(const char* str);

/// @brief Handles the -v or --version logic, and exits
/// @param argc The argument count
/// @param argv The argument values
void impl_version(int argc, const char** argv);

/// @brief Handles the -d or --disassemble logic, and exits
/// @param argc The argument count
/// @param argv The argument values
void impl_disassemble(int argc, const char** argv);

/// @brief Handles the -h or --help logic, and exits
/// @param argc The argument count
/// @param argv The argument values
void impl_help(int argc, const char** argv);

/// @brief Handles the -e or --enum logic and exits
/// @param argc The argument count
/// @param argv The argument values
void impl_enum(int argc, const char** argv);

/// @brief Handles the -o or --out logic, returns a valid path or exits
/// @param argc The argument count
/// @param argv The argument values
/// @param current_argc The offset to the value after -o
/// @return A valid path to which to write the executable
const char* impl_exec_out(int argc, const char** argv, size_t current_argc);

/// @brief Handles the -b or --byte-out, returns a valid path or exits
/// @param argc The argument count
/// @param argv The argument values
/// @param current_argc The offset to the value after -b
/// @return A valid path to which to write the byte-code
const char* impl_byte_out(int argc, const char** argv, size_t current_argc);

/// @brief Handles the --test-color and exits
/// @param argc The argument count
/// @param argv The argument values
void impl_test_color(int argc, const char** argv);

/// @brief Prints an error that is caused by an invalid combination of arguments.
/// Expects at least that argc is 2.
/// @param argc The argument count
/// @param argv The argument values
void impl_print_invalid_combination(int argc, const char** argv);

/// @brief Prints the help of '-d' or '--disassemble'
void impl_help_disassemble();

/// @brief Prints the help of '-v' or '--version'
void impl_help_version();

/// @brief Prints the help of '-h' or '--help'
void impl_help_help();

/// @brief Prints the help of '-e' or '--enum'
void impl_help_enum();

/// @brief Prints the help of '-o' or '--out'
void impl_help_exec_out();

/// @brief Prints the help of '-b' or '--byte-out'
void impl_help_byte_out();

/// @brief Prints the help of '--test-color'
void impl_help_test_color();

#endif //HG_COLTI_PARSE_ARGS