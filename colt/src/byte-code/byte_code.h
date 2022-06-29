/** @file byte_code.h
* Contains the byte-code enums, and helper functions to execute the byte-codes.
* The OpCode enum represents an Operation Code, which describes an operation
* to be done by the interpreter.
* These byte-codes are usually followed by operands giving more informations about
* what to do. For example, OP_NEGATE should be followed by an BuiltinTypeID,
* which describes the actual type of the QWORD to negate.
* To abstract away the OpCode, how they and their operands are stored,
* helper function of the form OpCode_{OP_CODE_NAME} are written, which expects
* the operands on which to perform the operation, and the operands describing that operation.
*/

#ifndef HG_COLT_BYTE_CODE
#define HG_COLT_BYTE_CODE

#include "common.h"

/// @brief Represents an instruction to be executed by the VM
typedef enum
{
	/// @brief Pushes an immediate BYTE to the top of the stack.
	/// [OP_PUSH_BYTE][BYTE]
	OP_PUSH_BYTE,
	/// @brief Pushes an immediate WORD to the top of the stack.
	/// [OP_PUSH_WORD][PADDING]?[WORD]
	OP_PUSH_WORD,
	/// @brief Pushes an immediate DWORD to the top of the stack.
	/// [OP_PUSH_DWORD][PADDING]?[DWORD]
	OP_PUSH_DWORD,
	/// @brief Pushes an immediate QWORD to the top of the stack.
	/// [OP_PUSH_QWORD][PADDING]?[QWORD]
	OP_PUSH_QWORD,

	/// @brief Pushes a QWORD from the global section to the top of the stack.
	/// [OP_LOAD_GLOBAL][PADDING]?[QWORD]
	/// The QWORD is the byte offset from the beginning of the Chunk to the QWORD
	OP_LOAD_GLOBAL,
	/// @brief Writes the top QWORD of the stack to the global section.
	/// [OP_STORE_GLOBAL][PADDING]?[QWORD]
	/// The QWORD is the byte offset from the beginning of the Chunk to the QWORD
	OP_STORE_GLOBAL,

	/// @brief Pushes the current scope, then allocates and pushes a new scope for local variables.
	/// [OP_PUSH_SCOPE][PADDING]?[LOCAL_COUNT]
	/// This allocates LOCAL_COUNT QWORDs in the stack, which are used as local variables.
	/// The LOCAL_COUNT is an aligned DWORD.
	OP_PUSH_SCOPE,
	/// @brief Deallocates the top scope, and pops the current scope.
	/// [OP_PUSH_SCOPE][PADDING]?[LOCAL_COUNT]
	/// This deallocates LOCAL_COUNT QWORDs in the stack, which are used as local variables.
	/// The LOCAL_COUNT is an aligned DWORD.
	OP_POP_SCOPE,

	/// @brief Short Load Local, loads a local from the current scope and pushes it to the stack.
	/// [OP_SLOAD_LOCAL][LOCAL_NUM]
	/// The LOCAL_NUM is a byte, which represents the QWORD offset to add to the current scope to get the variable to which to fetch.
	OP_SLOAD_LOCAL,
	/// @brief Short Store Local, writes the top of the stack to a local from the current scope.
	/// [OP_SSTORE_LOCAL][LOCAL_NUM]
	/// The LOCAL_NUM is a byte, which represents the QWORD offset to add to the current scope to get the variable to which to write.
	OP_SSTORE_LOCAL,
	/// @brief Loads a local from the current scope and pushes it to the stack.
	/// [OP_LOAD_LOCAL][PADDING]?[LOCAL_NUM]
	/// The LOCAL_NUM is a DWORD, which represents the QWORD offset to add to the current scope to get the variable to which to fetch.
	OP_LOAD_LOCAL,
	/// @brief Writes the top of the stack to a local from the current scope.
	/// [OP_STORE_LOCAL][PADDING]?[LOCAL_NUM]
	/// The LOCAL_NUM is a DWORD, which represents the QWORD offset to add to the current scope to get the variable to which to write.
	OP_STORE_LOCAL,

	/// @brief Pops the top of the stack and uses it as a byte offset to an lstring, then pushes the pointer to the beginning of the lstring.
	/// [OP_LOAD_LSTRING]
	OP_LOAD_LSTRING,
	/// @brief Pops the top of the stack and turns it back to a byte offset which is then pushed to the top of the stack
	/// [OP_STORE_LSTRING]
	OP_STORE_LSTRING,

	/// @brief Negates the top of the stack.
	/// [OP_NEGATE][TYPE]
	/// The type is a BuiltinTypeID (neither bool nor lstring) to which to cast the QWORD before negating it.
	OP_NEGATE,

	/// @brief Converts the top of the stack from a built-in type to another.
	/// [OP_CONVERT][FROM][TO]
	/// FROM and TO are BuilinTypeIDs (not lstring) to which to cast the QWORD from then to respectively.
	OP_CONVERT,

	/// @brief Pops two QWORD from the stack and pushes their sum.
	/// [OP_ADD][TYPE]
	/// The type is a BuiltinTypeID (neither bool nor lstring) to which to cast the QWORDs.
	/// The first popped QWORD is the right hand side of the operator.
	OP_ADD,
	/// @brief Pops two QWORD from the stack and pushes their subtraction.
	/// [OP_SUBTRACT][TYPE]
	/// The type is a BuiltinTypeID (neither bool nor lstring) to which to cast the QWORDs.
	/// The first popped QWORD is the right hand side of the operator.
	OP_SUBTRACT,
	/// @brief Pops two QWORD from the stack and pushes their multiplication.
	/// [OP_MULTIPLY][TYPE]
	/// The type is a BuiltinTypeID (neither bool nor lstring) to which to cast the QWORDs.
	/// The first popped QWORD is the right hand side of the operator.
	OP_MULTIPLY,
	/// @brief Pops two QWORD from the stack and pushes their division.
	/// [OP_DIVIDE][TYPE]
	/// The type is a BuiltinTypeID (neither bool nor lstring) to which to cast the QWORDs.
	/// The first popped QWORD is the right hand side of the operator.
	OP_DIVIDE,

	/// @brief Pops two QWORD from the stack and pushes the remainder of their division.
	/// [OP_MODULO][TYPE]
	/// The type is an integral BuiltinTypeID to which to cast the QWORDs.
	/// The first popped QWORD is the right hand side of the operator.
	OP_MODULO,

	/// @brief Pops two QWORD from the stack and pushes their bitwise AND.
	/// [OP_BIT_AND][TYPE]
	/// The type is an integral BuiltinTypeID to which to cast the QWORDs.
	OP_BIT_AND,
	/// @brief Pops two QWORD from the stack and pushes their bitwise OR.
	/// [OP_BIT_OR][TYPE]
	/// The type is an integral BuiltinTypeID to which to cast the QWORDs.
	OP_BIT_OR,
	/// @brief Pops two QWORD from the stack and pushes their bitwise XOR.
	/// [OP_BIT_XOR][TYPE]
	/// The type is an integral BuiltinTypeID to which to cast the QWORDs.
	OP_BIT_XOR,
	/// @brief Bitwise NOT the top of the stack.
	/// [OP_BIT_NOT][TYPE]
	/// The type is an integral BuiltinTypeID to which to cast the QWORD.
	OP_BIT_NOT,

	/// @brief Pops two QWORD from the stack and pushes their bit shift left.
	/// [OP_BIT_SHIFT_L][TYPE]
	/// The type is an integral BuiltinTypeID to which to cast the QWORDs.
	/// The first popped QWORD is the right hand side of the operator.
	OP_BIT_SHIFT_L,
	/// @brief Pops two QWORD from the stack and pushes their bit shift right.
	/// [OP_BIT_SHIFT_R][TYPE]
	/// The type is an integral BuiltinTypeID to which to cast the QWORDs.
	/// The first popped QWORD is the right hand side of the operator.
	OP_BIT_SHIFT_R,

	/// @brief Boolean not the top of the stack.
	/// [OP_BOOL_NOT][TYPE]
	/// The type is a BuiltinTypeID (not lstring) to which to cast the QWORD.
	OP_BOOL_NOT,

	/// @brief Pops 2 QWORDs, compares them and pushes a boolean result.
	/// [OP_CMP_GREATER][TYPE]
	/// The type is a BuiltinTypeID (neither bool nor lstring) to which to cast the QWORDs.
	OP_CMP_GREATER,
	/// @brief Pops 2 QWORDs, compares them and pushes a boolean result.
	/// [OP_CMP_LESS][TYPE]
	/// The type is a BuiltinTypeID (neither bool nor lstring) to which to cast the QWORDs.
	OP_CMP_LESS,
	/// @brief Pops 2 QWORDs, compares them and pushes a boolean result.
	/// [OP_CMP_GREATER_EQ][TYPE]
	/// The type is a BuiltinTypeID (neither bool nor lstring) to which to cast the QWORDs.
	OP_CMP_GREATER_EQ,
	/// @brief Pops 2 QWORDs, compares them and pushes a boolean result.
	/// [OP_CMP_LESS_EQ][TYPE]
	/// The type is a BuiltinTypeID (neither bool nor lstring) to which to cast the QWORDs.
	OP_CMP_LESS_EQ,
	/// @brief Pops 2 QWORDs, compares them and pushes a boolean result.
	/// [OP_CMP_EQUAL][TYPE]
	/// The type is a BuiltinTypeID (not lstring) to which to cast the QWORDs.
	OP_CMP_EQUAL,
	/// @brief Pops 2 QWORDs, compares them and pushes a boolean result.
	/// [OP_CMP_NOT_EQUAL][TYPE]
	/// The type is a BuiltinTypeID (not lstring) to which to cast the QWORDs.
	OP_CMP_NOT_EQUAL,

	/// @brief Short Jump if Greater, compares the top 2 QWORDs, if the result is true, performs a jump
	/// [OP_SJUMP_GREATER][TYPE][JUMP_OFFSET]
	/// The type is a BuiltinTypeID (neither bool nor lstring) to cast the QWORDs.
	/// The JUMP_OFFSET is an unsigned byte which is added to the instruction pointer of the VM.
	OP_SJUMP_GREATER,
	/// @brief Short Jump if Less, compares the top 2 QWORDs, if the result is true, performs a jump
	/// [OP_SJUMP_LESS][TYPE][JUMP_OFFSET]
	/// The type is a BuiltinTypeID (neither bool nor lstring) to cast the QWORDs.
	/// The JUMP_OFFSET is an unsigned byte which is added to the instruction pointer of the VM.
	OP_SJUMP_LESS,
	/// @brief Short Jump if Greater or Equal, compares the top 2 QWORDs, if the result is true, performs a jump
	/// [OP_SJUMP_GREATER_EQ][TYPE][JUMP_OFFSET]
	/// The type is a BuiltinTypeID (neither bool nor lstring) to cast the QWORDs.
	/// The JUMP_OFFSET is an unsigned byte which is added to the instruction pointer of the VM.
	OP_SJUMP_GREATER_EQ,
	/// @brief Short Jump if Greater or Equal, compares the top 2 QWORDs, if the result is true, performs a jump
	/// [OP_SJUMP_LESS_EQ][TYPE][JUMP_OFFSET]
	/// The type is a BuiltinTypeID (neither bool nor lstring) to cast the QWORDs.
	/// The JUMP_OFFSET is an unsigned byte which is added to the instruction pointer of the VM.
	OP_SJUMP_LESS_EQ,
	/// @brief Short Jump if Equal, compares the top 2 QWORDs, if the result is true, performs a jump
	/// [OP_SJUMP_EQUAL][TYPE][JUMP_OFFSET]
	/// The type is a BuiltinTypeID (not lstring) to cast the QWORDs.
	/// The JUMP_OFFSET is an unsigned byte which is added to the instruction pointer of the VM.
	OP_SJUMP_EQUAL,
	/// @brief Short Jump if Not Equal, compares the top 2 QWORDs, if the result is true, performs a jump
	/// [OP_SJUMP_NOT_EQUAL][TYPE][JUMP_OFFSET]
	/// The type is a BuiltinTypeID (not lstring) to cast the QWORDs.
	/// The JUMP_OFFSET is an unsigned byte which is added to the instruction pointer of the VM.
	OP_SJUMP_NOT_EQUAL,

	/// @brief Short Jump if True, if the top QWORD.b == 1, perform a jump
	/// [OP_SJUMP_TRUE][JUMP_OFFSET]
	/// The JUMP_OFFSET is an unsigned byte which is added to the instruction pointer of the VM.
	OP_SJUMP_TRUE,
	/// @brief Short Jump if Not True, if the top QWORD.b == 0, if it evaluates to a true, perform a jump
	/// [OP_SJUMP_NOT_TRUE][JUMP_OFFSET]
	/// The JUMP_OFFSET is an unsigned byte which is added to the instruction pointer of the VM.
	OP_SJUMP_NOT_TRUE,	

	/// @brief Prints the top value of the VM.
	/// [OP_PRINT][TYPE]
	/// The type is a BuiltinTypeID to cast the QWORD to.
	OP_PRINT,

	//MISCALLENEOUS

	/// @brief Pops the top QWORD of the stack.
	/// [OP_POP]
	OP_POP,
	/// @brief Returns execution from a function.
	/// [OP_RETURN]
	OP_RETURN,
	/// @brief Stops interpretation with an error code.
	/// [OP_EXIT][PADDING]?[I64]
	OP_EXIT,
} OpCode;


