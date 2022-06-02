/** @file byte_code.c
* Contains the definitions of the functions declared in 'byte_code.h'
*/

#include "byte_code.h"

const char* BuiltinTypeIDToString(BuiltinTypeID id)
{
	switch (id)
	{
	break; case ID_COLT_BOOL:		return "bool";
	break; case ID_COLT_DOUBLE:		return "double";
	break; case ID_COLT_FLOAT:		return "float";
	break; case ID_COLT_I8:			return "i8";
	break; case ID_COLT_I16:		return "i16";
	break; case ID_COLT_I32:		return "i32";
	break; case ID_COLT_I64:		return "i64";
	break; case ID_COLT_U8:			return "u8";
	break; case ID_COLT_U16:		return "u16";
	break; case ID_COLT_U32:		return "u32";
	break; case ID_COLT_U64:		return "u64";
	break; default:					return "UNKOWN";
	}
}

/**********************************
BYTE-CODE RUNNING
**********************************/

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
	break; default: colt_assert(false, "Invalid operand for OP_NEGATE!");
	}
	return result;
}

QWORD OpCode_Convert(QWORD value, BuiltinTypeID from, BuiltinTypeID to)
{
	//zeroes the memory
	QWORD ret = { .u64 = 0 };

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
			ret.b = (value.u64 != 0);
		break; case COLTI_FLOAT_ID:
			ret.b = (value.f != 0);
		break; case COLTI_DOUBLE_ID:
			ret.b = (value.d != 0);
		break; default:
			colt_assert(false, "Invalid operand for OP_CONVERT!");
		}
	break; case ID_COLT_FLOAT:
		switch (from)
		{
		case COLTI_I8_ID:
		case COLTI_I16_ID:
		case COLTI_I32_ID:
		case COLTI_I64_ID:
			ret.f = (float)value.i64;
			break;
		case COLTI_U8_ID:
		case COLTI_U16_ID:
		case COLTI_U32_ID:
		case COLTI_U64_ID:
			ret.f = (float)value.u64;
		break; case COLTI_BOOL_ID:
			ret.f = (value.b == 0 ? 0.0f : 1.0f);
		break; case COLTI_DOUBLE_ID:
			ret.f = (float)value.d;
		break; default:
			colt_assert(false, "Invalid operand for OP_CONVERT!");
		}
	break; case ID_COLT_DOUBLE:
		switch (from)
		{
		case COLTI_I8_ID:
		case COLTI_I16_ID:
		case COLTI_I32_ID:
		case COLTI_I64_ID:
			ret.d = (double)value.i64;
			break;
		case COLTI_U8_ID:
		case COLTI_U16_ID:
		case COLTI_U32_ID:
		case COLTI_U64_ID:
			ret.d = (double)value.u64;
		break; case COLTI_BOOL_ID:
			ret.d = (value.b == 0 ? 0.0 : 1.0);
		break; case COLTI_FLOAT_ID:
			ret.d = (double)value.f;
		break; default:
			colt_assert(false, "Invalid operand for OP_CONVERT!");
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
			ret.i64 = (int64_t)value.u64;
		break; case COLTI_BOOL_ID:
			ret.i64 = (value.b == 0 ? 0 : 1);
		break; case COLTI_FLOAT_ID:
			ret.i64 = (int64_t)value.f;
		break; case COLTI_DOUBLE_ID:
			ret.i64 = (int64_t)value.d;
		break; default:
			colt_assert(false, "Invalid operand for OP_CONVERT!");
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
			ret.u64 = (uint64_t)value.i64;
		break; case COLTI_BOOL_ID:
			ret.u64 = (value.b == 0 ? 0 : 1);
		break; case COLTI_FLOAT_ID:
			ret.u64 = (uint64_t)value.f;
		break; case COLTI_DOUBLE_ID:
			ret.u64 = (uint64_t)value.d;
		break; default:
			colt_assert(false, "Invalid operand for OP_CONVERT!");
		}
	break; default:
		colt_assert(false, "Invalid operand to convert to for OP_CONVERT!");
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
	break; case COLTI_U8_ID:		result.u8 = left.u8 + right.u8;
	break; case COLTI_U16_ID:		result.u16 = left.u16 + right.u16;
	break; case COLTI_U32_ID:		result.u32 = left.u32 + right.u32;
	break; case COLTI_U64_ID:		result.u64 = left.u64 + right.u64;
	break; case COLTI_FLOAT_ID:	result.f = left.f + right.f;
	break; case COLTI_DOUBLE_ID:	result.d = left.d + right.d;
	break; default: colt_assert(false, "Invalid operand for OP_ADD!");
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
	break; case COLTI_U8_ID:		result.u8 = left.u8 - right.u8;
	break; case COLTI_U16_ID:		result.u16 = left.u16 - right.u16;
	break; case COLTI_U32_ID:		result.u32 = left.u32 - right.u32;
	break; case COLTI_U64_ID:		result.u64 = left.u64 - right.u64;
	break; case COLTI_FLOAT_ID:	result.f = left.f - right.f;
	break; case COLTI_DOUBLE_ID:	result.d = left.d - right.d;
	break; default: colt_assert(false, "Invalid operand for OP_SUBTRACT!");
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
	break; case COLTI_U8_ID:		result.u8 = left.u8 * right.u8;
	break; case COLTI_U16_ID:		result.u16 = left.u16 * right.u16;
	break; case COLTI_U32_ID:		result.u32 = left.u32 * right.u32;
	break; case COLTI_U64_ID:		result.u64 = left.u64 * right.u64;
	break; case COLTI_FLOAT_ID:	result.f = left.f * right.f;
	break; case COLTI_DOUBLE_ID:	result.d = left.d * right.d;
	break; default: colt_assert(false, "Invalid operand for OP_MULTIPLY!");
	}
	return result;
}

