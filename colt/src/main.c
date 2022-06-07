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
		AST ast;
		ASTInit(&ast);
		String file_content = StringGetFileContent(args.file_path_in);
		StringFree(&file_content);
		ASTFree(&ast);
	}
	DUMP_MEMORY_LEAKS();
}