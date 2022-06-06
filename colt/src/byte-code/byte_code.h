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
	//IMMEDIATE VALUES LOADING
	/// @brief Specifies that the next byte-code is an immediate byte
	OP_PUSH_BYTE,
	/// @brief Specifies that a 2 bytes (aligned) WORD is written in the following bytes
	OP_PUSH_WORD,
	/// @brief Specifies that a 4 bytes (aligned) DWORD is written in the following bytes
	OP_PUSH_DWORD,
	/// @brief Specifies that an 8 bytes (aligned) QWORD is written in the following bytes
	OP_PUSH_QWORD,

	/// @brief Specifies that an 8 byte (aligned) QWORD is written in the following bytes, which gives an offset to the global BYTE to load
	OP_LOAD_BYTE,
	/// @brief Specifies that an 8 byte (aligned) QWORD is written in the following bytes, which gives an offset to the global WORD to load
	OP_LOAD_WORD,
	/// @brief Specifies that an 8 byte (aligned) QWORD is written in the following bytes, which gives an offset to the global DWORD to load
	OP_LOAD_DWORD,
	/// @brief Specifies that an 8 byte (aligned) QWORD is written in the following bytes, which gives an offset to the global QWORD to load
	OP_LOAD_QWORD,

	/// @brief Specifies that an 8 byte (aligned) QWORD is written in the following bytes, which gives an offset to the global BYTE to write to
	OP_STORE_BYTE,
	/// @brief Specifies that an 8 byte (aligned) QWORD is written in the following bytes, which gives an offset to the global WORD to write to
	OP_STORE_WORD,
	/// @brief Specifies that an 8 byte (aligned) QWORD is written in the following bytes, which gives an offset to the global DWORD to write to
	OP_STORE_DWORD,
	/// @brief Specifies that an 8 byte (aligned) QWORD is written in the following bytes, which gives an offset to the global QWORD to write to
	OP_STORE_QWORD,

	/// @brief Specifies that the next byte is an operand to which to cast a QWORD before negating its sign
	OP_NEGATE,

	/// @brief Specifies that the next 2 bytes are operands type, the first to which to cast a QWORD before converting it to the second
	OP_CONVERT,

	/// @brief Specifies that the next byte is an operand to which to cast 2 QWORD before doing their sum
	OP_ADD,
	/// @brief Specifies that the next byte is an operand to which to cast 2 QWORD before doing their difference
	OP_SUBTRACT,
	/// @brief Specifies that the next byte is an operand to which to cast 2 QWORD before doing their product
	OP_MULTIPLY,
	/// @brief Specifies that the next byte is an operand to which to cast 2 QWORD before doing their division
	OP_DIVIDE,

	//DON'T KNOW
	OP_MODULO,

	/// @brief Specifies that the next byte is an operand to which to cast 2 QWORD before doing their bitwise AND
	OP_BIT_AND,
	/// @brief Specifies that the next byte is an operand to which to cast 2 QWORD before doing their bitwise OR
	OP_BIT_OR,
	/// @brief Specifies that the next byte is an operand to which to cast 2 QWORD before doing their bitwise XOR
	OP_BIT_XOR,
	/// @brief Specifies that the next byte is an operand to which to cast a QWORD before doing its bitwise NOT
	OP_BIT_NOT,
	/// @brief Specifies that the next byte is an operand to which to cast 2 QWORD before doing their shift left
	OP_BIT_SHIFT_L,
	/// @brief Specifies that the next byte is an operand to which to cast 2 QWORD before doing their shift right
	OP_BIT_SHIFT_R,

	/// @brief Specifies that the next byte is an operand to which to cast 2 QWORD before comparing greater
	OP_CMP_GREATER,
	/// @brief Specifies that the next byte is an operand to which to cast 2 QWORD before comparing less
	OP_CMP_LESS,
	/// @brief Specifies that the next byte is an operand to which to cast 2 QWORD before comparing greater equal
	OP_CMP_GREATER_EQ,
	/// @brief Specifies that the next byte is an operand to which to cast 2 QWORD before comparing less equal
	OP_CMP_LESS_EQ,
	/// @brief Specifies that the next byte is an operand to which to cast 2 QWORD before comparing equal
	OP_CMP_EQUAL,
	/// @brief Specifies that the next byte is an operand to which to cast 2 QWORD before comparing not equal
	OP_CMP_NOT_EQUAL,

	/// @brief Short Jump if Greater, two operands, first type to which to cast to compare, second offset to jump to if true
	OP_SJUMP_GREATER,
	/// @brief Short Jump if Less, two operands, first type to which to cast to compare, second offset to jump to if true
	OP_SJUMP_LESS,
	/// @brief Short Jump if Greater or Equal, two operands, first type to which to cast to compare, second offset to jump to if true
	OP_SJUMP_GREATER_EQ,
	/// @brief Short Jump if Less or Equal, two operands, first type to which to cast to compare, second offset to jump to if true
	OP_SJUMP_LESS_EQ,
	/// @brief Short Jump if Equal, two operands, first type to which to cast to compare, second offset to jump to if true
	OP_SJUMP_EQUAL,
	/// @brief Short Jump if Not Equal, two operands, first type to which to cast to compare, second offset to jump to if true
	OP_SJUMP_NOT_EQUAL,

	/// @brief Specifies that the next byte is an operand to which to cast a QWORD before printing it (for debug purposes)
	OP_PRINT,

	//MISCALLENEOUS

	/// @brief Pops the last immediate value
	OP_POP,
	/// @brief Returns execution from a function
	OP_RETURN,
	/// @brief Stops interpreting, followed by an u64, which is the exit code
	OP_EXIT,
} OpCode;


/// @brief Represents an operand type
typedef enum
{
	/// @brief Represents a bool
	ID_COLT_BOOL		= COLTI_BOOL_ID,
	/// @brief Represents a 64-bit double-precision float
	ID_COLT_DOUBLE		= COLTI_DOUBLE_ID,
	/// @brief Represents a 32-bit single-precision float
	ID_COLT_FLOAT		= COLTI_FLOAT_ID,

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