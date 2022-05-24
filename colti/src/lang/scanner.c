/** @file scanner.c
* Contains the definitions of the functions declared in 'scanner.h'
*/

#include "scanner.h"

void ScannerInit(Scanner* scan, StringView to_scan)
{
	colti_assert(scan != NULL, "Pointer was NULL!");
	memset(scan, 0, sizeof(Scanner));
	scan->view = to_scan;
	scan->current_line = 1; //the line number starts at 1
	scan->current_char = ' ';
	StringInit(&scan->parsed_string);
}

void ScannerFree(Scanner* scan)
{
	StringFree(&scan->parsed_string);
}

StringView ScannerGetIdentifier(const Scanner* scan)
{
	return scan->parsed_identifier;
}

String ScannerGetString(const Scanner* scan)
{
	return StringCopy(&scan->parsed_string);
}

double ScannerGetDouble(const Scanner* scan)
{
	return scan->parsed_double;
}

uint64_t ScannerGetInt(const Scanner* scan)
{
	return scan->parsed_uinteger;
}

Token ScannerGetNextToken(Scanner* scan)
{
	//We skip spaces
	while (isspace(scan->current_char))
	{
		if (scan->current_char == '\n')
			scan->current_line += 1;
		scan->current_char = impl_get_next_char(scan);
	}

	//we store the current offset, which is the beginning of the current lexeme
	scan->lexeme_begin = scan->offset - 1;

	if (isalpha(scan->current_char) || scan->current_char == '_')
		return impl_scanner_handle_identifier(scan);
	else if (isdigit(scan->current_char))
		return impl_scanner_handle_digit(scan);
	
	switch (scan->current_char)
	{	
	case '+':
		return impl_scanner_handle_plus(scan);
	case '-':
		return impl_scanner_handle_minus(scan);
	case '*':
		return impl_scanner_handle_star(scan);
	case '/':
		return impl_scanner_handle_slash(scan);
	case '.':
		return impl_scanner_handle_dot(scan);
	case '<':
		return impl_scanner_handle_less(scan);
	case '>':
		return impl_scanner_handle_greater(scan);
	case '"':
		return impl_scanner_handle_string(scan);
	case ':':
		scan->current_char = impl_get_next_char(scan);
		if (scan->current_char == '>')
		{
			return TKN_OPERATOR_COLON_GREATER;
		}
		return TKN_COLON;
	case ',':
		scan->current_char = impl_get_next_char(scan);
		return TKN_COMMA;
	case '{':
		scan->current_char = impl_get_next_char(scan);
		return TKN_LEFT_CURLY;
	case '}':
		scan->current_char = impl_get_next_char(scan);
		return TKN_RIGHT_CURLY;
	case '(':
		scan->current_char = impl_get_next_char(scan);
		return TKN_LEFT_PAREN;
	case ')':
		scan->current_char = impl_get_next_char(scan);
		return TKN_RIGHT_PAREN;
	case '[':
		scan->current_char = impl_get_next_char(scan);
		return TKN_LEFT_SQUARE;
	case ']':
		scan->current_char = impl_get_next_char(scan);
		return TKN_RIGHT_SQUARE;
	case ';':
		scan->current_char = impl_get_next_char(scan);
		return TKN_SEMICOLON;
	case '%':
		scan->current_char = impl_get_next_char(scan);
		return TKN_OPERATOR_MODULO;
	}

	return TKN_EOF;
}

/**********************************
IMPLEMENTATION HELPERS
**********************************/

