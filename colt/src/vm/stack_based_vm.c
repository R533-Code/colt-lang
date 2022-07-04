/** @file stack_based_vm.c
* Contains the definitions of the functions declared in 'stack_based_vm.h'
*/

#include "stack_based_vm.h"

void StackVMInit(StackVM* vm)
{
	//Point to index 0 of the stack (which means empty)
	vm->stack_top = vm->stack;
	vm->active_locals = 0;
}

void StackVMFree(StackVM* vm)
{
	//Nothing to free, yet
}

void StackVMPush(StackVM* vm, QWORD value)
{
	colt_assert(StackVMSize(vm) != 256, "StackVM was full!");
	*vm->stack_top = value;
	vm->stack_top++;
}

QWORD StackVMTop(const StackVM* vm)
{
	colt_assert(!StackVMIsEmpty(vm), "Stack was empty!");
	return *(vm->stack_top - 1);
}

QWORD StackVMPop(StackVM* vm)
{
	colt_assert(!StackVMIsEmpty(vm), "Stack was empty!");
	return *(--vm->stack_top);
}

bool StackVMIsEmpty(const StackVM* vm)
{
	return vm->stack_top == vm->stack;
}

uint64_t StackVMSize(const StackVM* vm)
{
	return vm->stack_top - vm->stack;
}

