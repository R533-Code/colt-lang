/** @file parse_args.c
* Contains the definitions of the functions declared in 'parse_args.h'
*/

#include "parse_args.h"

ParseResult ParseArguments(int argc, const char** argv)
{
	ParseResult result;
	memset(&result, 0, sizeof(ParseResult));
	
	for (size_t i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			CommandLineArgument arg = args_string_to_arg(argv[i]);
			switch (arg)
			{
				//Even though all the impl_{ARG} do not return, break to silence warnings
			break; case ARG_HELP:
				args_help(argc, argv, i);
			break; case ARG_ENUM:
				args_enum(argc, argv, i);
			break; case ARG_VERSION:
				args_version(argc, argv, i);
			break; case ARG_DISASSEMBLE:
				args_disassemble(argc, argv, i);
			break; case ARG_TEST_COLOR_CONSOLE:
				args_test_color(argc, argv, i);
			break; case ARG_EXEC_OUTPUT:
				//As the function will read 1 argument more, we need to update i
				result.file_path_out = args_exec_out(argc, argv, ++i);
			break; case ARG_BYTE_CODE_OUTPUT:
				//As the function will read 1 argument more, we need to update i
				result.byte_code_out = args_byte_out(argc, argv, ++i);
			break; case ARG_RUN_BYTE_CODE:
				args_run_byte(argc, argv, i);
			break; default:
				print_error_format("Unknown argument '%s'!\nUse '-e' or '--enum' to get the list of valid arguments.", argv[i]);
				exit(EXIT_USER_INVALID_INPUT);
			}
		}
		else
		{
			result.file_path_in = argv[i];
		}
	}
	//If the user passed an -o or -b, an input file SHOULD BE SPECIFIED
	if ((result.file_path_out != NULL) && result.file_path_in == NULL)
	{
		print_error_string("No input file!");
		exit(EXIT_USER_INVALID_INPUT);
	}
	return result;
}

bool checkIfValidFile(const char* path)
{
	FILE* file = fopen(path, "rb");
	if (file == NULL)
		return false;
	fclose(file);
	return true;
}

/*****************************************
IMPLEMENTATION HELPERS
*****************************************/

CommandLineArgument args_string_to_arg(const char* str)
{
	colt_assert(strlen(str) > 1, "String size should be greater than 1!");
	size_t length = strlen(str);
	if (str[0] != '-')
		return ARG_INVALID;
	else if (length == 2)
	{
		switch (str[length - 1])
		{
		case 'A':
			return ARG_ABI_VERSION;
		case 'e':
			return ARG_ENUM;
		case 'v':
			return ARG_VERSION;
		case 'h':
			return ARG_HELP;
		case 'd':
			return ARG_DISASSEMBLE;
		case 'o':
			return ARG_EXEC_OUTPUT;
		case 'b':
			return ARG_BYTE_CODE_OUTPUT;
		case 'r':
			return ARG_RUN_BYTE_CODE;
		default:
			return ARG_INVALID;
		}
	}
	else if (str[1] == '-') // --...
	{
		//We optimize comparing string by first checking for the first character after --
		switch (str[2])
		{
		case 'A':
			if (strcmp(str + 3, "BI") == 0)
				return ARG_ABI_VERSION;
			return ARG_INVALID;
		case 't':
			if (strcmp(str + 3, "est-color") == 0)
				return ARG_TEST_COLOR_CONSOLE;
			return ARG_INVALID;
		case 'e':
			if (strcmp(str + 3, "num") == 0)
				return ARG_ENUM;
			return ARG_INVALID;
		case 'v':
			if (strcmp(str + 3, "ersion") == 0) //we already checked for --v
				return ARG_VERSION;
			return ARG_INVALID;
		case 'h':
			if (strcmp(str + 3, "elp") == 0) //we already checked for --h
				return ARG_HELP;
			return ARG_INVALID;
		case 'd':
			if (strcmp(str + 3, "isassemble") == 0)
				return ARG_DISASSEMBLE;
			return ARG_INVALID;
		case 'o':
			if (strcmp(str + 3, "ut") == 0)
				return ARG_EXEC_OUTPUT;
			return ARG_INVALID;
		case 'b':
			if (strcmp(str + 3, "yte-out") == 0)
				return ARG_BYTE_CODE_OUTPUT;
			return ARG_INVALID;
		case 'r':
			if (strcmp(str + 3, "un") == 0)
				return ARG_RUN_BYTE_CODE;
			return ARG_INVALID;
		default:
			return ARG_INVALID;
		}
	}
	return ARG_INVALID;
}