/// @brief Represents an operand type
typedef enum
{
	/// @brief Represents the absence of type
	ID_COLT_VOID		= COLTI_VOID_ID,
	/// @brief Represents a bool
	ID_COLT_BOOL		= COLTI_BOOL_ID,
	/// @brief Represents a 64-bit double-precision float
	ID_COLT_DOUBLE		= COLTI_DOUBLE_ID,
	/// @brief Represents a 32-bit single-precision float
	ID_COLT_FLOAT		= COLTI_FLOAT_ID,

	/// @brief Represents a pointer to a literal string
	ID_COLT_LSTRING = COLTI_LSTRING_ID,

	/// @brief Represents a signed 8-bit integer
	ID_COLT_I8			= COLTI_I8_ID,
	/// @brief Represents a signed 16-bit integer
	ID_COLT_I16			= COLTI_I16_ID,
	/// @brief Represents a signed 32-bit integer
	ID_COLT_I32			= COLTI_I32_ID,
	/// @brief Represents a signed 64-bit integer
	ID_COLT_I64			= COLTI_I64_ID,

	/// @brief Represents an unsigned 8-bit integer
	ID_COLT_U8			= COLTI_U8_ID,
	/// @brief Represents an unsigned 16-bit integer
	ID_COLT_U16			= COLTI_U16_ID,
	/// @brief Represents an unsigned 32-bit integer
	ID_COLT_U32			= COLTI_U32_ID,
	/// @brief Represents an unsigned 64-bit integer
	ID_COLT_U64			= COLTI_U64_ID,	
} BuiltinTypeID;