QWORD OpCode_Divide(QWORD left, QWORD right, BuiltinTypeID type)
{
	QWORD result;

	if (!is_type_floating(type))
	{
		if (right.u64 == 0)
		{
			print_error_format("Division by zero is undefined for integers!");
			exit(EXIT_ASSERTION_FAILURE);
		}
	}

	switch (type)
	{
	break; case COLTI_I8_ID:		result.i8 = left.i8 / right.i8;
	break; case COLTI_I16_ID:		result.i16 = left.i16 / right.i16;
	break; case COLTI_I32_ID:		result.i32 = left.i32 / right.i32;
	break; case COLTI_I64_ID:		result.i64 = left.i64 / right.i64;
	break; case COLTI_U8_ID:		result.u8 = left.u8 / right.u8;
	break; case COLTI_U16_ID:		result.u16 = left.u16 / right.u16;
	break; case COLTI_U32_ID:		result.u32 = left.u32 / right.u32;
	break; case COLTI_U64_ID:		result.u64 = left.u64 / right.u64;
	break; case COLTI_FLOAT_ID:	result.f = left.f / right.f;
	break; case COLTI_DOUBLE_ID:	result.d = left.d / right.d;
	break; default: colt_assert(false, "Invalid operand for OP_DIVIDE!");
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
	break; case COLTI_U8_ID:		result.u8 = left.u8 & right.u8;
	break; case COLTI_U16_ID:		result.u16 = left.u16 & right.u16;
	break; case COLTI_U32_ID:		result.u32 = left.u32 & right.u32;
	break; case COLTI_U64_ID:		result.u64 = left.u64 & right.u64;
	break; default: colt_assert(false, "Invalid operand for OP_BIT_AND!");
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
	break; case COLTI_U8_ID:		result.u8 = left.u8 | right.u8;
	break; case COLTI_U16_ID:		result.u16 = left.u16 | right.u16;
	break; case COLTI_U32_ID:		result.u32 = left.u32 | right.u32;
	break; case COLTI_U64_ID:		result.u64 = left.u64 | right.u64;
	break; default: colt_assert(false, "Invalid operand for OP_BIT_OR!");
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
	break; case COLTI_U8_ID:		result.u8 = left.u8 ^ right.u8;
	break; case COLTI_U16_ID:		result.u16 = left.u16 ^ right.u16;
	break; case COLTI_U32_ID:		result.u32 = left.u32 ^ right.u32;
	break; case COLTI_U64_ID:		result.u64 = left.u64 ^ right.u64;
	break; default: colt_assert(false, "Invalid operand for OP_BIT_XOR!");
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
	break; case COLTI_U8_ID:		result.u8 = ~value.u8;
	break; case COLTI_U16_ID:		result.u16 = ~value.u16;
	break; case COLTI_U32_ID:		result.u32 = ~value.u32;
	break; case COLTI_U64_ID:		result.u64 = ~value.u64;
	break; default: colt_assert(false, "Invalid operand for OP_BIT_NOT!");
	}
	return result;
}

