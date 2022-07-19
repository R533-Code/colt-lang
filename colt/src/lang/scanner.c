/** @file scanner.c
* Contains the definitions of the functions declared in 'scanner.h'
*/

#include "scanner.h"

void ScannerInit(Scanner* scan, StringView to_scan)
{
	colt_assert(scan != NULL, "Pointer was NULL!");
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

String* ScannerGetLString(const Scanner* scan)
{
	String* to_ret = safe_malloc(sizeof(String));
	StringCopy(to_ret, &scan->parsed_string);
	return to_ret;
}

const TypeInfo* ScannerGetTypeInfo(const Scanner* scan)
{
	switch (scan->parsed_value.u64)
	{
	case ID_COLT_VOID:
		return &ColtVoid;
	case ID_COLT_BOOL:
		return &ColtBool;
	case ID_COLT_I8:
		return &ColtI8;
	case ID_COLT_I16:
		return &ColtI16;
	case ID_COLT_I32:
		return &ColtI32;
	case ID_COLT_I64:
		return &ColtI64;
	case ID_COLT_U8:
		return &ColtU8;
	case ID_COLT_U16:
		return &ColtU16;
	case ID_COLT_U32:
		return &ColtU32;
	case ID_COLT_U64:
		return &ColtU64;
	case ID_COLT_FLOAT:
		return &ColtFloat;
	case ID_COLT_DOUBLE:
		return &ColtFloat;
	case ID_COLT_LSTRING:
		return &ColtLString;
	case ID_COLT_CHAR:
		return &ColtChar;
	default:
		colt_unreachable("Invalid argument!");
	}
}

QWORD ScannerGetParsedQWORD(const Scanner* scan)
{
	return scan->parsed_value;
}

Token ScannerGetNextToken(Scanner* scan)
{
	//We skip spaces
	while (isspace(scan->current_char))
	{
		if (scan->current_char == '\n')
			scan->current_line += 1;
		scan->current_char = scan_get_next_char(scan);
	}

	//we store the current offset, which is the beginning of the current lexeme
	scan->lexeme_begin = scan->offset - 1;

	if (isalpha(scan->current_char) || scan->current_char == '_')
		return scan_handle_identifier(scan);
	else if (isdigit(scan->current_char))
		return scan_handle_digit(scan);
	
	switch (scan->current_char)
	{	
	case '+':
		return scan_handle_plus(scan);
	case '-':
		return scan_handle_minus(scan);
	case '*':
		return scan_handle_star(scan);
	case '/':
		return scan_handle_slash(scan);
	case '.':
		return scan_handle_dot(scan);
	case '<':
		return scan_handle_less(scan);
	case '>':
		return scan_handle_greater(scan);
	case '&':
		return scan_handle_and(scan);
	case '|':
		return scan_handle_or(scan);
	case '^':
		return scan_handle_xor(scan);
	case '=':
		return scan_handle_equal(scan);
	case '"':
		return scan_handle_string(scan);
	case '\'':
		return scan_handle_char(scan);
	case ':':
		scan->current_char = scan_get_next_char(scan);
		if (scan->current_char == '>')
		{
			scan->current_char = scan_get_next_char(scan);
			return TKN_OPERATOR_COLON_GREATER;
		}
		return TKN_COLON;
	case '!':
		scan->current_char = scan_get_next_char(scan);
		if (scan->current_char == '=')
		{
			scan->current_char = scan_get_next_char(scan);
			return TKN_OPERATOR_BANG_EQUAL;
		}
		return TKN_OPERATOR_BANG;
	case '~':
		scan->current_char = scan_get_next_char(scan);
		return TKN_OPERATOR_TILDE;
	case ',':
		scan->current_char = scan_get_next_char(scan);
		return TKN_COMMA;
	case '{':
		scan->current_char = scan_get_next_char(scan);
		return TKN_LEFT_CURLY;
	case '}':
		scan->current_char = scan_get_next_char(scan);
		return TKN_RIGHT_CURLY;
	case '(':
		scan->current_char = scan_get_next_char(scan);
		return TKN_LEFT_PAREN;
	case ')':
		scan->current_char = scan_get_next_char(scan);
		return TKN_RIGHT_PAREN;
	case '[':
		scan->current_char = scan_get_next_char(scan);
		return TKN_LEFT_SQUARE;
	case ']':
		scan->current_char = scan_get_next_char(scan);
		return TKN_RIGHT_SQUARE;
	case ';':
		scan->current_char = scan_get_next_char(scan);
		return TKN_SEMICOLON;	
	case '%':
		scan->current_char = scan_get_next_char(scan);
		if (scan->current_char == '=')
		{
			scan->current_char = scan_get_next_char(scan);
			return TKN_OPERATOR_MODULO_EQUAL;
		}
		return TKN_OPERATOR_MODULO;
	}

	return TKN_EOF;
}

StringView ScannerGetCurrentLine(const Scanner* scan)
{
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

	StringView strv;
	strv.start = scan->view.start + line_begin;
	strv.end = scan->view.start + line_end;
	return strv;
}

StringView ScannerGetCurrentLexeme(const Scanner* scan)
{
	StringView strv;
	strv.start = scan->view.start + scan->lexeme_begin;
	strv.end = scan->view.start + scan->offset - 1;
	return strv;
}

/**********************************
IMPLEMENTATION HELPERS
**********************************/

void scan_print_error(const Scanner* scan, const char* error, ...)
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

void scan_print_unclosed_comment(const Scanner* scan)
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

char scan_get_next_char(Scanner* scan)
{
	if (scan->offset < (uint64_t)(scan->view.end - scan->view.start))
		return scan->view.start[scan->offset++];
	return EOF;
}

char scan_peek_next_char(const Scanner* scan, uint64_t offset)
{
	if (scan->offset + offset < (uint64_t)(scan->view.end - scan->view.start))
		return scan->view.start[scan->offset + offset];
	return EOF;
}

char impl_rewind_char(Scanner* scan)
{
	colt_assert(scan->offset > 1, "Should at least call scan_get_next_char 1 times before");
	//- 1 as the offset points to the NEXT character, not the current one
	return scan->view.start[--scan->offset - 1];
}

char scan_rewind_chars(Scanner* scan, uint64_t nb)
{
	colt_assert(scan->offset > 1, "Should at least call scan_get_next_char 'nb' times before!");
	//- 1 as the offset points to the NEXT character, not the current one
	return scan->view.start[(scan->offset -= nb) - 1];
}

Token scan_handle_identifier(Scanner* scan)
{
	//Clear the string
	StringClear(&scan->parsed_string);
	StringAppendChar(&scan->parsed_string, scan->current_char);
	
	scan->parsed_identifier.start = scan->view.start + scan->offset - 1;

	scan->current_char = scan_get_next_char(scan);
	while (isalnum(scan->current_char) || scan->current_char == '_')
	{
		StringAppendChar(&scan->parsed_string, scan->current_char);
		scan->current_char = scan_get_next_char(scan);
	}
	
	scan->parsed_identifier.end = scan->view.start + scan->offset - 1;
	return scan_get_identifier_or_keyword(scan);
}

Token scan_handle_digit(Scanner* scan)
{
	//Allows some optimization for when the AST needs to extract the values from the Scanner
	scan->parsed_value.u64 = 0;

	//Clear the string
	StringClear(&scan->parsed_string);
	StringAppendChar(&scan->parsed_string, scan->current_char);

	if (scan->current_char == '0') //Could be 0x, 0b, 0o
	{
		scan->current_char = scan_get_next_char(scan);
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
			if (isdigit(scan->current_char) || scan->current_char == '.')
			{
				//We recurse now that we have popped the leading 0
				return ScannerGetNextToken(scan);
			}
			else
				return token_str_to_integral(scan);
		}

		scan->current_char = scan_parse_alnum(scan);

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
			scan_print_error(scan, "'0%c' should be followed by characters in range %s!", scan->current_char, range_str);
			return TKN_ERROR;
		}
		return token_str_to_u64(scan, base);
	}

	//Parse as many digits as possible
	scan->current_char = scan_parse_digits(scan);

	bool isfloat = false;
	// [0-9]+ followed by a .[0-9] is a float
	if (scan->current_char == '.')
	{
		scan->current_char = scan_get_next_char(scan);
		if (isdigit(scan->current_char))
		{
			isfloat = true;
			StringAppendChar(&scan->parsed_string, '.');
			StringAppendChar(&scan->parsed_string, scan->current_char);
			
			//Parse as many digits as possible
			scan->current_char = scan_parse_digits(scan);
		}
		else
		{
			//The dot is not followed by a digit, this is not a float,
			//but rather should be the dot followed by an identifier for a function call
			scan->current_char = impl_rewind_char(scan);
			return token_str_to_u64(scan, 10);
		}
	}
	char after_e = scan_peek_next_char(scan, 0);
	// [0-9]+(.[0-9]+)?e[+-][0-9]+ is a float
	if (scan->current_char == 'e' && (after_e == '+' || after_e == '-' || isdigit(after_e)))
	{
		isfloat = true;
		StringAppendChar(&scan->parsed_string, scan->current_char);
		scan->current_char = scan_get_next_char(scan);
		if (scan->current_char == '+') //skip the + after the exponent
			scan->current_char = scan_get_next_char(scan);

		StringAppendChar(&scan->parsed_string, scan->current_char);
		
		//Parse as many digits as possible
		scan->current_char = scan_parse_digits(scan);
	}

	if (isfloat)
	{
		switch (scan_get_floating_suffix(scan))
		{
		case TKN_FLOAT:
			return token_str_to_float(scan);
		case TKN_DOUBLE:
			return token_str_to_double(scan);
		default:
			colt_unreachable("Floating suffix was invalid!");
		}
	}
	else
		return token_str_to_integral(scan);
}

