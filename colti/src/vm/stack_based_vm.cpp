/** @file stack_based_vm.c
* Contains the definitions of the functions declared in 'stack_based_vm.h'
*/

#include "stack_based_vm.h"

namespace colti::vm
{
	StackVM::StackVM() noexcept
		: stack_top(stack)
	{}

	StackVM::~StackVM() noexcept
	{
		//Nothing to free (right now, might change)
	}

	void StackVM::push(QWORD value) noexcept
	{
		this->stack_top = value;
		this->stack_top++;
	}

	QWORD& StackVM::top() noexcept
	{
		colti_assert(!this->isEmpty(), "Stack was empty!");
		return *(this->stack_top - 1);
	}

	QWORD StackVM::pop() noexcept
	{
		return *(--this->stack_top);
	}

	bool StackVM::isEmpty() const noexcept
	{
		return this->stack_top == this->stack;
	}

	uint64_t StackVM::size() const noexcept
	{
		return this->stack_top - this->stack;
	}

	InterpretResult StackVM::run(const Chunk& chunk) noexcept
	{
		using namespace colti::code;

		uint8_t* ip = chunk.code;
		for (;;)
		{
			switch (*(ip++)) //Dereferences then advances the pointer
			{

				/******************************************************/

			break; case Opcode::OP_IMMEDIATE_BYTE:
			{
				QWORD qword = { .byte = Chunk::unsafe_get_byte(ip) };
				this->push(qword);
			}
			break; case Opcode::OP_IMMEDIATE_WORD:
			{
				QWORD qword = { .word = Chunk::unsafe_get_word(ip) };
				this->push(qword);
			}
			break; case Opcode::OP_IMMEDIATE_DWORD:
			{
				QWORD qword = { .dword = Chunk::unsafe_get_dword(ip) };
				this->push(qword);
			}
			break; case Opcode::OP_IMMEDIATE_QWORD:
			{
				QWORD qword = Chunk::unsafe_get_qword(ip);
				this->push(qword);
			}

			/******************************************************/

			break; case OpCode::OP_NEGATE:
				colti_assert(!this->isEmpty(), "Stack should contain at least 1 items!");
				this->push(OpCode_Negate(this->pop(), static_cast<OperandType>(*(ip++))));

			break; case OpCode::OP_CONVERT:
			{
				colti_assert(!this->isEmpty(), "Stack should contain at least 1 items!");
				OperandType from = static_cast<OperandType>(*(ip++));
				OperandType to = static_cast<OperandType>(*(ip++));
				this->push(OpCode_Convert(this->pop(), from, to));
			}

			/******************************************************/

			break; case OpCode::OP_ADD:
			{
				colti_assert(this->size() >= 2, "Stack should contain at least 2 items!");
				QWORD val1 = this->pop();
				QWORD val2 = this->pop();
				this->push(OpCode_Sum(val1, val2, static_cast<OperandType>(*(ip++))));
			}
			break; case OpCode::OP_SUBTRACT:
			{
				colti_assert(this->size() >= 2, "Stack should contain at least 2 items!");
				QWORD val1 = this->pop();
				QWORD val2 = this->pop();
				this->push(OpCode_Difference(val1, val2, static_cast<OperandType>(*(ip++))));
			}
			break; case OpCode::OP_MULTIPLY:
			{
				colti_assert(this->size() >= 2, "Stack should contain at least 2 items!");
				QWORD val1 = this->pop();
				QWORD val2 = this->pop();
				this->push(OpCode_Multiply(val1, val2, static_cast<OperandType>(*(ip++))));
			}
			break; case OpCode::OP_DIVIDE:
			{
				colti_assert(this->size() >= 2, "Stack should contain at least 2 items!");
				QWORD val1 = this->pop();
				QWORD val2 = this->pop();
				this->push(OpCode_Difference(val1, val2, static_cast<OperandType>(*(ip++))));
			}
			/******************************************************/

			break; case OpCode::OP_PRINT:
			{
				colti_assert(!StackVMIsEmpty(vm), "Stack was empty!");
				OpCode_Print(this->top(), static_cast<OperandType>(*(ip++)));
			}
			break; case OpCode::OP_RETURN:
				return INTERPRET_OK;

			break; default:
				break;
			}
		}
	}
}