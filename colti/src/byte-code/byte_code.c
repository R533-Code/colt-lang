/** @file byte_code.c
* Contains the definitions of the functions declared in 'byte_code.h'
*/

#include "byte_code.h"

QWORD OpCode_Negate(QWORD value, BuiltinTypeID type)
{
	QWORD result;
	switch (type)
	{
	break; case COLTI_I8_ID:		result.i8 = -value.i8;
	break; case COLTI_I16_ID:		result.i16 = -value.i16;
	break; case COLTI_I32_ID:		result.i32 = -value.i32;
	break; case COLTI_I64_ID:		result.i64 = -value.i64;
	break; case COLTI_FLOAT_ID:	result.f = -value.f;
	break; case COLTI_DOUBLE_ID:	result.d = -value.d;
	break; default: colti_assert(false, "Invalid operand for OP_NEGATE!");
	}
	return result;
}

QWORD OpCode_Convert(QWORD value, BuiltinTypeID from, BuiltinTypeID to)
{
	//zeroes the memory
	QWORD ret = { .ui64 = 0 };

	switch (to)
	{
	break; case ID_COLT_BOOL:
		switch (from)
		{
		case COLTI_I8_ID:
		case COLTI_I16_ID:
		case COLTI_I32_ID:
		case COLTI_I64_ID:
		case COLTI_U8_ID:
		case COLTI_U16_ID:
		case COLTI_U32_ID:
		case COLTI_U64_ID:
			ret.b = (value.ui64 != 0);
		break; case COLTI_FLOAT_ID:
			ret.b = (value.f != 0);
		break; case COLTI_DOUBLE_ID:
			ret.b = (value.d != 0);
		break; default:
			colti_assert(false, "Invalid operand for OP_CONVERT!");
		}
	break; case ID_COLT_FLOAT:
		switch (from)
		{
		case COLTI_I8_ID:
		case COLTI_I16_ID:
		case COLTI_I32_ID:
		case COLTI_I64_ID:
		case COLTI_U8_ID:
		case COLTI_U16_ID:
		case COLTI_U32_ID:
		case COLTI_U64_ID:
			ret.f = (float)value.ui64;
		break; case COLTI_BOOL_ID:
			ret.f = (value.b == 0 ? 0.0f : 1.0f);
		break; case COLTI_DOUBLE_ID:
			ret.f = (float)value.d;
		break; default:
			colti_assert(false, "Invalid operand for OP_CONVERT!");
		}
	break; case ID_COLT_DOUBLE:
		switch (from)
		{
		case COLTI_I8_ID:
		case COLTI_I16_ID:
		case COLTI_I32_ID:
		case COLTI_I64_ID:
		case COLTI_U8_ID:
		case COLTI_U16_ID:
		case COLTI_U32_ID:
		case COLTI_U64_ID:
			ret.d = (double)value.ui64;
		break; case COLTI_BOOL_ID:
			ret.d = (value.b == 0 ? 0.0 : 1.0);
		break; case COLTI_FLOAT_ID:
			ret.d = (double)value.f;
		break; default:
			colti_assert(false, "Invalid operand for OP_CONVERT!");
		}
	break;
	case ID_COLT_I8:
	case ID_COLT_I16:
	case ID_COLT_I32:
	case ID_COLT_I64:
		switch (from)
		{
		case COLTI_U8_ID:
		case COLTI_U16_ID:
		case COLTI_U32_ID:
		case COLTI_U64_ID:
			ret.i64 = (int64_t)value.ui64;
		break; case COLTI_BOOL_ID:
			ret.i64 = (value.b == 0 ? 0 : 1);
		break; case COLTI_FLOAT_ID:
			ret.i64 = (int64_t)value.f;
		break; case COLTI_DOUBLE_ID:
			ret.i64 = (int64_t)value.d;
		break; default:
			colti_assert(false, "Invalid operand for OP_CONVERT!");
		}
	break;
	case ID_COLT_U8:
	case ID_COLT_U16:
	case ID_COLT_U32:
	case ID_COLT_U64:
		switch (from)
		{
		case COLTI_I8_ID:
		case COLTI_I16_ID:
		case COLTI_I32_ID:
		case COLTI_I64_ID:
			ret.ui64 = (uint64_t)value.i64;
		break; case COLTI_BOOL_ID:
			ret.ui64 = (value.b == 0 ? 0 : 1);
		break; case COLTI_FLOAT_ID:
			ret.ui64 = (uint64_t)value.f;
		break; case COLTI_DOUBLE_ID:
			ret.ui64 = (uint64_t)value.d;
		break; default:
			colti_assert(false, "Invalid operand for OP_CONVERT!");
		}
	break; default:
		colti_assert(false, "Invalid operand to convert to for OP_CONVERT!");
	}
	
	return ret;
}