Token scan_handle_string(Scanner* scan)
{
	StringClear(&scan->parsed_string);
	
	//Consume the "
	scan->current_char = scan_get_next_char(scan);

	while (scan->current_char != '"' && scan->current_char != '\n' && scan->current_char != EOF)
	{
		if (scan->current_char == '\\')
		{
			scan->current_char = scan_get_next_char(scan);
			switch (scan->current_char)
			{
			break; case '\'':
				StringAppendChar(&scan->parsed_string, '\'');
			break; case '\"':
				StringAppendChar(&scan->parsed_string, '\"');
			break; case '\\':
				StringAppendChar(&scan->parsed_string, '\\');
			break; case '0':
				StringAppendChar(&scan->parsed_string, '\0');
			break; case 'a':
				StringAppendChar(&scan->parsed_string, '\a');
			break; case 'b':
				StringAppendChar(&scan->parsed_string, '\b');
			break; case 'f':
				StringAppendChar(&scan->parsed_string, '\f');
			break; case 'n':
				StringAppendChar(&scan->parsed_string, '\n');
			break; case 'r':
				StringAppendChar(&scan->parsed_string, '\r');
			break; case 't':
				StringAppendChar(&scan->parsed_string, '\t');
			break; case 'v':
				StringAppendChar(&scan->parsed_string, '\v');
			break; default:
				//Consume whole string literal
				while (scan->current_char != ';' && scan->current_char != '\n' && scan->current_char != EOF)
					scan->current_char = scan_get_next_char(scan);
				scan_print_error(scan, "Invalid escape sequence!");
				return TKN_ERROR;
			}
			scan->current_char = scan_get_next_char(scan);
			continue;
		}
		StringAppendChar(&scan->parsed_string, scan->current_char);
		scan->current_char = scan_get_next_char(scan);
	}

	if (scan->current_char == '\n' || scan->current_char == EOF)
	{
		scan_print_error(scan, "Unterminated string literal!");
		return TKN_ERROR;
	}
	//Consume the "
	scan->current_char = scan_get_next_char(scan);
	return TKN_STRING;
}