int64_t StackVMRun(StackVM* vm, Chunk* chunk)
{
	uint8_t* ip = chunk->code + ChunkGetCODESection(chunk);
	if (ip == chunk->code)
	{
		print_error_string("Cannot run Chunk that does not contain byte-code!");
		exit(EXIT_USER_INVALID_INPUT);
	}
	if (ChunkGetABI(chunk) != COLTI_ABI)
	{
		fputs(CONSOLE_FOREGROUND_BRIGHT_RED "Error: " CONSOLE_COLOR_RESET "Cannot run Chunk as its ABI (", stderr);
		ChunkPrintABI(chunk, stderr);
		fputs(") does not match the VM's ABI (" COLTI_ABI_STRING ")!\n", stderr);
		exit(EXIT_USER_INVALID_INPUT);
	}

	for (;;)
	{
		switch (*(ip++)) //Dereferences then advances the pointer
		{

		/******************************************************/

		break; case OP_PUSH_BYTE:
		{
			QWORD qword = { .byte = unsafe_get_byte(&ip) };
			StackVMPush(vm, qword);
		}
		break; case OP_PUSH_WORD:
		{
			QWORD qword = { .word = unsafe_get_word(&ip) };
			StackVMPush(vm, qword);
		}
		break; case OP_PUSH_DWORD:
		{
			QWORD qword = { .dword = unsafe_get_dword(&ip) };
			StackVMPush(vm, qword);
		}
		break; case OP_PUSH_QWORD:
		{
			QWORD qword = unsafe_get_qword(&ip);
			StackVMPush(vm, qword);
		}

		/******************************************************/

		break;
		case OP_LOAD_GLOBAL:
		{
			QWORD offset = unsafe_get_qword(&ip);
			QWORD push = *(QWORD*)(chunk->code + offset.u64);
			StackVMPush(vm, push);
		}

		break;
		case OP_STORE_GLOBAL:
		{
			colt_assert(!StackVMIsEmpty(vm), "Stack was empty!");
			QWORD offset = unsafe_get_qword(&ip);
			*(QWORD*)(chunk->code + offset.u64) = StackVMTop(vm);
		}

		/******************************************************/

		break; case OP_PUSH_SCOPE:
		{
			DWORD offset = unsafe_get_dword(&ip);
			colt_assert(offset.u32 + vm->active_locals < 256, "Stack would overflow allocating new Scope!");
			vm->active_locals += offset.u32;
		}
		break; case OP_POP_SCOPE:
		{
			DWORD offset = unsafe_get_dword(&ip);
			colt_assert(offset.u32 <= vm->active_locals, "Invalid offset!");
			vm->active_locals -= offset.u32;
		}

		/******************************************************/

		break; case OP_SLOAD_LOCAL:
		{
			colt_assert((*ip) < 256, "Invalid offset!");
			StackVMPush(vm, vm->locals[(*ip)++]);
		}
		break; case OP_SSTORE_LOCAL:
		{
			colt_assert((*ip) < 256, "Invalid offset!");
			vm->locals[(*ip)++] = StackVMTop(vm);
		}

		/******************************************************/

		break;
		case OP_LOAD_LSTRING:
		{
			colt_assert(!StackVMIsEmpty(vm), "Stack was empty!");
			QWORD cptr = StackVMPop(vm);
			//Transform the offset to a pointer
			cptr.lstring = chunk->code + *(uint64_t*)(chunk->code + ChunkGetSTRINGSection(chunk) + (cptr.u64 + 1) * sizeof(QWORD));
			StackVMPush(vm, cptr);
		}

		break;
		case OP_STORE_LSTRING:
		{
			colt_assert(!StackVMIsEmpty(vm), "Stack was empty!");
			QWORD cptr = StackVMPop(vm);
			//help the compiler optimize
			const uint64_t count = unsafe_chunk_get_lstring_count(chunk);
			for (size_t i = 0; i < count; i++)
			{
				if (*(uint64_t*)(chunk->code + ChunkGetSTRINGSection(chunk) + (i + 1) * sizeof(QWORD)) == (cptr.lstring - (const char*)chunk->code))
				{
					cptr.u64 = i;
					StackVMPush(vm, cptr);
					break;
				}
			}			
		}

		/******************************************************/

		

		/******************************************************/

		break; case OP_NEGATE:
			colt_assert(!StackVMIsEmpty(vm), "Stack should contain at least 1 items!");
			StackVMPush(vm, OpCode_Negate(StackVMPop(vm), *(ip++)));

		break; case OP_CONVERT:
		{
			colt_assert(!StackVMIsEmpty(vm), "Stack should contain at least 1 items!");
			BuiltinTypeID from = *(ip++);
			BuiltinTypeID to = *(ip++);
			StackVMPush(vm, OpCode_Convert(StackVMPop(vm), from, to));
		}

		/******************************************************/

		break; case OP_ADD:
		{
			colt_assert(StackVMSize(vm) >= 2, "Stack should contain at least 2 items!");
			QWORD rhs = StackVMPop(vm); //pop in the opposite order
			QWORD lhs = StackVMPop(vm);
			StackVMPush(vm, OpCode_Sum(lhs, rhs, *(ip++)));
		}
		break; case OP_SUBTRACT:
		{
			colt_assert(StackVMSize(vm) >= 2, "Stack should contain at least 2 items!");
			QWORD rhs = StackVMPop(vm);
			QWORD lhs = StackVMPop(vm);
			StackVMPush(vm, OpCode_Difference(lhs, rhs, *(ip++)));
		}
		break; case OP_MULTIPLY:
		{
			colt_assert(StackVMSize(vm) >= 2, "Stack should contain at least 2 items!");
			QWORD rhs = StackVMPop(vm);
			QWORD lhs = StackVMPop(vm);
			StackVMPush(vm, OpCode_Multiply(lhs, rhs, *(ip++)));
		}
		break; case OP_DIVIDE:
		{
			colt_assert(StackVMSize(vm) >= 2, "Stack should contain at least 2 items!");
			QWORD rhs = StackVMPop(vm);
			QWORD lhs = StackVMPop(vm);
			StackVMPush(vm, OpCode_Divide(lhs, rhs, *(ip++)));
		}
		break; case OP_MODULO:
		{
			colt_assert(StackVMSize(vm) >= 2, "Stack should contain at least 2 items!");
			QWORD rhs = StackVMPop(vm);
			QWORD lhs = StackVMPop(vm);
			StackVMPush(vm, OpCode_Modulo(lhs, rhs, *(ip++)));
		}
		break; case OP_BIT_AND:
		{
			colt_assert(StackVMSize(vm) >= 2, "Stack should contain at least 2 items!");
			QWORD rhs = StackVMPop(vm);
			QWORD lhs = StackVMPop(vm);
			StackVMPush(vm, OpCode_BitAND(lhs, rhs, *(ip++)));
		}
		break; case OP_BIT_OR:
		{
			colt_assert(StackVMSize(vm) >= 2, "Stack should contain at least 2 items!");
			QWORD rhs = StackVMPop(vm);
			QWORD lhs = StackVMPop(vm);
			StackVMPush(vm, OpCode_BitOR(lhs, rhs, *(ip++)));
		}
		break; case OP_BIT_XOR:
		{
			colt_assert(StackVMSize(vm) >= 2, "Stack should contain at least 2 items!");
			QWORD rhs = StackVMPop(vm);
			QWORD lhs = StackVMPop(vm);
			StackVMPush(vm, OpCode_BitXOR(lhs, rhs, *(ip++)));
		}
		break; case OP_BIT_NOT:
		{
			colt_assert(StackVMSize(vm) >= 1, "Stack should contain at least 1 items!");
			QWORD value = StackVMPop(vm);
			StackVMPush(vm, OpCode_BitNOT(value, *(ip++)));
		}
		break; case OP_BIT_SHIFT_L:
		{
			colt_assert(StackVMSize(vm) >= 2, "Stack should contain at least 2 items!");
			QWORD rhs = StackVMPop(vm);
			QWORD lhs = StackVMPop(vm);
			StackVMPush(vm, OpCode_BitShiftL(lhs, rhs, *(ip++)));
		}
		break; case OP_BIT_SHIFT_R:
		{
			colt_assert(StackVMSize(vm) >= 2, "Stack should contain at least 2 items!");
			QWORD rhs = StackVMPop(vm);
			QWORD lhs = StackVMPop(vm);
			StackVMPush(vm, OpCode_BitShiftR(lhs, rhs, *(ip++)));
		}
		break; case OP_BOOL_NOT:
		{
			colt_assert(!StackVMIsEmpty(vm), "Stack was empty!");
			QWORD value = StackVMPop(vm);
			StackVMPush(vm, OpCode_BoolNot(value, *(ip++)));
		}
		break; case OP_CMP_GREATER:
		{
			colt_assert(StackVMSize(vm) >= 2, "Stack should contain at least 2 items!");
			QWORD rhs = StackVMPop(vm);
			QWORD lhs = StackVMPop(vm);
			StackVMPush(vm, OpCode_Greater(lhs, rhs, *(ip++)));
		}
		break; case OP_CMP_GREATER_EQ:
		{
			colt_assert(StackVMSize(vm) >= 2, "Stack should contain at least 2 items!");
			QWORD rhs = StackVMPop(vm);
			QWORD lhs = StackVMPop(vm);
			StackVMPush(vm, OpCode_GreaterEq(lhs, rhs, *(ip++)));
		}
		break; case OP_CMP_LESS:
		{
			colt_assert(StackVMSize(vm) >= 2, "Stack should contain at least 2 items!");
			QWORD rhs = StackVMPop(vm);
			QWORD lhs = StackVMPop(vm);
			StackVMPush(vm, OpCode_Less(lhs, rhs, *(ip++)));
		}
		break; case OP_CMP_LESS_EQ:
		{
			colt_assert(StackVMSize(vm) >= 2, "Stack should contain at least 2 items!");
			QWORD rhs = StackVMPop(vm);
			QWORD lhs = StackVMPop(vm);
			StackVMPush(vm, OpCode_LessEq(lhs, rhs, *(ip++)));
		}
		break; case OP_CMP_EQUAL:
		{
			colt_assert(StackVMSize(vm) >= 2, "Stack should contain at least 2 items!");
			QWORD rhs = StackVMPop(vm);
			QWORD lhs = StackVMPop(vm);
			StackVMPush(vm, OpCode_Equal(lhs, rhs, *(ip++)));
		}
		break; case OP_CMP_NOT_EQUAL:
		{
			colt_assert(StackVMSize(vm) >= 2, "Stack should contain at least 2 items!");
			QWORD rhs = StackVMPop(vm);
			QWORD lhs = StackVMPop(vm);
			StackVMPush(vm, OpCode_NotEqual(lhs, rhs, *(ip++)));
		}
		/******************************************************/

		break; case OP_SJUMP_GREATER:
		{
			colt_assert(StackVMSize(vm) >= 2, "Stack should contain at least 2 items!");
			QWORD rhs = *(vm->stack_top - 1);
			QWORD lhs = *(vm->stack_top - 2);
			if (OpCode_Greater(lhs, rhs, *(ip++)).b)
			{
				ip += *ip;
				continue;
			}
			ip++;
		}
		break; case OP_SJUMP_GREATER_EQ:
		{
			colt_assert(StackVMSize(vm) >= 2, "Stack should contain at least 2 items!");
			QWORD rhs = *(vm->stack_top - 1);
			QWORD lhs = *(vm->stack_top - 2);
			if (OpCode_GreaterEq(lhs, rhs, *(ip++)).b)
			{
				ip += *ip;
				continue;
			}
			ip++;
		}
		break; case OP_SJUMP_LESS:
		{
			colt_assert(StackVMSize(vm) >= 2, "Stack should contain at least 2 items!");
			QWORD rhs = *(vm->stack_top - 1);
			QWORD lhs = *(vm->stack_top - 2);
			if (OpCode_Less(lhs, rhs, *(ip++)).b)
			{
				ip += *ip;
				continue;
			}
			ip++;
		}
		break; case OP_SJUMP_LESS_EQ:
		{
			colt_assert(StackVMSize(vm) >= 2, "Stack should contain at least 2 items!");
			QWORD rhs = *(vm->stack_top - 1);
			QWORD lhs = *(vm->stack_top - 2);
			if (OpCode_LessEq(lhs, rhs, *(ip++)).b)
			{
				ip += *ip;
				continue;
			}
			ip++;
		}
		break; case OP_SJUMP_EQUAL:
		{
			colt_assert(StackVMSize(vm) >= 2, "Stack should contain at least 2 items!");
			QWORD rhs = *(vm->stack_top - 1);
			QWORD lhs = *(vm->stack_top - 2);
			if (OpCode_Equal(lhs, rhs, *(ip++)).b)
			{
				ip += *ip;
				continue;
			}
			ip++;
		}
		break; case OP_SJUMP_NOT_EQUAL:
		{
			colt_assert(StackVMSize(vm) >= 2, "Stack should contain at least 2 items!");
			QWORD rhs = *(vm->stack_top - 1);
			QWORD lhs = *(vm->stack_top - 2);
			if (OpCode_NotEqual(lhs, rhs, *(ip++)).b)
			{
				ip += *ip;
				continue;
			}
			ip++;
		}
		break; case OP_SJUMP_TRUE:
		{
			colt_assert(!StackVMIsEmpty(vm), "Stack was empty!");
			QWORD value = StackVMTop(vm);			
			if (value.b)
			{
				ip += *ip;
				continue;
			}
			ip++;
		}
		break; case OP_SJUMP_NOT_TRUE:
		{
			colt_assert(!StackVMIsEmpty(vm), "Stack was empty!");
			QWORD value = StackVMTop(vm);
			if (!value.b)
			{
				ip += *ip;
				continue;
			}
			ip++;
		}

		/******************************************************/

		break; case OP_PRINT:
		{
			colt_assert(!StackVMIsEmpty(vm), "Stack was empty!");
			OpCode_Print(StackVMTop(vm), *(ip++));
		}
		break; case OP_POP:
			colt_assert(!StackVMIsEmpty(vm), "Stack was empty!");
			StackVMPop(vm);
		break; case OP_RETURN:
			return 0;
		break; case OP_EXIT:
		{
			uint64_t size = StackVMSize(vm);
			printf("\nStack contains %"PRIu64" QWORD%c.", size, size == 1 ? '\0' : 's');
			return unsafe_get_qword(&ip).i64;
		}
		break; default:
			break;
		}
	}
}