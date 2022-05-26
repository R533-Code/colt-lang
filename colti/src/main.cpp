#include "precomph.h"

void debug_scan(StringView view)
{
	Scanner scan;
	ScannerInit(&scan, view);
	Token tkn = ScannerGetNextToken(&scan);
	do
	{
		switch (tkn)
		{
		break; case TKN_DOUBLE:
			printf("%s: %g\n", TokenToString(tkn), scan.parsed_double);
		break; case TKN_INTEGER:
			printf("%s: %"PRIu64"\n", TokenToString(tkn), scan.parsed_uinteger);
		break; case TKN_IDENTIFIER:
			printf("%s: %s\n", TokenToString(tkn), scan.parsed_string.ptr);
		break; case TKN_STRING:
			printf("%s: %s\n", TokenToString(tkn), scan.parsed_string.ptr);
		break; default:
			printf("%s\n", TokenToString(tkn));
		}
		tkn = ScannerGetNextToken(&scan);
	} while (tkn != TKN_EOF);
	ScannerFree(&scan);
}

int main(int argc, const char** argv)
{
	ParseResult args = ParseArguments(argc, argv);
	if (args.file_path_in == NULL)
	{
		while (!feof(stdin))
		{
			printf(CONSOLE_FOREGROUND_BRIGHT_MAGENTA"> "CONSOLE_COLOR_RESET);
			String line = StringGetLine();
			debug_scan(StringToStringView(&line));
			StringFree(&line);
		}
	}
	else
	{
		String file_content = StringGetFileContent(args.file_path_in);
		debug_scan(StringToStringView(&file_content));
		StringFree(&file_content);
	}
	DUMP_MEMORY_LEAKS();
}