Token scan_handle_char(Scanner* scan)
{
	//To simplify conversions
	scan->parsed_value.u64 = 0;
	
	//Consume the '
	scan->current_char = scan_get_next_char(scan);
	if (scan->current_char == '\'')
	{
		scan->current_char = scan_get_next_char(scan);
		scan_print_error(scan, "A char literal should at least contain a character!");
		return TKN_ERROR;
	}
	else if (scan->current_char == '\\')
	{
		scan->current_char = scan_get_next_char(scan);
		switch (scan->current_char)
		{
		break; case '\'':
			scan->parsed_value.c = '\'';
		break; case '\"':
			scan->parsed_value.c = '\"';
		break; case '\\':
			scan->parsed_value.c = '\\';
		break; case '0':
			scan->parsed_value.c = '\0';
		break; case 'a':
			scan->parsed_value.c = '\a';
		break; case 'b':
			scan->parsed_value.c = '\b';
		break; case 'f':
			scan->parsed_value.c = '\f';
		break; case 'n':
			scan->parsed_value.c = '\n';
		break; case 'r':
			scan->parsed_value.c = '\r';
		break; case 't':
			scan->parsed_value.c = '\t';
		break; case 'v':
			scan->parsed_value.c = '\v';
		break; default:
			//Consume whole string literal
			while (scan->current_char != ';' && scan->current_char != '\n' && scan->current_char != EOF && scan->current_char != '\'')
				scan->current_char = scan_get_next_char(scan);
			scan_print_error(scan, "Invalid escape sequence!");
			return TKN_ERROR;
		}
	}
	else
	{
		scan->parsed_value.c = scan->current_char;
	}
	scan->current_char = scan_get_next_char(scan);
	if (scan->current_char != '\'')
	{
		scan_print_error(scan, "Invalid char literal!");
		while (scan->current_char != ';' && scan->current_char != '\n' && scan->current_char != EOF && scan->current_char != '\'')
			scan->current_char = scan_get_next_char(scan);
		return TKN_ERROR;
	}
	//Consume the ''
	scan->current_char = scan_get_next_char(scan);
	return TKN_CHAR;
}