void impl_scanner_print_error(const Scanner* scan, const char* error, ...)
{
	fprintf(stderr, CONSOLE_FOREGROUND_BRIGHT_RED"Error: "CONSOLE_COLOR_RESET"On line %"PRIu64": ", scan->current_line);
	
	//prints the error
	va_list args;
	va_start(args, error);
	vfprintf(stderr, error, args);
	va_end(args);
	fputc('\n', stderr);

	size_t line_begin = 0;
	//set the searching pointer to before the lexeme, and start searching backwards
	//for the first '\n', which would be the beginning of the newline.
	const char* newline = scan->view.start + scan->lexeme_begin;
	while (newline != scan->view.start)
	{
		if (*newline == '\n')
		{
			line_begin = newline - scan->view.start;
			break;
		}
		newline--;
	}
	if (*newline == '\n')
		line_begin++; // + 1 as we don't want the \n to be included

	//starts with the size of the string from the beginning of the line till the end of the whole string.
	size_t line_end = scan->view.end - newline;
	
	newline = scan->view.start + scan->lexeme_begin;

	while (newline != scan->view.end)
	{
		if (*newline == '\n')
		{
			line_end = newline - scan->view.start;
			break;
		}
		newline++;
	}
	if (*newline == '\n')
		line_end--; // - 1 as we don't want the \n included

	//To highlight the error lexeme, we need to break down the line in 3 parts:
	//The part before the error, the error, and the part after the error
	fprintf(stderr, "%.*s"CONSOLE_BACKGROUND_BRIGHT_RED"%.*s"CONSOLE_COLOR_RESET"%.*s\n",
		(uint32_t)(scan->lexeme_begin - line_begin), scan->view.start + line_begin,
		(uint32_t)(scan->offset - 1 - scan->lexeme_begin), scan->view.start + scan->lexeme_begin,
		(uint32_t)(line_end > (scan->offset - 2) ? line_end - (scan->offset - 2) : 0), scan->view.start + scan->offset - 1
	);
}

void impl_scanner_print_unclosed_comment(const Scanner* scan)
{
	fprintf(stderr, CONSOLE_FOREGROUND_BRIGHT_RED"Error: "CONSOLE_COLOR_RESET"On line %"PRIu64": Unterminated multi-line comment!\n", scan->current_line);
	size_t line_begin = 0;
	//set the searching pointer to before the lexeme, and start searching backwards
	//for the first '\n', which would be the beginning of the newline.
	const char* newline = scan->view.start + scan->lexeme_begin;
	while (newline != scan->view.start)
	{
		if (*newline == '\n')
		{
			line_begin = newline - scan->view.start + 1; // + 1 as we don't want the \n to be included
			break;
		}
		newline--;
	}
	//starts with the size of the string from the beginning of the line till the end of the whole string.
	size_t line_end = scan->view.end - newline;
	newline = scan->view.start + scan->lexeme_begin;

	while (newline != scan->view.end)
	{
		if (*newline == '\n')
		{
			line_end = newline - scan->view.start; // - 1 as we don't want the \n included
			break;
		}
		newline++;
	}

	fprintf(stderr, "%.*s"CONSOLE_BACKGROUND_BRIGHT_RED"%.*s"CONSOLE_COLOR_RESET"\n",
		(uint32_t)(scan->lexeme_begin - line_begin), scan->view.start + line_begin,
		(uint32_t)(line_end - scan->lexeme_begin), scan->view.start + scan->lexeme_begin);
}

char impl_get_next_char(Scanner* scan)
{
	if (scan->offset < (uint64_t)(scan->view.end - scan->view.start))
		return scan->view.start[scan->offset++];
	return EOF;
}

char impl_peek_next_char(const Scanner* scan, uint64_t offset)
{
	if (scan->offset + offset < (uint64_t)(scan->view.end - scan->view.start))
		return scan->view.start[scan->offset + offset];
	return EOF;
}

Token impl_scanner_handle_identifier(Scanner* scan)
{
	//Clear the string
	StringClear(&scan->parsed_string);
	StringAppendChar(&scan->parsed_string, scan->current_char);
	
	scan->parsed_identifier.start = scan->view.start + scan->offset - 1;

	scan->current_char = impl_get_next_char(scan);
	while (isalnum(scan->current_char) || scan->current_char == '_')
	{
		StringAppendChar(&scan->parsed_string, scan->current_char);
		scan->current_char = impl_get_next_char(scan);
	}
	
	scan->parsed_identifier.end = scan->view.start + scan->offset - 1;
	return impl_token_identifier_or_keyword(&scan->parsed_string);
}

