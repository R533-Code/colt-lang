/** @file memory.c
* Contains the definitions of the functions declared in 'memory.h'
*/

#include "memory.h"

void* checked_malloc(size_t size)
{
	void* ptr = malloc(size);
	if (ptr) return ptr;

	printf(CONSOLE_FOREGROUND_BRIGHT_RED "Error: "CONSOLE_COLOR_RESET"Could not allocate memory!\n");
	DO_IF_DEBUG_BUILD((void)getc(stdin));
	exit(EXIT_OS_RESOURCE_FAILURE);
}

void checked_free(void* ptr)
{
	if (ptr)
	{
		free(ptr); return;
	}
	printf(CONSOLE_FOREGROUND_BRIGHT_RED "Error: "CONSOLE_COLOR_RESET"Pointer passed 'checked_free' was NULL!\n");
	DO_IF_DEBUG_BUILD((void)getc(stdin));
	exit(EXIT_OS_RESOURCE_FAILURE);
}