Token scan_handle_plus(Scanner* scan)
{
	scan->current_char = scan_get_next_char(scan);
	switch (scan->current_char)
	{
	break; case '=':
		scan->current_char = scan_get_next_char(scan);
		return TKN_OPERATOR_PLUS_EQUAL;
	break; case '+':
		scan->current_char = scan_get_next_char(scan);
		return TKN_OPERATOR_PLUS_PLUS;
	break; default:
		return TKN_OPERATOR_PLUS;
	}
}

Token scan_handle_minus(Scanner* scan)
{
	scan->current_char = scan_get_next_char(scan);
	switch (scan->current_char)
	{
	break; case '=':
		scan->current_char = scan_get_next_char(scan);
		return TKN_OPERATOR_MINUS_EQUAL;
	break; case '-':
		scan->current_char = scan_get_next_char(scan);
		return TKN_OPERATOR_MINUS_MINUS;
	break; default:
		return TKN_OPERATOR_MINUS;
	}
}

Token scan_handle_star(Scanner* scan)
{
	scan->current_char = scan_get_next_char(scan);
	if (scan->current_char == '=')
	{
		scan->current_char = scan_get_next_char(scan);
		return TKN_OPERATOR_STAR_EQUAL;
	}
	return TKN_OPERATOR_STAR;
}

Token scan_handle_slash(Scanner* scan)
{
	scan->current_char = scan_get_next_char(scan);
	switch (scan->current_char)
	{
	case '=':
		scan->current_char = scan_get_next_char(scan);
		return TKN_OPERATOR_SLASH_EQUAL;
	case '/': // one line comment
	{
		scan->current_char = scan_get_next_char(scan);
		while (scan->current_char != EOF && scan->current_char != '\n')
			scan->current_char = scan_get_next_char(scan);
		
		if (scan->current_char == '\n')
			scan->current_line++;
		return ScannerGetNextToken(scan); //recurse and return the token after the comment
	}
	case '*': // multi-line comment
	{
		//in the case of an unterminated multi-line comment, we want
		//to print the line of the beginning of the multi-line comment.
		size_t line_count = 0;
		scan->current_char = scan_get_next_char(scan);
		while (scan->current_char != EOF)
		{
			if (scan->current_char == '\n')
				line_count++;
			if (scan->current_char == '*')
			{
				scan->current_char = scan_get_next_char(scan);
				if (scan->current_char == '/')
				{
					//update the line count
					scan->current_line += line_count;

					//consume closing /
					scan->current_char = scan_get_next_char(scan);
					return ScannerGetNextToken(scan); //recurse and return the token after the comment
				}
			}
			scan->current_char = scan_get_next_char(scan);
		}
		scan_print_unclosed_comment(scan);
		return TKN_EOF; //Compilation should fail directly
	}
	default:
		return TKN_OPERATOR_SLASH;
	}
}

Token scan_handle_equal(Scanner* scan)
{
	scan->current_char = scan_get_next_char(scan);
	if (scan->current_char == '=')
	{
		scan->current_char = scan_get_next_char(scan);
		return TKN_OPERATOR_EQUAL_EQUAL;
	}
	return TKN_OPERATOR_EQUAL;
}

Token scan_handle_dot(Scanner* scan)
{
	scan->current_char = scan_get_next_char(scan);
	if (isdigit(scan->current_char))
	{
		//Clear the string
		StringClear(&scan->parsed_string);
		StringAppendChar(&scan->parsed_string, '.');
		StringAppendChar(&scan->parsed_string, scan->current_char);

		scan->current_char = scan_parse_digits(scan);

		char after_e = scan_peek_next_char(scan, 0);
		// [0-9]+(.[0-9]+)?e[+-][0-9]+ is a float
		if (scan->current_char == 'e' && (after_e == '+' || after_e == '-' || isdigit(after_e)))
		{
			StringAppendChar(&scan->parsed_string, scan->current_char);
			scan->current_char = scan_get_next_char(scan);
			if (scan->current_char == '+') //skip the + after the exponent
				scan->current_char = scan_get_next_char(scan);

			StringAppendChar(&scan->parsed_string, scan->current_char);

			//Parse as many digits as possible
			scan->current_char = scan_parse_digits(scan);
		}
		switch (scan_get_floating_suffix(scan))
		{
		case TKN_FLOAT:
			return token_str_to_float(scan);
		case TKN_DOUBLE:
			return token_str_to_double(scan);
		default:
			colt_unreachable("Floating suffix was invalid!");
		}
	}
	else
	{
		return TKN_DOT;
	}
}

