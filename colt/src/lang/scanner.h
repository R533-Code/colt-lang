/** @file scanner.h
* Contains the Scanner struct, which breaks a string into lexemes.
* As the Scanner does not need to modify the input string, it takes in
* a StringView.
* The Scanner also handles error printing through `impl_scanner_print_error`.
* The Scanner's string to integer conversion handles binary integers `0b`, decimal integers `0x`, octal integers `0o`.
* As identifiers are never modified when extracted from source code, they are stored
* as a StringView. On the other hand, string literals are stored as String,
* and should be freed using `StringFree`.
*/

#ifndef HG_COLT_SCANNER
#define HG_COLT_SCANNER

#include "common.h"
#include "type/type.h"
#include "structs/struct_string.h"
#include "token.h"

/// @brief Struct responsible of breaking a string into lexemes
typedef struct
{
	/// @brief The last parsed identifier
	StringView parsed_identifier;
	/// @brief The last parsed string literal, which is also used as a temporary buffer
	String parsed_string;
	/// @brief Last parsed integer/bool/double/float
	QWORD parsed_value;

	/// @brief The string to scan
	StringView view;
	/// @brief The offset to the current character from the string
	uint64_t offset;
	/// @brief The offset to the beginning of the current lexeme
	uint64_t lexeme_begin;
	/// @brief The current line number
	uint64_t current_line;
	/// @brief The current char, which is the one to parse next
	char current_char;
} Scanner;

/// @brief Initializes a Scanner
/// @param scan The scanner to initialize
/// @param to_scan The StringView to scan
void ScannerInit(Scanner* scan, StringView to_scan);

/// @brief Frees any resources used by a Scanner
/// @param scan The scanner to modify
void ScannerFree(Scanner* scan);

/// @brief Returns the parsed identifier
/// @param scan The scanner from which to get the value
/// @return A StringView of the identifier
StringView ScannerGetIdentifier(const Scanner* scan);

/// @brief Returns a heap-allocated copy of the last parsed string literal
/// @param scan The scanner from which to get the value
/// @return A pointer to a copy of the String, that should be freed with StringFree and also deleted
String* ScannerGetLString(const Scanner* scan);

/// @brief Returns the last parsed type name
/// @param scan The scanner from which to get the value
/// @return A Type representing the last parsed type name
const TypeInfo* ScannerGetTypeInfo(const Scanner* scan);

/// @brief Returns the last parsed double/float/integral as a QWORD
/// @param scan The scanner from which to get the value
/// @return The QWORD representing the last parsed literal
QWORD ScannerGetParsedQWORD(const Scanner* scan);

/// @brief Get the next token from a scanner.
/// @param scan The scanner from which to get the value
/// @return A Token representing the parsed lexeme, or TKN_EOF if there are no more lexemes
Token ScannerGetNextToken(Scanner* scan);

/// @brief Returns a StringView over the current line being parsed
/// @param scan The scanner from which to get the line
/// @return A StringView of the current line, without any newline character at the beginning or the end
StringView ScannerGetCurrentLine(const Scanner* scan);

/// @brief Returns a StringView over the current lexeme parsed lexeme
/// @param scan The scanner from which to get the lexeme
/// @return A StringView of the lexeme
StringView ScannerGetCurrentLexeme(const Scanner* scan);

/**********************************
IMPLEMENTATION HELPERS
**********************************/

/// @brief Prints formatted 'error' and highlights the current lexeme.
/// Prints the line number, the line, highlights the lexeme.
/// This function is as optimized as it can be, but it should still only
/// be called for when error are found to avoid slowdown.
/// @param scan The scanner from which to get the lexeme and line
/// @param error The error, which is a `printf` style-format string
/// @param ... Variadic number of arguments to format to 'error'
void scan_print_error(const Scanner* scan, const char* error, ...);

/// @brief Prints an error for unclosed multi-line comments.
/// The string manipulation calculation used by `impl_scanner_print_error`
/// do not work for unclosed comments, which is why it is separated in its
/// own function.
/// @param scan The scanner from which to get the lexeme and line
void scan_print_unclosed_comment(const Scanner* scan);