Token impl_scanner_handle_digit(Scanner* scan)
{
	//Clear the string
	StringClear(&scan->parsed_string);
	StringAppendChar(&scan->parsed_string, scan->current_char);

	if (scan->current_char == '0') //Could be 0x, 0b, 0o
	{
		scan->current_char = impl_get_next_char(scan);
		int base = 10;
		switch (scan->current_char)
		{
		break; case 'x': //HEXADECIMAL
			base = 16;			
		break; case 'b': //BINARY
			base = 2;
		break; case 'o': //OCTAL	
			base = 8;
		break; default:
			if (!isdigit(scan->current_char) && scan->current_char != '.')
			{
				scan->parsed_uinteger = 0;
				return TKN_INTEGER;
			}
			else //We recurse now that we have popped the leading 0
				return impl_scanner_handle_digit(scan);
		}

		scan->current_char = impl_parse_alnum(scan);

		if (scan->parsed_string.size == 2) //Contains only the '0'
		{
			const char* range_str;
			switch (scan->current_char)
			{
			break; case 'x':
				range_str = "[0-9a-f]";
			break; case 'b':
				range_str = "[0-1]";
			break; case 'o':
				range_str = "[0-7]";
			break; default: //should never happen
				range_str = "ERROR";
			}
			impl_scanner_print_error(scan, "'0%c' should be followed by characters in range %s!", scan->current_char, range_str);
			return TKN_ERROR;
		}
		return impl_token_str_to_uinteger(scan, base);
	}

	//Parse as many digits as possible
	scan->current_char = impl_parse_digits(scan);

	bool isfloat = false;
	// [0-9]+ followed by a .[0-9] is a float
	if (scan->current_char == '.' && isdigit(impl_peek_next_char(scan, 0)))
	{
		isfloat = true;
		StringAppendChar(&scan->parsed_string, scan->current_char);

		//Parse as many digits as possible
		scan->current_char = impl_parse_digits(scan);
	}
	char after_e = impl_peek_next_char(scan, 0);
	// [0-9]+(.[0-9]+)?e[+-][0-9]+ is a float
	if (scan->current_char == 'e' && (after_e == '+' || after_e == '-' || isdigit(after_e)))
	{
		isfloat = true;
		StringAppendChar(&scan->parsed_string, scan->current_char);
		scan->current_char = impl_get_next_char(scan);
		if (scan->current_char == '+') //skip the + after the exponent
			scan->current_char = impl_get_next_char(scan);

		StringAppendChar(&scan->parsed_string, scan->current_char);
		
		//Parse as many digits as possible
		scan->current_char = impl_parse_digits(scan);
	}

	if (isfloat)
		return impl_token_str_to_double(scan);
	else
		return impl_token_str_to_uinteger(scan, 10);
}

Token impl_scanner_handle_string(Scanner* scan)
{
	StringClear(&scan->parsed_string);
	
	//Consume the "
	scan->current_char = impl_get_next_char(scan);

	while (scan->current_char != '"' && scan->current_char != '\n' && scan->current_char != EOF)
	{
		StringAppendChar(&scan->parsed_string, scan->current_char);
		scan->current_char = impl_get_next_char(scan);
	}

	if (scan->current_char == '\n' || scan->current_char == EOF)
	{
		impl_scanner_print_error(scan, "Unterminated string literal!");
		return TKN_ERROR;
	}
	//Consume the "
	scan->current_char = impl_get_next_char(scan);
	return TKN_STRING;
}

Token impl_scanner_handle_plus(Scanner* scan)
{
	scan->current_char = impl_get_next_char(scan);
	switch (scan->current_char)
	{
	break; case '=':
		scan->current_char = impl_get_next_char(scan);
		return TKN_OPERATOR_PLUS_EQUAL;
	break; case '+':
		scan->current_char = impl_get_next_char(scan);
		return TKN_OPERATOR_PLUS_PLUS;
	break; default:
		return TKN_OPERATOR_PLUS;
	}
}

Token impl_scanner_handle_minus(Scanner* scan)
{
	scan->current_char = impl_get_next_char(scan);
	switch (scan->current_char)
	{
	break; case '=':
		scan->current_char = impl_get_next_char(scan);
		return TKN_OPERATOR_MINUS_EQUAL;
	break; case '-':
		scan->current_char = impl_get_next_char(scan);
		return TKN_OPERATOR_MINUS_MINUS;
	break; default:
		return TKN_OPERATOR_MINUS;
	}
}