Token scan_handle_less(Scanner* scan)
{
	scan->current_char = scan_get_next_char(scan);
	switch (scan->current_char)
	{
	break; case '=':
		scan->current_char = scan_get_next_char(scan);
		return TKN_OPERATOR_LESS_EQUAL;
	break; case '<':
		scan->current_char = scan_get_next_char(scan);
		if (scan->current_char == '=')
		{
			scan->current_char = scan_get_next_char(scan);
			return TKN_OPERATOR_LESS_LESS_EQUAL;
		}
		return TKN_OPERATOR_LESS_LESS;
	break; case ':':
		scan->current_char = scan_get_next_char(scan);
		return TKN_OPERATOR_LESS_COLON;
	break; default:
		return TKN_OPERATOR_LESS;
	}	
}

Token scan_handle_greater(Scanner* scan)
{
	scan->current_char = scan_get_next_char(scan);
	switch (scan->current_char)
	{
	break; case '=':
		scan->current_char = scan_get_next_char(scan);
		return TKN_OPERATOR_GREATER_EQUAL;
	break; case '>':
		scan->current_char = scan_get_next_char(scan);
		if (scan->current_char == '=')
		{
			scan->current_char = scan_get_next_char(scan);
			return TKN_OPERATOR_GREATER_GREATER_EQUAL;
		}
		return TKN_OPERATOR_GREATER_GREATER;
	break; default:
		return TKN_OPERATOR_GREATER;
	}
}

Token scan_handle_and(Scanner* scan)
{
	scan->current_char = scan_get_next_char(scan);
	switch (scan->current_char)
	{
	break; case '=':
		scan->current_char = scan_get_next_char(scan);
		return TKN_OPERATOR_AND_EQUAL;
	break; case '&':
		scan->current_char = scan_get_next_char(scan);
		return TKN_OPERATOR_AND_AND;
	break; default:
		return TKN_OPERATOR_AND;
	}
}

Token scan_handle_or(Scanner* scan)
{
	scan->current_char = scan_get_next_char(scan);
	switch (scan->current_char)
	{
	break; case '=':
		scan->current_char = scan_get_next_char(scan);
		return TKN_OPERATOR_OR_EQUAL;
	break; case '|':
		scan->current_char = scan_get_next_char(scan);
		return TKN_OPERATOR_OR_OR;
	break; default:
		return TKN_OPERATOR_OR;
	}
}

Token scan_handle_xor(Scanner* scan)
{
	scan->current_char = scan_get_next_char(scan);
	switch (scan->current_char)
	{
	break; case '=':
		scan->current_char = scan_get_next_char(scan);
		return TKN_OPERATOR_XOR_EQUAL;	
	break; default:
		return TKN_OPERATOR_XOR;
	}
}