/// @brief Returns the next character in the stream, and updates the offset
/// @param scan The scanner from which to get the character
/// @return The next character or EOF (-1) if no more characters are available
char scan_get_next_char(Scanner* scan);

/// @brief Returns the next character + offset in the stream, without updating the offset.
/// To peek to the next character, let offset be equal to 0.
/// @param scan The scanner from which to get the character
/// @param offset The next character + offset
/// @return The character or EOF if no more characters are available
char scan_peek_next_char(const Scanner* scan, uint64_t offset);

/// @brief Rewinds one character before the current one
/// @param scan The scanner to rewind
/// @return The character pointed to after rewinding
char scan_rewind_char(Scanner* scan);

/// @brief Rewinds 'nb' character before the current one
/// @param scan The scanner to rewind
/// @param nb The number of character to rewind
/// @return The character pointed to after rewinding
char scan_rewind_chars(Scanner* scan, uint64_t nb);

/// @brief Handles an identifier case, searching for if it's a keyword or not
/// @param scan The scanner from which to get the identifier
/// @return The Token representing the identifier
Token scan_handle_identifier(Scanner* scan);

/// @brief Handles a digit case, searching for if it's a float or an integer
/// @param scan The scanner from which to get the value
/// @return The Token representing the identifier
Token scan_handle_digit(Scanner* scan);

/// @brief Handles string literals, "...", """..."""
/// @param scan The scanner from which to get the string
/// @return TKN_STRING or TKN_ERROR if an error is encountered
Token scan_handle_string(Scanner* scan);

/// @brief Handles char literals '.'
/// @param scan The scanner from which to get the char
/// @return TKN_CHAR or TKN_ERROR if an error is encountered
Token scan_handle_char(Scanner* scan);

/// @brief Handles +, +=, ++
/// @param scan The scanner from which to parse
/// @return Token representing the parsed lexeme
Token scan_handle_plus(Scanner* scan);

/// @brief Handles -, -=, --
/// @param scan The scanner from which to parse
/// @return Token representing the parsed lexeme
Token scan_handle_minus(Scanner* scan);

/// @brief Handles *, *=
/// @param scan The scanner from which to parse
/// @return Token representing the parsed lexeme
Token scan_handle_star(Scanner* scan);

/// @brief Handles /, /= and comments
/// @param scan The scanner from which to parse
/// @return Token representing the parsed lexeme
Token scan_handle_slash(Scanner* scan);

/// @brief Handles = and !=
/// @param scan The scanner from which to parse
/// @return Token representing the parsed lexeme
Token scan_handle_equal(Scanner* scan);

/// @brief Handles ., which can be a floating point
/// @param scan The scanner from which to parse
/// @return Token representing the parsed lexeme
Token scan_handle_dot(Scanner* scan);

/// @brief Handles <, <=, <<, <:
/// @param scan The scanner from which to parse
/// @return Token representing the parsed lexeme
Token scan_handle_less(Scanner* scan);

/// @brief Handles >, >=, >>
/// @param scan The scanner from which to parse
/// @return Token representing the parsed lexeme
Token scan_handle_greater(Scanner* scan);

/// @brief Handles &, &=, &&
/// @param scan The scanner from which to parse
/// @return Token representing the parsed lexeme
Token scan_handle_and(Scanner* scan);

/// @brief Handles |, |=, ||
/// @param scan The scanner from which to parse
/// @return Token representing the parsed lexeme
Token scan_handle_or(Scanner* scan);

/// @brief Handles ^, ^=
/// @param scan The scanner from which to parse
/// @return Token representing the parsed lexeme
Token scan_handle_xor(Scanner* scan);

/// @brief Handles comparisons for determining if an identifier is a keyword.
/// This function also handles bool literals, which are a special type of keywords.
/// @param scan The scanner whose 'parsed_string' is to compare
/// @return A Token representing a keyword, or TKN_IDENTIFIER
Token scan_get_identifier_or_keyword(Scanner* scan);