Token impl_scanner_handle_star(Scanner* scan)
{
	scan->current_char = impl_get_next_char(scan);
	if (scan->current_char == '=')
	{
		scan->current_char = impl_get_next_char(scan);
		return TKN_OPERATOR_STAR_EQUAL;
	}
	return TKN_OPERATOR_STAR;
}

Token impl_scanner_handle_slash(Scanner* scan)
{
	scan->current_char = impl_get_next_char(scan);
	switch (scan->current_char)
	{
	case '=':
		scan->current_char = impl_get_next_char(scan);
		return TKN_OPERATOR_SLASH_EQUAL;
	case '/': // one line comment
	{
		scan->current_char = impl_get_next_char(scan);
		while (scan->current_char != EOF && scan->current_char != '\n')
			scan->current_char = impl_get_next_char(scan);
		
		if (scan->current_char == '\n')
			scan->current_line++;
		return ScannerGetNextToken(scan); //recurse and return the token after the comment
	}
	case '*': // multi-line comment
	{
		//in the case of an unterminated multi-line comment, we want
		//to print the line of the beginning of the multi-line comment.
		size_t line_count = 0;
		scan->current_char = impl_get_next_char(scan);
		while (scan->current_char != EOF)
		{
			if (scan->current_char == '\n')
				line_count++;
			if (scan->current_char == '*')
			{
				scan->current_char = impl_get_next_char(scan);
				if (scan->current_char == '/')
				{
					//update the line count
					scan->current_line += line_count;

					//consume closing /
					scan->current_char = impl_get_next_char(scan);
					return ScannerGetNextToken(scan); //recurse and return the token after the comment
				}
			}
			scan->current_char = impl_get_next_char(scan);
		}
		impl_scanner_print_unclosed_comment(scan);
		return TKN_EOF; //Compilation should fail directly
	}
	default:
		return TKN_OPERATOR_SLASH;
	}
}

Token impl_scanner_handle_dot(Scanner* scan)
{
	if (isdigit(impl_peek_next_char(scan, 0)))
	{
		//Clear the string
		StringClear(&scan->parsed_string);
		StringAppendChar(&scan->parsed_string, scan->current_char);

		scan->current_char = impl_parse_digits(scan);

		char after_e = impl_peek_next_char(scan, 0);
		// [0-9]+(.[0-9]+)?e[+-][0-9]+ is a float
		if (scan->current_char == 'e' && (after_e == '+' || after_e == '-' || isdigit(after_e)))
		{
			StringAppendChar(&scan->parsed_string, scan->current_char);
			scan->current_char = impl_get_next_char(scan);
			if (scan->current_char == '+') //skip the + after the exponent
				scan->current_char = impl_get_next_char(scan);

			StringAppendChar(&scan->parsed_string, scan->current_char);

			//Parse as many digits as possible
			scan->current_char = impl_parse_digits(scan);
		}		
		return impl_token_str_to_double(scan);
	}
	else
		return TKN_DOT;
}

Token impl_scanner_handle_less(Scanner* scan)
{
	scan->current_char = impl_get_next_char(scan);
	switch (scan->current_char)
	{
	break; case '=':
		scan->current_char = impl_get_next_char(scan);
		return TKN_OPERATOR_LESS_EQUAL;
	break; case '<':
		scan->current_char = impl_get_next_char(scan);
		return TKN_OPERATOR_LESS_LESS;
	break; case ':':
		scan->current_char = impl_get_next_char(scan);
		return TKN_OPERATOR_LESS_COLON;
	break; default:
		return TKN_OPERATOR_LESS;
	}	
}

Token impl_scanner_handle_greater(Scanner* scan)
{
	scan->current_char = impl_get_next_char(scan);
	switch (scan->current_char)
	{
	break; case '=':
		scan->current_char = impl_get_next_char(scan);
		return TKN_OPERATOR_GREATER_EQUAL;
	break; case '>':
		scan->current_char = impl_get_next_char(scan);
		return TKN_OPERATOR_GREATER_GREATER;
	break; default:
		return TKN_OPERATOR_GREATER;
	}
}

