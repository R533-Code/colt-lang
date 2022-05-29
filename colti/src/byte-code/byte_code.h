/** @file byte_code.h
* Contains the byte-code enums, and helper functions to execute the byte-codes.
* The OpCode enum represents an Operation Code, which describes an operation
* to be done by the interpreter.
* These byte-codes are usually followed by operands giving more informations about
* what to do. For example, OP_NEGATE should be followed by an OperandType,
* which describes the actual type of the QWORD to negate.
* To abstract away the OpCode, how they and their operands are stored,
* helper function of the form OpCode_{OP_CODE_NAME} are written, which expects
* the operands on which to perform the operation, and the operands describing that operation.
*/

#ifndef HG_COLTI_BYTE_CODE
#define HG_COLTI_BYTE_CODE

#include "common.h"

/// @brief Represents an instruction to be executed by the VM
typedef enum
{
	//IMMEDIATE VALUES LOADING
	/// @brief Specifies that the next byte-code is an immediate byte
	OP_IMMEDIATE_BYTE,
	/// @brief Specifies that a 2 bytes (aligned) WORD is written in the following byte-codes
	OP_IMMEDIATE_WORD,
	/// @brief Specifies that a 4 bytes (aligned) DWORD is written in the following byte-codes
	OP_IMMEDIATE_DWORD,
	/// @brief Specifies that an 8 bytes (aligned) QWORD is written in the following byte-codes
	OP_IMMEDIATE_QWORD,

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

	/// @brief Specifies that the next byte is an operand to which to cast a QWORD before printing it (for debug purposes)
	OP_PRINT,

	//MISCALLENEOUS
	OP_RETURN,
} OpCode;


/// @brief Represents an operand type
typedef enum
{
	/// @brief Represents a bool
	COLTI_BOOL		= COLTI_BOOL_ID,
	/// @brief Represents a 64-bit double-precision float
	COLTI_DOUBLE	= COLTI_DOUBLE_ID,
	/// @brief Represents a 32-bit single-precision float
	COLTI_FLOAT		= COLTI_FLOAT_ID,

	/// @brief Represents a signed 8-bit integer
	COLTI_INT8		= COLTI_I8_ID,
	/// @brief Represents a signed 16-bit integer
	COLTI_INT16		= COLTI_I16_ID,
	/// @brief Represents a signed 32-bit integer
	COLTI_INT32		= COLTI_I32_ID,
	/// @brief Represents a signed 64-bit integer
	COLTI_INT64		= COLTI_I64_ID,

	/// @brief Represents an unsigned 8-bit integer
	COLTI_UINT8		= COLTI_U8_ID,
	/// @brief Represents an unsigned 16-bit integer
	COLTI_UINT16	= COLTI_U16_ID,
	/// @brief Represents an unsigned 32-bit integer
	COLTI_UINT32	= COLTI_U32_ID,
	/// @brief Represents an unsigned 64-bit integer
	COLTI_UINT64	= COLTI_U64_ID,
} OperandType;

/**********************************
BYTE-CODE RUNNING
**********************************/

/// @brief Casts 'value' to 'type', negates the result and return it
/// @param value The QWORD on which to operate
/// @param type The type to which to cast the QWORD
/// @return The modified QWORD
QWORD OpCode_Negate(QWORD value, OperandType type);

/// @brief Casts 'value' from 'from' to 'to'
/// @param value The QWORD on which to operate
/// @param from The type of the QWORD
/// @param to The type to which to cast the QWORD
/// @return The modified QWORD
QWORD OpCode_Convert(QWORD value, OperandType from, OperandType to);

/// @brief Casts 2 QWORD and return their sums
/// @param left The left hand side
/// @param right The right hand side
/// @param type The type of the QWORDs
/// @return The sum of the QWORDs
QWORD OpCode_Sum(QWORD left, QWORD right, OperandType type);

/// @brief Casts 2 QWORD and return their difference
/// @param left The left hand side
/// @param right The right hand side
/// @param type The type of the QWORDs
/// @return The difference of the QWORDs
QWORD OpCode_Difference(QWORD left, QWORD right, OperandType type);

/// @brief Casts 2 QWORD and return their multiplication
/// @param left The left hand side
/// @param right The right hand side
/// @param type The type of the QWORDs
/// @return The multiplication of the QWORDs
QWORD OpCode_Multiply(QWORD left, QWORD right, OperandType type);

/// @brief Casts 2 QWORD and return their division
/// @param left The left hand side
/// @param right The right hand side
/// @param type The type of the QWORDs
/// @return The division of the QWORDs
QWORD OpCode_Divide(QWORD left, QWORD right, OperandType type);

/// @brief Casts 'value' to 'type' then prints its value, for DEBUG purposes
/// @param value The QWORD to print
/// @param type The type of the QWORD
void OpCode_Print(QWORD value, OperandType type);

#endif //HG_COLTI_BYTE_CODE