QWORD OpCode_BitShiftL(QWORD left, QWORD right, BuiltinTypeID type)
{
	QWORD result;

	switch (type)
	{
	break; case COLTI_I8_ID:		result.i8 = left.i8 << right.i8;
	break; case COLTI_I16_ID:		result.i16 = left.i16 << right.i16;
	break; case COLTI_I32_ID:		result.i32 = left.i32 << right.i32;
	break; case COLTI_I64_ID:		result.i64 = left.i64 << right.i64;
	break; case COLTI_U8_ID:		result.u8 = left.u8 << right.u8;
	break; case COLTI_U16_ID:		result.u16 = left.u16 << right.u16;
	break; case COLTI_U32_ID:		result.u32 = left.u32 << right.u32;
	break; case COLTI_U64_ID:		result.u64 = left.u64 << right.u64;
	break; default: colt_assert(false, "Invalid operand for OP_BIT_XOR!");
	}
	return result;
}

QWORD OpCode_BitShiftR(QWORD left, QWORD right, BuiltinTypeID type)
{
	QWORD result;

	switch (type)
	{
	break; case COLTI_I8_ID:		result.i8 = left.i8 >> right.i8;
	break; case COLTI_I16_ID:		result.i16 = left.i16 >> right.i16;
	break; case COLTI_I32_ID:		result.i32 = left.i32 >> right.i32;
	break; case COLTI_I64_ID:		result.i64 = left.i64 >> right.i64;
	break; case COLTI_U8_ID:		result.u8 = left.u8 >> right.u8;
	break; case COLTI_U16_ID:		result.u16 = left.u16 >> right.u16;
	break; case COLTI_U32_ID:		result.u32 = left.u32 >> right.u32;
	break; case COLTI_U64_ID:		result.u64 = left.u64 >> right.u64;
	break; default: colt_assert(false, "Invalid operand for OP_BIT_XOR!");
	}
	return result;
}

QWORD OpCode_Greater(QWORD left, QWORD right, BuiltinTypeID type)
{
	QWORD result;
	switch (type)
	{
	break; case COLTI_I8_ID:		result.b = left.i8 > right.i8;
	break; case COLTI_I16_ID:		result.b = left.i16 > right.i16;
	break; case COLTI_I32_ID:		result.b = left.i32 > right.i32;
	break; case COLTI_I64_ID:		result.b = left.i64 > right.i64;
	break; case COLTI_U8_ID:		result.b = left.u8 > right.u8;
	break; case COLTI_U16_ID:		result.b = left.u16 > right.u16;
	break; case COLTI_U32_ID:		result.b = left.u32 > right.u32;
	break; case COLTI_U64_ID:		result.b = left.u64 > right.u64;
	break; case COLTI_FLOAT_ID:		result.b = left.f > right.f;
	break; case COLTI_DOUBLE_ID:	result.b = left.d > right.d;
	break; default: colt_assert(false, "Invalid operand for OP_CMP_GREATER!");
	}
	return result;
}

QWORD OpCode_Less(QWORD left, QWORD right, BuiltinTypeID type)
{
	QWORD result;
	switch (type)
	{
	break; case COLTI_I8_ID:		result.b = left.i8 < right.i8;
	break; case COLTI_I16_ID:		result.b = left.i16 < right.i16;
	break; case COLTI_I32_ID:		result.b = left.i32 < right.i32;
	break; case COLTI_I64_ID:		result.b = left.i64 < right.i64;
	break; case COLTI_U8_ID:		result.b = left.u8 < right.u8;
	break; case COLTI_U16_ID:		result.b = left.u16 < right.u16;
	break; case COLTI_U32_ID:		result.b = left.u32 < right.u32;
	break; case COLTI_U64_ID:		result.b = left.u64 < right.u64;
	break; case COLTI_FLOAT_ID:		result.b = left.f < right.f;
	break; case COLTI_DOUBLE_ID:	result.b = left.d < right.d;
	break; default: colt_assert(false, "Invalid operand for OP_CMP_LESS!");
	}
	return result;
}

QWORD OpCode_GreaterEq(QWORD left, QWORD right, BuiltinTypeID type)
{
	QWORD result;
	switch (type)
	{
	break; case COLTI_I8_ID:		result.b = left.i8 >= right.i8;
	break; case COLTI_I16_ID:		result.b = left.i16 >= right.i16;
	break; case COLTI_I32_ID:		result.b = left.i32 >= right.i32;
	break; case COLTI_I64_ID:		result.b = left.i64 >= right.i64;
	break; case COLTI_U8_ID:		result.b = left.u8 >= right.u8;
	break; case COLTI_U16_ID:		result.b = left.u16 >= right.u16;
	break; case COLTI_U32_ID:		result.b = left.u32 >= right.u32;
	break; case COLTI_U64_ID:		result.b = left.u64 >= right.u64;
	break; case COLTI_FLOAT_ID:		result.b = left.f >= right.f;
	break; case COLTI_DOUBLE_ID:	result.b = left.d >= right.d;
	break; default: colt_assert(false, "Invalid operand for OP_CMP_GREATER_EQ!");
	}
	return result;
}