Token impl_token_identifier_or_keyword(const String* string)
{
	const char* str = string->ptr;
	
	//TODO: optimize better using len
	//size - 1 as we don't care about comparing the NUL terminator
	//size_t len = string->size - 1;
	
	if (string->size == 2)
		return TKN_IDENTIFIER;

	//Table of keywords
	//We optimize comparisons by comparing the first character
	switch (str[0])
	{
	break; case 'a':
		if (strcmp(str, "and") == 0)
			return TKN_OPERATOR_AND_AND;
	break; case 'b':
		if (strcmp(str, "break") == 0)
			return TKN_KEYWORD_BREAK;
	break; case 'c':
		if (strcmp(str, "case") == 0)
			return TKN_KEYWORD_CASE;
		else if (strcmp(str, "continue") == 0)
			return TKN_KEYWORD_CONTINUE;
	break; case 'd':
		if (strcmp(str, "default") == 0)
			return TKN_KEYWORD_DEFAULT;
	break; case 'e':
		if (strcmp(str, "elif") == 0)
			return TKN_KEYWORD_ELIF;
		else if (strcmp(str, "else") == 0)
			return TKN_KEYWORD_ELSE;
	break; case 'f':
		if (strcmp(str, "for") == 0)
			return TKN_KEYWORD_FOR;
	break; case 'g':
		if (strcmp(str, "goto") == 0)
			return TKN_KEYWORD_GOTO;
	break; case 'i':
		if (strcmp(str, "if") == 0)
			return TKN_KEYWORD_IF;
	break; case 'o':
		if (strcmp(str, "or") == 0)
			return TKN_OPERATOR_OR_OR;
	break; case 's':
		if (strcmp(str, "switch") == 0)
			return TKN_KEYWORD_SWITCH;
	break; case 'w':
		if (strcmp(str, "while") == 0)
			return TKN_KEYWORD_WHILE;
	break; default:
		return TKN_IDENTIFIER;
	}
	return TKN_IDENTIFIER;
}

Token impl_token_str_to_double(Scanner* scan)
{
	char* end;
	double value = strtod(scan->parsed_string.ptr, &end);
	if (end != scan->parsed_string.ptr + scan->parsed_string.size - 1)
	{
		impl_scanner_print_error(scan, "Unexpected character '%c' while parsing floating point literal.", *end);
		return TKN_ERROR;
	}
	else if (value == HUGE_VAL && errno == ERANGE)
	{
		errno = 0;
		impl_scanner_print_error(scan, "Floating point literal is not representable.");
		return TKN_ERROR;
	}
	scan->parsed_double = value;
	return TKN_DOUBLE;
}

Token impl_token_str_to_uinteger(Scanner* scan, int base)
{
	char* end;
	uint64_t value = strtoull(scan->parsed_string.ptr, &end, base);
	
	if (end != scan->parsed_string.ptr + scan->parsed_string.size - 1)
	{
		impl_scanner_print_error(scan, "Unexpected character '%c' while parsing integer literal.", *end);
		return TKN_ERROR;
	}
	else if (value == ULLONG_MAX && errno == ERANGE)
	{
		errno = 0;
		impl_scanner_print_error(scan, "Integer literal is not representable.");
		return TKN_ERROR;
	}
	scan->parsed_uinteger = value;
	return TKN_INTEGER;
}

char impl_parse_alnum(Scanner* scan)
{
	char next_char = impl_get_next_char(scan);
	while (isalnum(next_char))
	{
		StringAppendChar(&scan->parsed_string, next_char);
		next_char = impl_get_next_char(scan);
	}
	return next_char;
}

char impl_parse_digits(Scanner* scan)
{
	char next_char = impl_get_next_char(scan);
	while (isdigit(next_char))
	{
		StringAppendChar(&scan->parsed_string, next_char);
		next_char = impl_get_next_char(scan);
	}
	return next_char;
}
