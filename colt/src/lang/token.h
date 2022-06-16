/** @file token.h
* Contains the tokens that can be returned from the scanner.
*/

#ifndef HG_COLT_TOKEN
#define HG_COLT_TOKEN

/// @brief Enum representing individual lexemes of the Colt language
typedef enum
{
	/*********************
	* OPERATORS
	*********************/

	/// @brief +
	TKN_OPERATOR_PLUS,
	/// @brief ++
	TKN_OPERATOR_PLUS_PLUS,
	/// @brief +=
	TKN_OPERATOR_PLUS_EQUAL,

	/// @brief -
	TKN_OPERATOR_MINUS,
	/// @brief --
	TKN_OPERATOR_MINUS_MINUS,
	/// @brief -=
	TKN_OPERATOR_MINUS_EQUAL,

	/// @brief *
	TKN_OPERATOR_STAR,
	/// @brief *=
	TKN_OPERATOR_STAR_EQUAL,

	/// @brief /
	TKN_OPERATOR_SLASH,
	/// @brief /=
	TKN_OPERATOR_SLASH_EQUAL,

	/// @brief <
	TKN_OPERATOR_LESS,
	/// @brief <<
	TKN_OPERATOR_LESS_LESS,
	/// @brief <=
	TKN_OPERATOR_LESS_EQUAL,

	/// @brief >
	TKN_OPERATOR_GREATER,
	/// @brief >>
	TKN_OPERATOR_GREATER_GREATER,
	/// @brief >=
	TKN_OPERATOR_GREATER_EQUAL,

	/// @brief =
	TKN_OPERATOR_EQUAL,
	/// @brief ==
	TKN_OPERATOR_EQUAL_EQUAL,

	/// @brief !
	TKN_OPERATOR_BANG,
	/// @brief !=
	TKN_OPERATOR_BANG_EQUAL,

	/// @brief &
	TKN_OPERATOR_AND,
	/// @brief &=
	TKN_OPERATOR_AND_EQUAL,
	/// @brief && or 'and'
	TKN_OPERATOR_AND_AND,

	/// @brief |
	TKN_OPERATOR_OR,
	/// @brief |=
	TKN_OPERATOR_OR_EQUAL,
	/// @brief || or 'or'
	TKN_OPERATOR_OR_OR,

	/// @brief ^
	TKN_OPERATOR_XOR,
	/// @brief ^=
	TKN_OPERATOR_XOR_EQUAL,

	/// @brief ~
	TKN_OPERATOR_TILDE,

	/// @brief %
	TKN_OPERATOR_MODULO,

	/// @brief <:
	TKN_OPERATOR_LESS_COLON,
	/// @brief :>
	TKN_OPERATOR_COLON_GREATER,

	/*********************
	* LITERALS
	*********************/

	/// @brief [a-zA-Z][a-zA-Z0-9]*
	TKN_IDENTIFIER,
	/// @brief ".*?"
	TKN_STRING,
	/// @brief [0-9]+[uU]8
	TKN_U8,
	/// @brief [0-9]+[iI]8
	TKN_I8,
	/// @brief [0-9]+[uU]16
	TKN_U16,
	/// @brief [0-9]+[iI]16
	TKN_I16,
	/// @brief [0-9]+[uU]32
	TKN_U32,
	/// @brief [0-9]+(?:[iI]32)?
	TKN_I32,
	/// @brief [0-9]+[uU]64
	TKN_U64,
	/// @brief [0-9]+[iI]64
	TKN_I64,
	/// @brief [-+]?[0-9]*.[0-9]+(e[-+][0-9]+)[fF]?
	TKN_FLOAT,
	/// @brief [-+]?[0-9]*.[0-9]+(e[-+][0-9]+)[dD]?
	TKN_DOUBLE,
	/// @brief true|false
	TKN_BOOL,
	/// @brief bool, float, double, [ui](8|16|32|64)
	TKN_BUILTIN_TYPE,

	/*********************
	* KEYWORDS
	*********************/

	/// @brief break
	TKN_KEYWORD_BREAK,
	/// @brief case
	TKN_KEYWORD_CASE,
	/// @brief continue
	TKN_KEYWORD_CONTINUE,
	/// @brief default
	TKN_KEYWORD_DEFAULT,
	/// @brief elif
	TKN_KEYWORD_ELIF,
	/// @brief else
	TKN_KEYWORD_ELSE,
	/// @brief for
	TKN_KEYWORD_FOR,
	/// @brief goto
	TKN_KEYWORD_GOTO,
	/// @brief if
	TKN_KEYWORD_IF,
	/// @brief switch
	TKN_KEYWORD_SWITCH,
	/// @brief while
	TKN_KEYWORD_WHILE,
	/// @brief var
	TKN_KEYWORD_VAR,
	/// @brief static_print
	TKN_KEYWORD_STATIC_PRINT,

	/*********************
	* MISCELLANEOUS
	*********************/

	/// @brief ,
	TKN_COMMA,
	/// @brief '.'
	TKN_DOT,
	/// @brief :
	TKN_COLON,
	/// @brief ';'
	TKN_SEMICOLON,

	/// @brief {
	TKN_LEFT_CURLY,
	/// @brief }
	TKN_RIGHT_CURLY,
	/// @brief (
	TKN_LEFT_PAREN,
	/// @brief )
	TKN_RIGHT_PAREN,
	/// @brief [
	TKN_LEFT_SQUARE,
	/// @brief ]
	TKN_RIGHT_SQUARE,


	/// @brief Returned after the whole string is parsed
	TKN_EOF,
	/// @brief Returned for unterminated string and not recognized characters
	TKN_ERROR
} Token;

/// @brief Converts a Token to a c-string
/// @param tkn The token to convert
/// @return A string representing the valid Token or UNKNOWN
const char* TokenToString(Token tkn);

#endif //HG_COLT_TOKEN