void args_version(int argc, const char** argv, uint64_t offset)
{
	if (argc == 2)
	{
		fputs(CONSOLE_BACKGROUND_BRIGHT_MAGENTA CONSOLE_FOREGROUND_BLACK
			"COLT v" COLT_VERSION_STRING "("COLT_CONFIG_STRING") on " COLT_OS_STRING CONSOLE_COLOR_RESET "\n", stdout);
		exit(EXIT_NO_FAILURE);
	}
	else
	{
		args_print_invalid_combination(argc, argv, offset);
		exit(EXIT_USER_INVALID_INPUT);
	}
}

void args_abi_version(int argc, const char** argv, uint64_t offset)
{
	if (argc == 2)
	{
		fputs(CONSOLE_BACKGROUND_BRIGHT_MAGENTA CONSOLE_FOREGROUND_BLACK
			"COLTI ABI v" COLTI_ABI_STRING CONSOLE_COLOR_RESET "\n", stdout);
		exit(EXIT_NO_FAILURE);
	}
	else
	{
		args_print_invalid_combination(argc, argv, offset);
		exit(EXIT_USER_INVALID_INPUT);
	}
}

void args_disassemble(int argc, const char** argv, uint64_t offset)
{
	if (argc == 3)
	{
		if (checkIfValidFile(argv[2]))
		{
			Chunk chunk = ChunkDeserialize(argv[2]);
			ChunkDisassemble(&chunk, argv[2]);
			ChunkFree(&chunk);
			exit(EXIT_NO_FAILURE);
		}
		else
		{
			print_error_format("'%s' is not a valid path!", argv[2]);
			exit(EXIT_USER_INVALID_INPUT);
		}
	}
	else if (argc == 2)
	{
		fputs(CONSOLE_FOREGROUND_BRIGHT_CYAN"-d, --disassemble"CONSOLE_COLOR_RESET": Disassembles a serialized chunk of code (compiled byte-code), which usually ends with '.ctc'.\nUse: "CONSOLE_FOREGROUND_BRIGHT_CYAN"--disassemble"CONSOLE_FOREGROUND_BRIGHT_MAGENTA" <PATH>\n"CONSOLE_COLOR_RESET, stdout);
		exit(EXIT_USER_INVALID_INPUT);
	}
	else
	{
		args_print_invalid_combination(argc, argv, offset);
		exit(EXIT_USER_INVALID_INPUT);
	}
}

