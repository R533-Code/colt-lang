#include "precomph.h"
#include "vm/repl.h"

int main(int argc, const char** argv)
{
	ParseResult args = ParseArguments(argc, argv);
	if (args.file_path_in == NULL)
	{
		ColtREPL(args.byte_code_out);
	}
	else
	{
		String file_content = StringGetFileContent(args.file_path_in);
		//PARSE
		StringFree(&file_content);
	}
	DUMP_MEMORY_LEAKS();
}