Token scan_get_identifier_or_keyword(Scanner* scan)
{
	const char* str = scan->parsed_string.ptr;
	
	if (scan->parsed_string.size == 2)
		return TKN_IDENTIFIER;

	//GlobalTable of keywords
	//We optimize comparisons by comparing the first character
	switch (str[0])
	{
	break; case 'a':
		if (strcmp(str, "and") == 0)
			return TKN_OPERATOR_AND_AND;
	break; case 'b':
		if (strcmp(str, "break") == 0)
			return TKN_KEYWORD_BREAK;
		else if (strcmp(str, "bool") == 0)
		{
			scan->parsed_value.u64 = ID_COLT_BOOL;
			return TKN_BUILTIN_TYPE;
		}
	break; case 'c':
		if (strcmp(str, "char") == 0)
		{
			scan->parsed_value.u64 = ID_COLT_CHAR;
			return TKN_BUILTIN_TYPE;
		}
		else if (strcmp(str, "case") == 0)
			return TKN_KEYWORD_CASE;
		else if (strcmp(str, "const") == 0)
			return TKN_KEYWORD_CONST;
		else if (strcmp(str, "continue") == 0)
			return TKN_KEYWORD_CONTINUE;
	break; case 'd':
		if (strcmp(str, "double") == 0)
		{
			scan->parsed_value.u64 = ID_COLT_DOUBLE;
			return TKN_BUILTIN_TYPE;
		}
		else if (strcmp(str, "default") == 0)
			return TKN_KEYWORD_DEFAULT;
	break; case 'e':
		if (strcmp(str, "elif") == 0)
			return TKN_KEYWORD_ELIF;
		else if (strcmp(str, "else") == 0)
			return TKN_KEYWORD_ELSE;
	break; case 'f':
		if (strcmp(str, "for") == 0)
			return TKN_KEYWORD_FOR;
		else if (strcmp(str, "false") == 0)
		{
			scan->parsed_value.b = false;
			return TKN_BOOL;
		}
		else if (strcmp(str, "float") == 0)
		{
			scan->parsed_value.u64 = ID_COLT_FLOAT;
			return TKN_BUILTIN_TYPE;
		}
	break; case 'g':
		if (strcmp(str, "goto") == 0)
			return TKN_KEYWORD_GOTO;
	break; case 'i':
		if (strcmp(str, "if") == 0)
			return TKN_KEYWORD_IF;
		else if (strcmp(str, "i8") == 0)
		{
			scan->parsed_value.u64 = ID_COLT_I8;
			return TKN_BUILTIN_TYPE;
		}
		else if (strcmp(str, "i16") == 0)
		{
			scan->parsed_value.u64 = ID_COLT_I16;
			return TKN_BUILTIN_TYPE;
		}
		else if (strcmp(str, "i32") == 0)
		{
			scan->parsed_value.u64 = ID_COLT_I32;
			return TKN_BUILTIN_TYPE;
		}
		else if (strcmp(str, "i64") == 0)
		{
			scan->parsed_value.u64 = ID_COLT_I64;
			return TKN_BUILTIN_TYPE;
		}
	break; case 'l':
		if (strcmp(str, "lstring") == 0)
		{
			scan->parsed_value.u64 = ID_COLT_LSTRING;
			return TKN_BUILTIN_TYPE;
		}
	break; case 'o':
		if (strcmp(str, "or") == 0)
			return TKN_OPERATOR_OR_OR;
	break; case 's':
		if (strcmp(str, "switch") == 0)
			return TKN_KEYWORD_SWITCH;
		else if (strcmp(str, "static_print") == 0)
			return TKN_KEYWORD_STATIC_PRINT;
	break; case 't':
		if (strcmp(str, "true") == 0)
		{
			scan->parsed_value.b = true;
			return TKN_BOOL;
		}
	break; case 'u':
		///TODO: optimize in a finite automaton
		if (strcmp(str, "u8") == 0)
		{
			scan->parsed_value.u64 = ID_COLT_U8;
			return TKN_BUILTIN_TYPE;
		}
		else if (strcmp(str, "u16") == 0)
		{
			scan->parsed_value.u64 = ID_COLT_U16;
			return TKN_BUILTIN_TYPE;
		}
		else if (strcmp(str, "u32") == 0)
		{
			scan->parsed_value.u64 = ID_COLT_U32;
			return TKN_BUILTIN_TYPE;
		}
		else if (strcmp(str, "u64") == 0)
		{
			scan->parsed_value.u64 = ID_COLT_U64;
			return TKN_BUILTIN_TYPE;
		}
	break; case 'w':
		if (strcmp(str, "while") == 0)
			return TKN_KEYWORD_WHILE;
	break; case 'v':
		if (strcmp(str, "var") == 0)
			return TKN_KEYWORD_VAR;
	break; default:
		return TKN_IDENTIFIER;
	}
	return TKN_IDENTIFIER;
}

Token scan_get_floating_suffix(Scanner* scan)
{
	switch (tolower(scan->current_char))
	{
	break; case 'f':
		scan->current_char = scan_get_next_char(scan);
		return TKN_FLOAT;
	break; case 'd':
		scan->current_char = scan_get_next_char(scan);
		return TKN_DOUBLE;
	break; default:
		return TKN_DOUBLE;
	}
}

Token token_str_to_double(Scanner* scan)
{
	char* end;
	double value = strtod(scan->parsed_string.ptr, &end);
	if (end != scan->parsed_string.ptr + scan->parsed_string.size - 1)
	{
		scan_print_error(scan, "Unexpected character '%c' while parsing 'double' literal.", *end);
		return TKN_ERROR;
	}
	else if (value == HUGE_VAL && errno == ERANGE)
	{
		errno = 0;
		scan_print_error(scan, "'double' literal is not representable!");
		return TKN_ERROR;
	}
	scan->parsed_value.d = value;
	return TKN_DOUBLE;
}

Token token_str_to_float(Scanner* scan)
{
	char* end;
	float value = strtof(scan->parsed_string.ptr, &end);
	if (end != scan->parsed_string.ptr + scan->parsed_string.size - 1)
	{
		scan_print_error(scan, "Unexpected character '%c' while parsing 'float' literal.", *end);
		return TKN_ERROR;
	}
	else if (value == HUGE_VALF && errno == ERANGE)
	{
		errno = 0;
		scan_print_error(scan, "'float' literal is not representable!");
		return TKN_ERROR;
	}
	scan->parsed_value.f = value;
	return TKN_FLOAT;
}