void args_help(int argc, const char** argv, uint64_t offset)
{
	if (argc == 2)
	{
		printf("Usage: "CONSOLE_FOREGROUND_BRIGHT_CYAN"colti" CONSOLE_FOREGROUND_BRIGHT_MAGENTA" <PATH>"CONSOLE_FOREGROUND_BRIGHT_GREEN" [-o <OUT_PATH>]\n\n"CONSOLE_COLOR_RESET);
		printf("Use "CONSOLE_FOREGROUND_BRIGHT_CYAN"-h"CONSOLE_COLOR_RESET" or "CONSOLE_FOREGROUND_BRIGHT_CYAN"--help"CONSOLE_COLOR_RESET" followed by any valid argument to get its documentation and use.\n");
		printf("Use "CONSOLE_FOREGROUND_BRIGHT_CYAN"-e"CONSOLE_COLOR_RESET" or "CONSOLE_FOREGROUND_BRIGHT_CYAN"--enum"CONSOLE_COLOR_RESET" to enumerate all valid arguments.\n");
		exit(EXIT_USER_INVALID_INPUT);
	}
	else if (argc == 3)
	{
		CommandLineArgument arg = args_string_to_arg(argv[2]);
		switch (arg)
		{
		break; case ARG_HELP:
			fputs(CONSOLE_FOREGROUND_BRIGHT_CYAN"-h, --help"CONSOLE_COLOR_RESET": Prints the purpose and use of an argument.\nUse: "CONSOLE_FOREGROUND_BRIGHT_CYAN"--help"CONSOLE_FOREGROUND_BRIGHT_MAGENTA" <ARG>\n"CONSOLE_COLOR_RESET, stdout);
		break; case ARG_ENUM:
			fputs(CONSOLE_FOREGROUND_BRIGHT_CYAN"-e, --enum"CONSOLE_COLOR_RESET": Prints all the possible valid arguments.\nUse: "CONSOLE_FOREGROUND_BRIGHT_CYAN"--enum\n"CONSOLE_COLOR_RESET, stdout);
		break; case ARG_VERSION:
			fputs(CONSOLE_FOREGROUND_BRIGHT_CYAN"-v, --version"CONSOLE_COLOR_RESET": Prints the version of the compiler/interpreter.\nUse: "CONSOLE_FOREGROUND_BRIGHT_CYAN"--version\n"CONSOLE_COLOR_RESET, stdout);
		break; case ARG_ABI_VERSION:
			fputs(CONSOLE_FOREGROUND_BRIGHT_CYAN"-A, --ABI"CONSOLE_COLOR_RESET": Prints the ABI version of the interpreter.\nUse: "CONSOLE_FOREGROUND_BRIGHT_CYAN"--ABI\n"CONSOLE_COLOR_RESET, stdout);
		break; case ARG_DISASSEMBLE:
			fputs(CONSOLE_FOREGROUND_BRIGHT_CYAN"-d, --disassemble"CONSOLE_COLOR_RESET": Disassembles a serialized chunk of code (compiled byte-code), which usually ends with '.ctc'.\nUse: "CONSOLE_FOREGROUND_BRIGHT_CYAN"--disassemble"CONSOLE_FOREGROUND_BRIGHT_MAGENTA" <PATH>\n"CONSOLE_COLOR_RESET, stdout);
		break; case ARG_EXEC_OUTPUT:
			fputs(CONSOLE_FOREGROUND_BRIGHT_CYAN"-o, --out"CONSOLE_COLOR_RESET": Specifies the executable output path.\nUse: "CONSOLE_FOREGROUND_BRIGHT_CYAN"--out"CONSOLE_FOREGROUND_BRIGHT_MAGENTA" <PATH>\n"CONSOLE_COLOR_RESET, stdout);
		break; case ARG_BYTE_CODE_OUTPUT:
			fputs(CONSOLE_FOREGROUND_BRIGHT_CYAN"-b, --byte-out"CONSOLE_COLOR_RESET": Specifies the byte-code output path.\nUse: "CONSOLE_FOREGROUND_BRIGHT_CYAN"--byte-out"CONSOLE_FOREGROUND_BRIGHT_MAGENTA" <PATH>\n"CONSOLE_COLOR_RESET, stdout);
		break; case ARG_RUN_BYTE_CODE:
			fputs(CONSOLE_FOREGROUND_BRIGHT_CYAN"-r, --run"CONSOLE_COLOR_RESET": Interpret serialized byte-code. To serialize byte-code, use "CONSOLE_FOREGROUND_BRIGHT_CYAN"-b"CONSOLE_COLOR_RESET".\nUse: "CONSOLE_FOREGROUND_BRIGHT_CYAN"--run"CONSOLE_FOREGROUND_BRIGHT_MAGENTA" <PATH>\n"CONSOLE_COLOR_RESET, stdout);
		break; case ARG_TEST_COLOR_CONSOLE:
			fputs(CONSOLE_FOREGROUND_BRIGHT_CYAN"--test-color"CONSOLE_COLOR_RESET": Prints colored output (as a test) to the terminal.\nUse: "CONSOLE_FOREGROUND_BRIGHT_CYAN"--test-color\n"CONSOLE_COLOR_RESET, stdout);
		break; default:
			args_print_invalid_combination(argc, argv, offset);
			exit(EXIT_USER_INVALID_INPUT);
		}
		exit(EXIT_NO_FAILURE);
	}
	else
	{
		args_print_invalid_combination(argc, argv, offset);
		exit(EXIT_USER_INVALID_INPUT);
	}
}

void args_enum(int argc, const char** argv, uint64_t offset)
{
	if (argc == 2)
	{
		fputs("The possible arguments are:"
			CONSOLE_FOREGROUND_BRIGHT_CYAN "\n\t-h, --help              " CONSOLE_COLOR_RESET "Prints the usage of other command-line argument"
			CONSOLE_FOREGROUND_BRIGHT_CYAN "\n\t-e, --enum              " CONSOLE_COLOR_RESET "Enumerate all valid command-line argument"
			CONSOLE_FOREGROUND_BRIGHT_CYAN "\n\t-v, --version           " CONSOLE_COLOR_RESET "Prints the version of the Colt Compiler/Interpreter"
			CONSOLE_FOREGROUND_BRIGHT_CYAN "\n\t-A, --ABI               " CONSOLE_COLOR_RESET "Prints the ABI version of the Colt interpreter"
			CONSOLE_FOREGROUND_BRIGHT_CYAN "\n\t-d, --disassemble       " CONSOLE_COLOR_RESET "Disassembles a serialized byte-code"
			CONSOLE_FOREGROUND_BRIGHT_CYAN "\n\t-o, --out               " CONSOLE_COLOR_RESET "Specifies the executable output name"
			CONSOLE_FOREGROUND_BRIGHT_CYAN "\n\t-b, --byte-code         " CONSOLE_COLOR_RESET "Serializes generated byte-code"
			CONSOLE_FOREGROUND_BRIGHT_CYAN "\n\t-r, --run               " CONSOLE_COLOR_RESET "Runs serialized byte-code"
			CONSOLE_FOREGROUND_BRIGHT_CYAN "\n\t--test-color            " CONSOLE_COLOR_RESET "Writes colored text to the terminal"
			"\n"
		, stdout);
		//Interpreter
		exit(EXIT_NO_FAILURE);
	}
	else
	{
		args_print_invalid_combination(argc, argv, offset);
		exit(EXIT_USER_INVALID_INPUT);
	}
}

