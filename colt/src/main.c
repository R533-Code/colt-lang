#include "precomph.h"
#include "vm/repl.h"

void ColtRun(const char* byte_code_out, StringView to_parse)
{
	AST ast;
	ASTInit(&ast);
	if (ASTParse(&ast, to_parse))
	{
		Chunk chunk;
		ChunkInit(&chunk);
		if (generateByteCode(&chunk, &ast.table, &ast.expr, false))
		{
			if (byte_code_out != NULL)
				ChunkSerialize(&chunk, byte_code_out);
			StackVM vm;
			StackVMInit(&vm);
			StackVMRun(&vm, &chunk);
			fputc('\n', stdout);
			StackVMFree(&vm);
		}
		ChunkFree(&chunk);
	}
	ASTFree(&ast);
}

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
		//Parses and run if valid.
		ColtRun(args.byte_code_out, StringToStringViewWithNUL(&file_content));		
		StringFree(&file_content);
	}
	DUMP_MEMORY_LEAKS();
}