Token scan_get_integral_suffix(Scanner* scan)
{
	//finite automata
	switch (tolower(scan->current_char))
	{
	break; case 'u':		
		switch (scan->current_char = scan_get_next_char(scan))
		{
		break; case '8':
			scan->current_char = scan_get_next_char(scan);
			return TKN_U8;
		break; case '1':
			scan->current_char = scan_get_next_char(scan);
			if (scan->current_char == '6')
			{
				scan->current_char = scan_get_next_char(scan);
				return TKN_U16;
			}
			else
			{
				scan->current_char = scan_rewind_chars(scan, 2);
			}
		break; case '3':
			scan->current_char = scan_get_next_char(scan);
			if (scan->current_char == '2')
			{
				scan->current_char = scan_get_next_char(scan);
				return TKN_U32;
			}
			else
			{
				scan->current_char = scan_rewind_chars(scan, 2);
			}
		break; case '6':
			scan->current_char = scan_get_next_char(scan);
			if (scan->current_char == '4')
			{
				scan->current_char = scan_get_next_char(scan);
				return TKN_U64;
			}
			else
			{
				scan->current_char = scan_rewind_chars(scan, 2);
			}
		}
	break; case 'i':
		switch (scan->current_char = scan_get_next_char(scan))
		{
		break; case '8':
			scan->current_char = scan_get_next_char(scan);
			return TKN_I8;
		break; case '1':
			scan->current_char = scan_get_next_char(scan);
			if (scan->current_char == '6')
			{
				scan->current_char = scan_get_next_char(scan);
				return TKN_I16;
			}
			else
			{
				scan->current_char = scan_rewind_chars(scan, 2);
			}
		break; case '3':
			scan->current_char = scan_get_next_char(scan);
			if (scan->current_char == '2')
			{
				scan->current_char = scan_get_next_char(scan);
				return TKN_I32;
			}
			else
			{
				scan->current_char = scan_rewind_chars(scan, 2);
			}
		break; case '6':
			scan->current_char = scan_get_next_char(scan);
			if (scan->current_char == '4')
			{
				scan->current_char = scan_get_next_char(scan);
				return TKN_I64;
			}
			else
			{
				scan->current_char = scan_rewind_chars(scan, 2);
			}
		}
	}
	return TKN_I32;
}

Token token_str_to_u64(Scanner* scan, int base)
{
	char* end;
	uint64_t value = strtoull(scan->parsed_string.ptr, &end, base);
	
	if (end != scan->parsed_string.ptr + scan->parsed_string.size - 1)
	{
		scan_print_error(scan, "Unexpected character '%c' while parsing 'u64' literal.", *end);
		return TKN_ERROR;
	}
	else if (value == ULLONG_MAX && errno == ERANGE)
	{
		errno = 0;
		scan_print_error(scan, "Unsigned integer literal is not representable in a 'u64'.");
		return TKN_ERROR;
	}
	scan->parsed_value.u64 = value;
	return TKN_U64;
}

Token token_str_to_i64(Scanner* scan, int base)
{
	char* end;
	int64_t value = strtoll(scan->parsed_string.ptr, &end, base);

	if (end != scan->parsed_string.ptr + scan->parsed_string.size - 1)
	{
		scan_print_error(scan, "Unexpected character '%c' while parsing 'i64' literal.", *end);
		return TKN_ERROR;
	}
	else if (value == LLONG_MAX && errno == ERANGE)
	{
		errno = 0;
		scan_print_error(scan, "Signed integer literal is not representable in a 'i64'.");
		return TKN_ERROR;
	}
	scan->parsed_value.i64 = value;
	return TKN_I64;
}

Token token_str_to_u32(Scanner* scan, int base)
{
	char* end;
	uint32_t value = strtoul(scan->parsed_string.ptr, &end, base);

	if (end != scan->parsed_string.ptr + scan->parsed_string.size - 1)
	{
		scan_print_error(scan, "Unexpected character '%c' while parsing 'u32' literal.", *end);
		return TKN_ERROR;
	}
	else if (value == ULONG_MAX && errno == ERANGE)
	{
		errno = 0;
		scan_print_error(scan, "Unsigned integer literal is not representable in a 'u32'.");
		return TKN_ERROR;
	}
	scan->parsed_value.u32 = value;
	return TKN_U32;
}

Token token_str_to_i32(Scanner* scan, int base)
{
	char* end;
	int32_t value = strtol(scan->parsed_string.ptr, &end, base);

	if (end != scan->parsed_string.ptr + scan->parsed_string.size - 1)
	{
		scan_print_error(scan, "Unexpected character '%c' while parsing 'i32' literal.", *end);
		return TKN_ERROR;
	}
	else if (value == LONG_MAX && errno == ERANGE)
	{
		errno = 0;
		scan_print_error(scan, "Signed integer literal is not representable in a 'i32'.");
		return TKN_ERROR;
	}
	scan->parsed_value.i32 = value;
	return TKN_I32;
}

