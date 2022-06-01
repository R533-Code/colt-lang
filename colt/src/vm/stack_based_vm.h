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

/// @brief VM containing a stack
typedef struct
{
	/// @brief The pointer to the stack's top.
	/// Points to where the next push should be written.
	QWORD* stack_top;
	/// @brief The stack-allocated stack
	QWORD stack[256];
} StackVM;

/// @brief Initializes a StackVM
/// @param vm The virtual machine to initialize
void StackVMInit(StackVM* vm);

/// @brief Frees the resources used by a StackVM
/// @param vm The virtual machine to modify
void StackVMFree(StackVM* vm);

/// @brief Pushes a QWORD on the top the stack of a StackVM
/// @param vm The virtual machine to modify
/// @param value The value to push
void StackVMPush(StackVM* vm, QWORD value);

/// @brief returns the top of a StackVM
/// @param vm The virtual machine to modify
/// @return The top of the stack
QWORD StackVMTop(StackVM* vm);

/// @brief Pops and returns the top of a StackVM
/// @param vm The virtual machine to modify
/// @return The popped QWORD
QWORD StackVMPop(StackVM* vm);

/// @brief Check if a stack of a StackVM is empty
/// @param vm The virtual machine for which to check
/// @return True if the stack was empty
bool StackVMIsEmpty(const StackVM* vm);

/// @brief Returns the number of QWORD pushed on the stack of a StackVM
/// @param vm The virtual machine for which to check
/// @return The count of QWORD pushed
uint64_t StackVMSize(const StackVM* vm);

/// @brief Runs code contained in a Chunk using an initialized StackVM
/// @param vm The virtual machine in which to run
/// @param chunk The chunk containing the code to run
/// @return The result of the interpretation
InterpretResult StackVMRun(StackVM* vm, Chunk* chunk);

#endif //HG_COLTI_STACK_BASED_VM