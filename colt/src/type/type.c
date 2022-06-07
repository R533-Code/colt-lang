#include "type.h"

bool is_type_signed_int(uint64_t type)
{
	return type < ID_COLT_FLOAT&& type > ID_COLT_U64;
}

bool is_type_unsigned_int(uint64_t type)
{
	return type < ID_COLT_I8&& type > ID_COLT_BOOL;
}

bool is_type_integral(uint64_t type)
{
	return type > ID_COLT_BOOL && type < ID_COLT_FLOAT;
}

bool is_type_floating(uint64_t type)
{
	return type == ID_COLT_FLOAT || type == ID_COLT_DOUBLE;
}

Type type_unsigned_to_signed(uint64_t type)
{
	switch (type)
	{
	case COLTI_U8_ID:	return ColtI8;
	case COLTI_U16_ID:	return ColtI16;
	case COLTI_U32_ID:	return ColtI32;
	case COLTI_U64_ID:	return ColtI64;
	default:
		colt_assert(false, "Type ID was not that of an unsigned integer!");
		exit(1);
	}
}

Type type_get_from_id(BuiltinTypeID id)
{
	switch (id)
	{
	case ID_COLT_BOOL:
		return ColtBool;
	case ID_COLT_I8:
		return ColtI8;
	case ID_COLT_I16:
		return ColtI16;
	case ID_COLT_I32:
		return ColtI32;
	case ID_COLT_I64:
		return ColtI64;
	case ID_COLT_U8:
		return ColtU8;
	case ID_COLT_U16:
		return ColtU16;
	case ID_COLT_U32:
		return ColtU32;
	case ID_COLT_U64:
		return ColtU64;
	case ID_COLT_FLOAT:
		return ColtFloat;
	case ID_COLT_DOUBLE:
		return ColtFloat;
	default:
		colt_assert(false, "ID was not that of a Built-in Type!");
		exit(1);
	}
}