/// @brief Converts a built-in type ID to a const char*
/// @param id The ID to convert
/// @return const char* representing the ID or UNKNOWN
const char* BuiltinTypeIDToString(BuiltinTypeID id);

/**********************************
BYTE-CODE RUNNING
**********************************/

/// @brief Casts 'value' to 'type', negates the result and return it
/// @param value The QWORD on which to operate
/// @param type The type to which to cast the QWORD
/// @return The modified QWORD
QWORD OpCode_Negate(QWORD value, BuiltinTypeID type);

/// @brief Casts 'value' from 'from' to 'to'
/// @param value The QWORD on which to operate
/// @param from The type of the QWORD
/// @param to The type to which to cast the QWORD
/// @return The modified QWORD
QWORD OpCode_Convert(QWORD value, BuiltinTypeID from, BuiltinTypeID to);

/// @brief Casts 2 QWORD and return their sums
/// @param left The left hand side
/// @param right The right hand side
/// @param type The type of the QWORDs
/// @return The sum of the QWORDs
QWORD OpCode_Sum(QWORD left, QWORD right, BuiltinTypeID type);

/// @brief Casts 2 QWORD and return their difference
/// @param left The left hand side
/// @param right The right hand side
/// @param type The type of the QWORDs
/// @return The difference of the QWORDs
QWORD OpCode_Difference(QWORD left, QWORD right, BuiltinTypeID type);