Token token_str_to_u16(Scanner* scan, int base)
{
	char* end;
	uint32_t value = strtoul(scan->parsed_string.ptr, &end, base);

	if (end != scan->parsed_string.ptr + scan->parsed_string.size - 1)
	{
		scan_print_error(scan, "Unexpected character '%c' while parsing 'u16' literal.", *end);
		return TKN_ERROR;
	}
	else if ((value > USHRT_MAX) || errno == ERANGE)
	{
		errno = 0;
		scan_print_error(scan, "Unsigned integer literal is not representable in a 'u16'.");
		return TKN_ERROR;
	}
	scan->parsed_value.u16 = (uint16_t)value;
	return TKN_U16;
}

Token token_str_to_i16(Scanner* scan, int base)
{
	char* end;
	int32_t value = strtol(scan->parsed_string.ptr, &end, base);

	if (end != scan->parsed_string.ptr + scan->parsed_string.size - 1)
	{
		scan_print_error(scan, "Unexpected character '%c' while parsing 'i16' literal.", *end);
		return TKN_ERROR;
	}
	else if ((value > SHRT_MAX || value < SHRT_MIN) || errno == ERANGE)
	{
		errno = 0;
		scan_print_error(scan, "Signed integer literal is not representable in a 'i16'.");
		return TKN_ERROR;
	}
	scan->parsed_value.i16 = (int16_t)value;
	return TKN_I16;
}

Token token_str_to_u8(Scanner* scan, int base)
{
	char* end;
	//as no overload can convert a string to an uint8_t, we convert to a long value
	//then compare that to the range of a uint8_t
	uint32_t value = strtoul(scan->parsed_string.ptr, &end, base);

	if (end != scan->parsed_string.ptr + scan->parsed_string.size - 1)
	{
		scan_print_error(scan, "Unexpected character '%c' while parsing 'u8' literal.", *end);
		return TKN_ERROR;
	}
	//as we are converting a long, if an ERANGE error is returned, than
	//the value is of course out of the range of an uint8_t, so ||
	else if ((value > UCHAR_MAX) || errno == ERANGE)
	{
		errno = 0;
		scan_print_error(scan, "Unsigned integer literal is not representable in a 'u8'.");
		return TKN_ERROR;
	}
	scan->parsed_value.u8 = (uint8_t)value;
	return TKN_U8;
}

Token token_str_to_i8(Scanner* scan, int base)
{
	char* end;
	//as no overload can convert a string to an int8_t, we convert to a long value
	//then compare that to the range of an int8_t
	int32_t value = strtol(scan->parsed_string.ptr, &end, base);

	if (end != scan->parsed_string.ptr + scan->parsed_string.size - 1)
	{
		scan_print_error(scan, "Unexpected character '%c' while parsing 'i8' literal.", *end);
		return TKN_ERROR;
	}
	//as we are converting a long, if an ERANGE error is returned, than
	//the value is of course out of the range of an int8_t, so ||
	else if ((value > CHAR_MAX || value < CHAR_MIN) || errno == ERANGE)
	{
		errno = 0;
		scan_print_error(scan, "Signed integer literal is not representable in a 'i8'.");
		return TKN_ERROR;
	}
	scan->parsed_value.i8 = (int8_t)value;
	return TKN_I8;
}

Token token_str_to_integral(Scanner* scan)
{
	switch (scan_get_integral_suffix(scan))
	{
	case TKN_I8:
		return token_str_to_i8(scan, 10);
	case TKN_I16:
		return token_str_to_i16(scan, 10);
	case TKN_I32:
		return token_str_to_i32(scan, 10);
	case TKN_I64:
		return token_str_to_i64(scan, 10);
	case TKN_U8:
		return token_str_to_u8(scan, 10);
	case TKN_U16:
		return token_str_to_u16(scan, 10);
	case TKN_U32:
		return token_str_to_u32(scan, 10);
	case TKN_U64:
		return token_str_to_u64(scan, 10);
	default:
		colt_unreachable("Integral suffix was invalid!");
	}
}

char scan_parse_alnum(Scanner* scan)
{
	char next_char = scan_get_next_char(scan);
	while (isalnum(next_char))
	{
		StringAppendChar(&scan->parsed_string, next_char);
		next_char = scan_get_next_char(scan);
	}
	return next_char;
}

char scan_parse_digits(Scanner* scan)
{
	char next_char = scan_get_next_char(scan);
	while (isdigit(next_char))
	{
		StringAppendChar(&scan->parsed_string, next_char);
		next_char = scan_get_next_char(scan);
	}
	return next_char;
}
