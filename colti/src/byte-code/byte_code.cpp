/** @file byte_code.c
* Contains the definitions of the functions declared in 'byte_code.h'
*/

#include "byte_code.h"

namespace colti::code
{
	QWORD OpCode_Negate(QWORD value, OperandType type)
	{
		QWORD result;
		switch (type)
		{
		break; case OperandType::Int8:		result.i8 = -value.i8;
		break; case OperandType::Int16:		result.i16 = -value.i16;
		break; case OperandType::Int32:		result.i32 = -value.i32;
		break; case OperandType::Int64:		result.i64 = -value.i64;
		break; case OperandType::Float:		result.f = -value.f;
		break; case OperandType::Double:	result.d = -value.d;
		break; default: colti_assert(false, "Invalid operand for OP_ADD!");
		}
		return result;
	}

	QWORD OpCode_Convert(QWORD value, OperandType from, OperandType to)
	{
		colti_assert(false, "Still not implemented!");
	}

	QWORD OpCode_Sum(QWORD left, QWORD right, OperandType type)
	{
		QWORD result;
		switch (type)
		{
		break; case OperandType::Int8:		result.i8 = left.i8 + right.i8;
		break; case OperandType::Int16:		result.i16 = left.i16 + right.i16;
		break; case OperandType::Int32:		result.i32 = left.i32 + right.i32;
		break; case OperandType::Int64:		result.i64 = left.i64 + right.i64;
		break; case OperandType::UInt8:		result.ui8 = left.ui8 + right.ui8;
		break; case OperandType::UInt16:	result.ui16 = left.ui16 + right.ui16;
		break; case OperandType::UInt32:	result.ui32 = left.ui32 + right.ui32;
		break; case OperandType::UInt64:	result.ui64 = left.ui64 + right.ui64;
		break; case OperandType::Float:		result.f = left.f + right.f;
		break; case OperandType::Double:	result.d = left.d + right.d;
		break; default: colti_assert(false, "Invalid operand for OP_ADD!");
		}
		return result;
	}

	QWORD OpCode_Difference(QWORD left, QWORD right, OperandType type)
	{
		QWORD result;
		switch (type)
		{
		break; case OperandType::Int8:		result.i8 = left.i8 - right.i8;
		break; case OperandType::Int16:		result.i16 = left.i16 - right.i16;
		break; case OperandType::Int32:		result.i32 = left.i32 - right.i32;
		break; case OperandType::Int64:		result.i64 = left.i64 - right.i64;
		break; case OperandType::UInt8:		result.ui8 = left.ui8 - right.ui8;
		break; case OperandType::UInt16:		result.ui16 = left.ui16 - right.ui16;
		break; case OperandType::UInt32:		result.ui32 = left.ui32 - right.ui32;
		break; case OperandType::UInt64:		result.ui64 = left.ui64 - right.ui64;
		break; case OperandType::Float:	result.f = left.f - right.f;
		break; case OperandType::Double:	result.d = left.d - right.d;
		break; default: colti_assert(false, "Invalid operand for OP_ADD!");
		}
		return result;
	}

	QWORD OpCode_Multiply(QWORD left, QWORD right, OperandType type)
	{
		QWORD result;
		switch (type)
		{
		break; case OperandType::Int8:		result.i8 = left.i8 * right.i8;
		break; case OperandType::Int16:		result.i16 = left.i16 * right.i16;
		break; case OperandType::Int32:		result.i32 = left.i32 * right.i32;
		break; case OperandType::Int64:		result.i64 = left.i64 * right.i64;
		break; case OperandType::UInt8:		result.ui8 = left.ui8 * right.ui8;
		break; case OperandType::UInt16:		result.ui16 = left.ui16 * right.ui16;
		break; case OperandType::UInt32:		result.ui32 = left.ui32 * right.ui32;
		break; case OperandType::UInt64:		result.ui64 = left.ui64 * right.ui64;
		break; case OperandType::Float:	result.f = left.f * right.f;
		break; case OperandType::Double:	result.d = left.d * right.d;
		break; default: colti_assert(false, "Invalid operand for OP_ADD!");
		}
		return result;
	}

	QWORD OpCode_Divide(QWORD left, QWORD right, OperandType type)
	{
		QWORD result;
		switch (type)
		{
		break; case OperandType::Int8:		result.i8 = left.i8 / right.i8;
		break; case OperandType::Int16:		result.i16 = left.i16 / right.i16;
		break; case OperandType::Int32:		result.i32 = left.i32 / right.i32;
		break; case OperandType::Int64:		result.i64 = left.i64 / right.i64;
		break; case OperandType::UInt8:		result.ui8 = left.ui8 / right.ui8;
		break; case OperandType::UInt16:		result.ui16 = left.ui16 / right.ui16;
		break; case OperandType::UInt32:		result.ui32 = left.ui32 / right.ui32;
		break; case OperandType::UInt64:		result.ui64 = left.ui64 / right.ui64;
		break; case OperandType::Float:	result.f = left.f / right.f;
		break; case OperandType::Double:	result.d = left.d / right.d;
		break; default: colti_assert(false, "Invalid operand for OP_ADD!");
		}
		return result;
	}

	void OpCode_Print(QWORD value, OperandType type)
	{
		switch (type)
		{
		break; case OperandType::Bool:		printf("%s", value.b ? "true\n" : "false\n");
		break; case OperandType::Int8:		printf("%"PRId8"\n", value.ui8);
		break; case OperandType::Int16:		printf("%"PRId16"\n", value.ui16);
		break; case OperandType::Int32:		printf("%"PRId32"\n", value.ui32);
		break; case OperandType::Int64:		printf("%"PRId64"\n", value.ui64);
		break; case OperandType::UInt8:		printf("%"PRIu8"\n", value.ui8);
		break; case OperandType::UInt16:	printf("%"PRIu16"\n", value.ui16);
		break; case OperandType::UInt32:	printf("%"PRIu32"\n", value.ui32);
		break; case OperandType::UInt64:	printf("%"PRIu64"\n", value.ui64);
		break; case OperandType::Float:		printf("%g\n", value.f);
		break; case OperandType::Double:	printf("%g\n", value.d);
		break; default: colti_assert(false, "Invalid operand for OP_PRINT!");
		}
	}
}