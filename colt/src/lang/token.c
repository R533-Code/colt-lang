/** @file token.c
* Contains the definitions of the functions declared in 'token.h'
*/

#include "token.h"

const char* TokenToString(Token tkn)
{
	switch (tkn) //Thank you REGEX
	{
	/*********************
	* OPERATORS
	*********************/

	case TKN_OPERATOR_PLUS:
		return "TKN_OPERATOR_PLUS";
	case TKN_OPERATOR_PLUS_PLUS:
		return "TKN_OPERATOR_PLUS_PLUS";
	case TKN_OPERATOR_PLUS_EQUAL:
		return "TKN_OPERATOR_PLUS_EQUAL";
	case TKN_OPERATOR_MINUS:
		return "TKN_OPERATOR_MINUS";
	case TKN_OPERATOR_MINUS_MINUS:
		return "TKN_OPERATOR_MINUS_MINUS";
	case TKN_OPERATOR_MINUS_EQUAL:
		return "TKN_OPERATOR_MINUS_EQUAL";
	case TKN_OPERATOR_STAR:
		return "TKN_OPERATOR_STAR";
	case TKN_OPERATOR_STAR_EQUAL:
		return "TKN_OPERATOR_STAR_EQUAL";
	case TKN_OPERATOR_SLASH:
		return "TKN_OPERATOR_SLASH";
	case TKN_OPERATOR_SLASH_EQUAL:
		return "TKN_OPERATOR_SLASH_EQUAL";
	case TKN_OPERATOR_LESS:
		return "TKN_OPERATOR_LESS";
	case TKN_OPERATOR_LESS_LESS:
		return "TKN_OPERATOR_LESS_LESS";
	case TKN_OPERATOR_LESS_EQUAL:
		return "TKN_OPERATOR_LESS_EQUAL";
	case TKN_OPERATOR_GREATER:
		return "TKN_OPERATOR_GREATER";
	case TKN_OPERATOR_GREATER_GREATER:
		return "TKN_OPERATOR_GREATER_GREATER";
	case TKN_OPERATOR_GREATER_EQUAL:
		return "TKN_OPERATOR_GREATER_EQUAL";
	case TKN_OPERATOR_LESS_COLON:
		return "TKN_OPERATOR_LESS_COLON";
	case TKN_OPERATOR_COLON_GREATER:
		return "TKN_OPERATOR_COLON_GREATER";
	case TKN_OPERATOR_EQUAL:
		return "TKN_OPERATOR_EQUAL";
	case TKN_OPERATOR_EQUAL_EQUAL:
		return "TKN_OPERATOR_EQUAL_EQUAL";
	case TKN_OPERATOR_BANG:
		return "TKN_OPERATOR_BANG";
	case TKN_OPERATOR_BANG_EQUAL:
		return "TKN_OPERATOR_BANG_EQUAL";
	case TKN_OPERATOR_AND:
		return "TKN_OPERATOR_AND";
	case TKN_OPERATOR_AND_EQUAL:
		return "TKN_OPERATOR_AND_EQUAL";
	case TKN_OPERATOR_AND_AND:
		return "TKN_OPERATOR_AND_AND";
	case TKN_OPERATOR_OR:
		return "TKN_OPERATOR_OR";
	case TKN_OPERATOR_OR_EQUAL:
		return "TKN_OPERATOR_OR_EQUAL";
	case TKN_OPERATOR_OR_OR:
		return "TKN_OPERATOR_OR_OR";
	case TKN_OPERATOR_XOR:
		return "TKN_OPERATOR_XOR";
	case TKN_OPERATOR_XOR_EQUAL:
		return "TKN_OPERATOR_XOR_EQUAL";
	case TKN_OPERATOR_TILDE:
		return "TKN_OPERATOR_TILDE";
	case TKN_OPERATOR_MODULO:
		return "TKN_OPERATOR_MODULO";

	/*********************
	* LITERALS
	*********************/

	case TKN_IDENTIFIER:
		return "TKN_IDENTIFIER";
	case TKN_STRING:
		return "TKN_STRING";
	case TKN_U8:
		return "TKN_U8";
	case TKN_I8:
		return "TKN_I8";
	case TKN_U16:
		return "TKN_U16";
	case TKN_I16:
		return "TKN_I16";
	case TKN_U32:
		return "TKN_U32";
	case TKN_I32:
		return "TKN_I32";
	case TKN_U64:
		return "TKN_U64";
	case TKN_I64:
		return "TKN_I64";
	case TKN_FLOAT:
		return "TKN_FLOAT";
	case TKN_DOUBLE:
		return "TKN_DOUBLE";
	case TKN_BOOL:
		return "TKN_BOOL";

	/*********************
	* KEYWORDS
	*********************/

	case TKN_KEYWORD_BREAK:
		return "TKN_KEYWORD_BREAK";
	case TKN_KEYWORD_CASE:
		return "TKN_KEYWORD_CASE";
	case TKN_KEYWORD_CONTINUE:
		return "TKN_KEYWORD_CONTINUE";
	case TKN_KEYWORD_DEFAULT:
		return "TKN_KEYWORD_DEFAULT";
	case TKN_KEYWORD_ELIF:
		return "TKN_KEYWORD_ELIF";
	case TKN_KEYWORD_ELSE:
		return "TKN_KEYWORD_ELSE";
	case TKN_KEYWORD_FOR:
		return "TKN_KEYWORD_FOR";
	case TKN_KEYWORD_GOTO:
		return "TKN_KEYWORD_GOTO";
	case TKN_KEYWORD_IF:
		return "TKN_KEYWORD_IF";
	case TKN_KEYWORD_SWITCH:
		return "TKN_KEYWORD_SWITCH";
	case TKN_KEYWORD_WHILE:
		return "TKN_KEYWORD_WHILE";
	case TKN_KEYWORD_VAR:
		return "TKN_KEYWORD_VAR";

	/*********************
	* MISCELLANEOUS
	*********************/

	case TKN_COMMA:
		return "TKN_COMMA";
	case TKN_DOT:
		return "TKN_DOT";
	case TKN_COLON:
		return "TKN_COLON";
	case TKN_SEMICOLON:
		return "TKN_SEMICOLON";

	case TKN_LEFT_CURLY:
		return "TKN_LEFT_CURLY";
	case TKN_RIGHT_CURLY:
		return "TKN_RIGHT_CURLY";
	case TKN_LEFT_PAREN:
		return "TKN_LEFT_PAREN";
	case TKN_RIGHT_PAREN:
		return "TKN_RIGHT_PAREN";
	case TKN_LEFT_SQUARE:
		return "TKN_LEFT_SQUARE";
	case TKN_RIGHT_SQUARE:
		return "TKN_RIGHT_SQUARE";

	case TKN_EOF:
		return "TKN_EOF";
	case TKN_ERROR:
		return "TKN_ERROR";
	default:
		return "UNKNOWN";
	}
}