/// @brief Casts 2 QWORD and return their multiplication
/// @param left The left hand side
/// @param right The right hand side
/// @param type The type of the QWORDs
/// @return The multiplication of the QWORDs
QWORD OpCode_Multiply(QWORD left, QWORD right, BuiltinTypeID type);

/// @brief Casts 2 QWORD and return their division
/// @param left The left hand side
/// @param right The right hand side
/// @param type The type of the QWORDs
/// @return The division of the QWORDs
QWORD OpCode_Divide(QWORD left, QWORD right, BuiltinTypeID type);

/// @brief Casts 2 QWORD and return the remainder of their division
/// @param left The left hand side
/// @param right The right hand side
/// @param type The type of the QWORDs
/// @return The remainder of their division
QWORD OpCode_Modulo(QWORD left, QWORD right, BuiltinTypeID type);

/// @brief Casts 2 QWORD and return their bitwise AND
/// @param left The left hand side
/// @param right The right hand side
/// @param type The type of the QWORDs
/// @return The bitwise AND of the QWORDs
QWORD OpCode_BitAND(QWORD left, QWORD right, BuiltinTypeID type);

/// @brief Casts 2 QWORD and return their bitwise OR
/// @param left The left hand side
/// @param right The right hand side
/// @param type The type of the QWORDs
/// @return The bitwise OR of the QWORDs
QWORD OpCode_BitOR(QWORD left, QWORD right, BuiltinTypeID type);

