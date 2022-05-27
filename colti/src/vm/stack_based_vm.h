/** @file stack_based_vm.h
* A stack based virtual machine implementation for Colt.
* The stack supports simple pushing and popping operations.
* To run code written in a Chunk, use StackVMRun(...) which takes
* in a StackVM* (which should be initialized using StackVMInit(...)) and a Chunk
* containing the code.
*/

#ifndef HG_COLTI_STACK_BASED_VM
#define HG_COLTI_STACK_BASED_VM

#include "common.h"

#include "byte-code/chunk.h"
#include "values/colti_floating_value.h"


namespace colti::vm
{
	/// @brief VM containing a stack
	class StackVM
	{
		/// @brief The pointer to the stack's top.
		/// Points to where the next push should be written.
		QWORD* stack_top;
		/// @brief The stack-allocated stack
		QWORD stack[256];

	public: //CONSTRUCTORS AND DESTRUCTORS
		/// @brief Initializes a StackVM
		StackVM() noexcept;

		/// @brief Frees the resources used by a StackVM
		~StackVM() noexcept;

	public:
		/// @brief Pushes a QWORD on the top the stack of a StackVM
		/// @param value The value to push
		void push(QWORD value) noexcept;

		/// @brief Pops and returns the top of a StackVM
		/// @return The popped QWORD
		QWORD pop() noexcept;

		/// @brief returns the top of a StackVM
		/// @return The top of the stack
		QWORD& top() noexcept;

		/// @brief Check if a stack of a StackVM is empty
		/// @return True if the stack was empty
		bool isEmpty() const noexcept;
	
		/// @brief Returns the number of QWORD pushed on the stack of a StackVM
		/// @return The count of QWORD pushed
		uint64_t size() const noexcept;

		/// @brief Runs code contained in a Chunk using an initialized StackVM
		/// @param chunk The chunk containing the code to run
		/// @return The result of the interpretation
		InterpretResult run(const Chunk& chunk) noexcept;
	};
}




#endif //HG_COLTI_STACK_BASED_VM