QWORD OpCode_Sum(QWORD left, QWORD right, BuiltinTypeID type)
{
	QWORD result;
	switch (type)
	{
	break; case COLTI_I8_ID:		result.i8 = left.i8 + right.i8;
	break; case COLTI_I16_ID:		result.i16 = left.i16 + right.i16;
	break; case COLTI_I32_ID:		result.i32 = left.i32 + right.i32;
	break; case COLTI_I64_ID:		result.i64 = left.i64 + right.i64;
	break; case COLTI_U8_ID:		result.ui8 = left.ui8 + right.ui8;
	break; case COLTI_U16_ID:		result.ui16 = left.ui16 + right.ui16;
	break; case COLTI_U32_ID:		result.ui32 = left.ui32 + right.ui32;
	break; case COLTI_U64_ID:		result.ui64 = left.ui64 + right.ui64;
	break; case COLTI_FLOAT_ID:	result.f = left.f + right.f;
	break; case COLTI_DOUBLE_ID:	result.d = left.d + right.d;
	break; default: colti_assert(false, "Invalid operand for OP_ADD!");
	}
	return result;
}

QWORD OpCode_Difference(QWORD left, QWORD right, BuiltinTypeID type)
{
	QWORD result;
	switch (type)
	{
	break; case COLTI_I8_ID:		result.i8 = left.i8 - right.i8;
	break; case COLTI_I16_ID:		result.i16 = left.i16 - right.i16;
	break; case COLTI_I32_ID:		result.i32 = left.i32 - right.i32;
	break; case COLTI_I64_ID:		result.i64 = left.i64 - right.i64;
	break; case COLTI_U8_ID:		result.ui8 = left.ui8 - right.ui8;
	break; case COLTI_U16_ID:		result.ui16 = left.ui16 - right.ui16;
	break; case COLTI_U32_ID:		result.ui32 = left.ui32 - right.ui32;
	break; case COLTI_U64_ID:		result.ui64 = left.ui64 - right.ui64;
	break; case COLTI_FLOAT_ID:	result.f = left.f - right.f;
	break; case COLTI_DOUBLE_ID:	result.d = left.d - right.d;
	break; default: colti_assert(false, "Invalid operand for OP_SUBTRACT!");
	}
	return result;
}

QWORD OpCode_Multiply(QWORD left, QWORD right, BuiltinTypeID type)
{
	QWORD result;
	switch (type)
	{
	break; case COLTI_I8_ID:		result.i8 = left.i8 * right.i8;
	break; case COLTI_I16_ID:		result.i16 = left.i16 * right.i16;
	break; case COLTI_I32_ID:		result.i32 = left.i32 * right.i32;
	break; case COLTI_I64_ID:		result.i64 = left.i64 * right.i64;
	break; case COLTI_U8_ID:		result.ui8 = left.ui8 * right.ui8;
	break; case COLTI_U16_ID:		result.ui16 = left.ui16 * right.ui16;
	break; case COLTI_U32_ID:		result.ui32 = left.ui32 * right.ui32;
	break; case COLTI_U64_ID:		result.ui64 = left.ui64 * right.ui64;
	break; case COLTI_FLOAT_ID:	result.f = left.f * right.f;
	break; case COLTI_DOUBLE_ID:	result.d = left.d * right.d;
	break; default: colti_assert(false, "Invalid operand for OP_MULTIPLY!");
	}
	return result;
}

QWORD OpCode_Divide(QWORD left, QWORD right, BuiltinTypeID type)
{
	QWORD result;
	switch (type)
	{
	break; case COLTI_I8_ID:		result.i8 = left.i8 / right.i8;
	break; case COLTI_I16_ID:		result.i16 = left.i16 / right.i16;
	break; case COLTI_I32_ID:		result.i32 = left.i32 / right.i32;
	break; case COLTI_I64_ID:		result.i64 = left.i64 / right.i64;
	break; case COLTI_U8_ID:		result.ui8 = left.ui8 / right.ui8;
	break; case COLTI_U16_ID:		result.ui16 = left.ui16 / right.ui16;
	break; case COLTI_U32_ID:		result.ui32 = left.ui32 / right.ui32;
	break; case COLTI_U64_ID:		result.ui64 = left.ui64 / right.ui64;
	break; case COLTI_FLOAT_ID:	result.f = left.f / right.f;
	break; case COLTI_DOUBLE_ID:	result.d = left.d / right.d;
	break; default: colti_assert(false, "Invalid operand for OP_DIVIDE!");
	}
	return result;
}