const char* args_exec_out(int argc, const char** argv, size_t current_argc)
{
	if (current_argc == argc)
	{
		print_error_format("'%s' expects a file path!", argv[current_argc - 1]);
		exit(EXIT_USER_INVALID_INPUT);
	}
	if (args_string_to_arg(argv[current_argc]) == ARG_INVALID)
		return argv[current_argc];
	else
	{
		print_error_format("Expected a file path, not '%s'!", argv[current_argc]);
		exit(EXIT_USER_INVALID_INPUT);
	}
}

const char* args_byte_out(int argc, const char** argv, size_t current_argc)
{
	if (current_argc == argc)
	{
		print_error_format("'%s' expects a file path!", argv[current_argc - 1]);
		exit(EXIT_USER_INVALID_INPUT);
	}
	if (args_string_to_arg(argv[current_argc]) == ARG_INVALID)
		return argv[current_argc];
	else
	{
		print_error_format("Expected a file path, not '%s'!\n", argv[current_argc]);
		exit(EXIT_USER_INVALID_INPUT);
	}
}

void args_run_byte(int argc, const char** argv, size_t current_argc)
{
	if (argc == 3)
	{
		if (checkIfValidFile(argv[2]))
		{
			Chunk chunk = ChunkDeserialize(argv[2]);
			StackVM vm;
			StackVMInit(&vm);
			StackVMRun(&vm, &chunk);
			fputc('\n', stdout);
			StackVMFree(&vm);
			ChunkFree(&chunk);
			exit(EXIT_NO_FAILURE);
		}
		else
		{
			print_error_format("'%s' is not a valid path!", argv[2]);
			exit(EXIT_USER_INVALID_INPUT);
		}
	}
	else if (argc == 2)
	{
		fputs(CONSOLE_FOREGROUND_BRIGHT_CYAN"-r, --run"CONSOLE_COLOR_RESET": Interpret serialized byte-code. To serialize byte-code, use "CONSOLE_FOREGROUND_BRIGHT_CYAN"-b"CONSOLE_COLOR_RESET".\nUse: "CONSOLE_FOREGROUND_BRIGHT_CYAN"--run"CONSOLE_FOREGROUND_BRIGHT_MAGENTA" <PATH>\n"CONSOLE_COLOR_RESET, stdout);
		exit(EXIT_USER_INVALID_INPUT);
	}
	else
	{
		args_print_invalid_combination(argc, argv, current_argc);
		exit(EXIT_USER_INVALID_INPUT);
	}
}

void args_test_color(int argc, const char** argv, uint64_t offset)
{
	if (argc > 2)
	{
		args_print_invalid_combination(argc, argv, offset);
		exit(EXIT_USER_INVALID_INPUT);
	}
	int n;
	for (int i = 0; i < 11; i++) {
		for (int j = 0; j < 10; j++) {
			n = 10 * i + j;
			if (n > 107) break;
			printf("\033[%dm %3d\033[m", n, n);
		}
		printf("\n");
	}
	exit(EXIT_NO_FAILURE);
}

void args_print_invalid_combination(int argc, const char** argv, uint64_t offset)
{
	colt_assert(argc >= 2, "Expected 'argc' greater or equal to 2!");
	
	//We can not use print_error_format here as it adds a '\n' at the end
	printf(CONSOLE_FOREGROUND_BRIGHT_RED "Error: " CONSOLE_COLOR_RESET "Invalid argument combination for '%s'", argv[offset]);
	if (argc > offset)
	{
		fputc(':', stdout);
		for (size_t i = 2; i < argc; i++)
		{
			printf(" %s", argv[i]);
		}
	}
	fputs(".\n", stdout);
}