/// @brief Read more character, determining if the literal is a float or double
/// @param scan The scanner from which to read the next character
/// @return TKN_FLOAT if [fF], else TKN_DOUBLE [dD]
Token scan_get_floating_suffix(Scanner* scan);

/// @brief Converts a scanner's identifier string to a double.
/// This function also stores the result in the scanner's 'parsed_double',
/// and handles any error.
/// @param scan The scanner to modify
/// @return TKN_DOUBLE or TKN_ERROR if an error is encountered
Token scan_str_to_double(Scanner* scan);

/// @brief Converts a scanner's identifier string to a float.
/// This function also stores the result in the scanner's 'parsed_double',
/// and handles any error.
/// @param scan The scanner to modify
/// @return TKN_FLOAT or TKN_ERROR if an error is encountered
Token scan_str_to_float(Scanner* scan);

/// @brief Read more character, determining what type is the integral literal
/// @param scan The scanner from which to read the next character
/// @return TKN_U(8|16|32|64) if [uU]($1), or TKN_U(8|16|32|64) if [iI]($1), defaults to TKN_I32
Token scan_get_integral_suffix(Scanner* scan);

/// @brief Converts a scanner's identifier string to an int of base 'base'.
/// @param scan The scanner to modify
/// @param base The base of the int to parse
/// @return TKN_U64 or TKN_ERROR if an error is encountered
Token scan_str_to_u64(Scanner* scan, int base);

/// @brief Converts a scanner's identifier string to an int of base 'base'.
/// @param scan The scanner to modify
/// @param base The base of the int to parse
/// @return TKN_U32 or TKN_ERROR if an error is encountered
Token scan_str_to_u32(Scanner* scan, int base);

/// @brief Converts a scanner's identifier string to an int of base 'base'.
/// @param scan The scanner to modify
/// @param base The base of the int to parse
/// @return TKN_U16 or TKN_ERROR if an error is encountered
Token scan_str_to_u16(Scanner* scan, int base);

/// @brief Converts a scanner's identifier string to an int of base 'base'.
/// @param scan The scanner to modify
/// @param base The base of the int to parse
/// @return TKN_U8 or TKN_ERROR if an error is encountered
Token scan_str_to_u8(Scanner* scan, int base);

/// @brief Converts a scanner's identifier string to an int of base 'base'.
/// @param scan The scanner to modify
/// @param base The base of the int to parse
/// @return TKN_I64 or TKN_ERROR if an error is encountered
Token scan_str_to_i64(Scanner* scan, int base);

/// @brief Converts a scanner's identifier string to an int of base 'base'.
/// @param scan The scanner to modify
/// @param base The base of the int to parse
/// @return TKN_I32 or TKN_ERROR if an error is encountered
Token scan_str_to_i32(Scanner* scan, int base);

/// @brief Converts a scanner's identifier string to an int of base 'base'.
/// @param scan The scanner to modify
/// @param base The base of the int to parse
/// @return TKN_I16 or TKN_ERROR if an error is encountered
Token scan_str_to_i16(Scanner* scan, int base);

/// @brief Converts a scanner's identifier string to an int of base 'base'.
/// @param scan The scanner to modify
/// @param base The base of the int to parse
/// @return TKN_I8 or TKN_ERROR if an error is encountered
Token scan_str_to_i8(Scanner* scan, int base);

/// @brief Converts a string to the right integer type by checking for suffixes, and stores it
/// @param scan The scanner to modify
/// @return A Token representing an integral type
Token scan_str_to_integral(Scanner* scan);

/// @brief Adds characters to the scanner's identifier string while they are alpha or digits
/// @param scan The scanner to modify
char scan_parse_alnum(Scanner* scan);

/// @brief Adds characters to the scanner's identifier string while they are digits
/// @param scan The scanner to modify
char scan_parse_digits(Scanner* scan);

#endif //HG_COLT_SCANNER