QWORD OpCode_BitAND(QWORD left, QWORD right, BuiltinTypeID type)
{
	QWORD result;
	switch (type)
	{
	break; case COLTI_I8_ID:		result.i8 = left.i8 & right.i8;
	break; case COLTI_I16_ID:		result.i16 = left.i16 & right.i16;
	break; case COLTI_I32_ID:		result.i32 = left.i32 & right.i32;
	break; case COLTI_I64_ID:		result.i64 = left.i64 & right.i64;
	break; case COLTI_U8_ID:		result.ui8 = left.ui8 & right.ui8;
	break; case COLTI_U16_ID:		result.ui16 = left.ui16 & right.ui16;
	break; case COLTI_U32_ID:		result.ui32 = left.ui32 & right.ui32;
	break; case COLTI_U64_ID:		result.ui64 = left.ui64 & right.ui64;
	break; default: colti_assert(false, "Invalid operand for OP_BIT_AND!");
	}
	return result;
}

QWORD OpCode_BitOR(QWORD left, QWORD right, BuiltinTypeID type)
{
	QWORD result;
	switch (type)
	{
	break; case COLTI_I8_ID:		result.i8 = left.i8 | right.i8;
	break; case COLTI_I16_ID:		result.i16 = left.i16 | right.i16;
	break; case COLTI_I32_ID:		result.i32 = left.i32 | right.i32;
	break; case COLTI_I64_ID:		result.i64 = left.i64 | right.i64;
	break; case COLTI_U8_ID:		result.ui8 = left.ui8 | right.ui8;
	break; case COLTI_U16_ID:		result.ui16 = left.ui16 | right.ui16;
	break; case COLTI_U32_ID:		result.ui32 = left.ui32 | right.ui32;
	break; case COLTI_U64_ID:		result.ui64 = left.ui64 | right.ui64;
	break; default: colti_assert(false, "Invalid operand for OP_BIT_OR!");
	}
	return result;
}

QWORD OpCode_BitXOR(QWORD left, QWORD right, BuiltinTypeID type)
{
	QWORD result;
	switch (type)
	{
	break; case COLTI_I8_ID:		result.i8 = left.i8 ^ right.i8;
	break; case COLTI_I16_ID:		result.i16 = left.i16 ^ right.i16;
	break; case COLTI_I32_ID:		result.i32 = left.i32 ^ right.i32;
	break; case COLTI_I64_ID:		result.i64 = left.i64 ^ right.i64;
	break; case COLTI_U8_ID:		result.ui8 = left.ui8 ^ right.ui8;
	break; case COLTI_U16_ID:		result.ui16 = left.ui16 ^ right.ui16;
	break; case COLTI_U32_ID:		result.ui32 = left.ui32 ^ right.ui32;
	break; case COLTI_U64_ID:		result.ui64 = left.ui64 ^ right.ui64;
	break; default: colti_assert(false, "Invalid operand for OP_BIT_XOR!");
	}
	return result;
}

QWORD OpCode_BitNOT(QWORD value, BuiltinTypeID type)
{
	QWORD result;
	switch (type)
	{
	break; case COLTI_I8_ID:		result.i8 = ~value.i8;
	break; case COLTI_I16_ID:		result.i16 = ~value.i16;
	break; case COLTI_I32_ID:		result.i32 = ~value.i32;
	break; case COLTI_I64_ID:		result.i64 = ~value.i64;
	break; case COLTI_U8_ID:		result.ui8 = ~value.ui8;
	break; case COLTI_U16_ID:		result.ui16 = ~value.ui16;
	break; case COLTI_U32_ID:		result.ui32 = ~value.ui32;
	break; case COLTI_U64_ID:		result.ui64 = ~value.ui64;
	break; default: colti_assert(false, "Invalid operand for OP_BIT_NOT!");
	}
	return result;
}

void OpCode_Print(QWORD value, BuiltinTypeID type)
{
	switch (type)
	{
	break; case COLTI_BOOL_ID:		printf("%s", value.b ? "true\n" : "false\n");
	break; case COLTI_I8_ID:		printf("%"PRId8"\n", value.ui8);
	break; case COLTI_I16_ID:		printf("%"PRId16"\n", value.ui16);
	break; case COLTI_I32_ID:		printf("%"PRId32"\n", value.ui32);
	break; case COLTI_I64_ID:		printf("%"PRId64"\n", value.ui64);
	break; case COLTI_U8_ID:		printf("%"PRIu8"\n", value.ui8);
	break; case COLTI_U16_ID:		printf("%"PRIu16"\n", value.ui16);
	break; case COLTI_U32_ID:		printf("%"PRIu32"\n", value.ui32);
	break; case COLTI_U64_ID:		printf("%"PRIu64"\n", value.ui64);
	break; case COLTI_FLOAT_ID:	printf("%g\n", value.f);
	break; case COLTI_DOUBLE_ID:	printf("%g\n", value.d);
	break; default: colti_assert(false, "Invalid operand for OP_PRINT!");
	}
}