QWORD OpCode_LessEq(QWORD left, QWORD right, BuiltinTypeID type)
{
	QWORD result;
	switch (type)
	{
	break; case COLTI_I8_ID:		result.b = left.i8 <= right.i8;
	break; case COLTI_I16_ID:		result.b = left.i16 <= right.i16;
	break; case COLTI_I32_ID:		result.b = left.i32 <= right.i32;
	break; case COLTI_I64_ID:		result.b = left.i64 <= right.i64;
	break; case COLTI_U8_ID:		result.b = left.u8 <= right.u8;
	break; case COLTI_U16_ID:		result.b = left.u16 <= right.u16;
	break; case COLTI_U32_ID:		result.b = left.u32 <= right.u32;
	break; case COLTI_U64_ID:		result.b = left.u64 <= right.u64;
	break; case COLTI_FLOAT_ID:		result.b = left.f <= right.f;
	break; case COLTI_DOUBLE_ID:	result.b = left.d <= right.d;
	break; default: colt_assert(false, "Invalid operand for OP_CMP_LESS_EQ!");
	}
	return result;
}

QWORD OpCode_Equal(QWORD left, QWORD right, BuiltinTypeID type)
{
	QWORD result;
	switch (type)
	{
	break; case COLTI_BOOL_ID:		result.b = left.b == right.b;
	break; case COLTI_I8_ID:		result.b = left.i8 == right.i8;
	break; case COLTI_I16_ID:		result.b = left.i16 == right.i16;
	break; case COLTI_I32_ID:		result.b = left.i32 == right.i32;
	break; case COLTI_I64_ID:		result.b = left.i64 == right.i64;
	break; case COLTI_U8_ID:		result.b = left.u8 == right.u8;
	break; case COLTI_U16_ID:		result.b = left.u16 == right.u16;
	break; case COLTI_U32_ID:		result.b = left.u32 == right.u32;
	break; case COLTI_U64_ID:		result.b = left.u64 == right.u64;
	break; case COLTI_FLOAT_ID:		result.b = left.f == right.f;
	break; case COLTI_DOUBLE_ID:	result.b = left.d == right.d;
	break; default: colt_assert(false, "Invalid operand for OP_CMP_EQUAL!");
	}
	return result;
}

QWORD OpCode_NotEqual(QWORD left, QWORD right, BuiltinTypeID type)
{
	QWORD result;
	switch (type)
	{
	break; case COLTI_BOOL_ID:		result.b = left.b != right.b;
	break; case COLTI_I8_ID:		result.b = left.i8 != right.i8;
	break; case COLTI_I16_ID:		result.b = left.i16 != right.i16;
	break; case COLTI_I32_ID:		result.b = left.i32 != right.i32;
	break; case COLTI_I64_ID:		result.b = left.i64 != right.i64;
	break; case COLTI_U8_ID:		result.b = left.u8 != right.u8;
	break; case COLTI_U16_ID:		result.b = left.u16 != right.u16;
	break; case COLTI_U32_ID:		result.b = left.u32 != right.u32;
	break; case COLTI_U64_ID:		result.b = left.u64 != right.u64;
	break; case COLTI_FLOAT_ID:		result.b = left.f != right.f;
	break; case COLTI_DOUBLE_ID:	result.b = left.d != right.d;
	break; default: colt_assert(false, "Invalid operand for OP_CMP_NOT_EQUAL!");
	}
	return result;
}

void OpCode_Print(QWORD value, BuiltinTypeID type)
{
	switch (type)
	{
	break; case COLTI_BOOL_ID:		printf("%s", value.b ? "true\n" : "false\n");
	break; case COLTI_I8_ID:		printf("%"PRId8"\n", value.u8);
	break; case COLTI_I16_ID:		printf("%"PRId16"\n", value.u16);
	break; case COLTI_I32_ID:		printf("%"PRId32"\n", value.u32);
	break; case COLTI_I64_ID:		printf("%"PRId64"\n", value.u64);
	break; case COLTI_U8_ID:		printf("%"PRIu8"\n", value.u8);
	break; case COLTI_U16_ID:		printf("%"PRIu16"\n", value.u16);
	break; case COLTI_U32_ID:		printf("%"PRIu32"\n", value.u32);
	break; case COLTI_U64_ID:		printf("%"PRIu64"\n", value.u64);
	break; case COLTI_FLOAT_ID:	printf("%g\n", value.f);
	break; case COLTI_DOUBLE_ID:	printf("%g\n", value.d);
	break; default: colt_assert(false, "Invalid operand for OP_PRINT!");
	}
}