/// @brief Casts 2 QWORD and return their bitwise XOR
/// @param left The left hand side
/// @param right The right hand side
/// @param type The type of the QWORDs
/// @return The bitwise XOR of the QWORDs
QWORD OpCode_BitXOR(QWORD left, QWORD right, BuiltinTypeID type);

/// @brief Casts 1 QWORD and return its bitwise NOT
/// @param value The QWORD to bitwise NOT
/// @param type The type of the QWORD
/// @return The bitwise NOT of the QWORD
QWORD OpCode_BitNOT(QWORD value, BuiltinTypeID type);

/// @brief Casts 'left' and 'right' to 'type' and perform a shift left
/// @param left The QWORD to shift
/// @param right The QWORD to shift by
/// @param type The type of the QWORD
/// @return The shifted QWORD
QWORD OpCode_BitShiftL(QWORD left, QWORD right, BuiltinTypeID type);

/// @brief Casts 'left' and 'right' to 'type' and perform a shift right
/// @param left The QWORD to shift
/// @param right The QWORD to shift by
/// @param type The type of the QWORD
/// @return The shifted QWORD
QWORD OpCode_BitShiftR(QWORD left, QWORD right, BuiltinTypeID type);

/// @brief Casts 'value' to 'type' and performs a bool conversion to it before doing its not '!'
/// @param value The value to bool not
/// @param type The type of the QWORD
/// @return The bool not-ed QWORD
QWORD OpCode_BoolNot(QWORD value, BuiltinTypeID type);

/// @brief Casts 2 QWORD and return a bool from 'greater' comparison
/// @param left The left hand side of the operator
/// @param right The right hand side of the operator
/// @param type The type to which to cast both QWORD
/// @return Bool representing the 2 QWORD comparison
QWORD OpCode_Greater(QWORD left, QWORD right, BuiltinTypeID type);

/// @brief Casts 2 QWORD and return a bool from 'less' comparison
/// @param left The left hand side of the operator
/// @param right The right hand side of the operator
/// @param type The type to which to cast both QWORD
/// @return Bool representing the 2 QWORD comparison
QWORD OpCode_Less(QWORD left, QWORD right, BuiltinTypeID type);

/// @brief Casts 2 QWORD and return a bool from 'greater or equal' comparison
/// @param left The left hand side of the operator
/// @param right The right hand side of the operator
/// @param type The type to which to cast both QWORD
/// @return Bool representing the 2 QWORD comparison
QWORD OpCode_GreaterEq(QWORD left, QWORD right, BuiltinTypeID type);

/// @brief Casts 2 QWORD and return a bool from 'less or equal' comparison
/// @param left The left hand side of the operator
/// @param right The right hand side of the operator
/// @param type The type to which to cast both QWORD
/// @return Bool representing the 2 QWORD comparison
QWORD OpCode_LessEq(QWORD left, QWORD right, BuiltinTypeID type);

/// @brief Casts 2 QWORD and return a bool from 'equal' comparison
/// @param left The left hand side of the operator
/// @param right The right hand side of the operator
/// @param type The type to which to cast both QWORD
/// @return Bool representing the 2 QWORD comparison
QWORD OpCode_Equal(QWORD left, QWORD right, BuiltinTypeID type);

/// @brief Casts 2 QWORD and return a bool from 'not equal' comparison
/// @param left The left hand side of the operator
/// @param right The right hand side of the operator
/// @param type The type to which to cast both QWORD
/// @return Bool representing the 2 QWORD comparison
QWORD OpCode_NotEqual(QWORD left, QWORD right, BuiltinTypeID type);

/// @brief Casts 'value' to 'type' then prints its value, for DEBUG purposes
/// @param value The QWORD to print
/// @param type The type of the QWORD
void OpCode_Print(QWORD value, BuiltinTypeID type);

#endif //HG_COLT_BYTE_CODE