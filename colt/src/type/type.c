#include "type.h"

bool is_type_builtin(Type type)
{
	return type.typeinfo->type_id <= ID_COLT_LSTRING;
}

bool is_type_signed_int(Type type)
{
	return type.typeinfo->type_id < ID_COLT_FLOAT && type.typeinfo->type_id > ID_COLT_U64;
}

bool is_type_unsigned_int(Type type)
{
	return type.typeinfo->type_id < ID_COLT_I8 && type.typeinfo->type_id > ID_COLT_BOOL;
}

bool is_type_integral(Type type)
{
	return type.typeinfo->type_id > ID_COLT_BOOL && type.typeinfo->type_id < ID_COLT_FLOAT;
}

bool is_type_floating(Type type)
{
	return type.typeinfo->type_id == ID_COLT_FLOAT || type.typeinfo->type_id == ID_COLT_DOUBLE;
}

Type type_unsigned_to_signed(Type type)
{
	Type ret = { .is_const = type.is_const };
	
	switch (type.typeinfo->type_id)
	{
	break; case ID_COLT_U8:
		ret.typeinfo = &ColtU8;
	break; case COLTI_U16_ID:
		ret.typeinfo = &ColtI16;
	break; case ID_COLT_U32:
		ret.typeinfo = &ColtI32;
	break; case ID_COLT_U64:
		ret.typeinfo = &ColtI64;
	break; default:
		colt_unreachable("Invalid argument!");
	}
	return ret;
}

TypeInfo* TypeInfoGetBuiltInFromID(BuiltinTypeID id)
{
	switch (id)
	{
	case ID_COLT_VOID:
		return &ColtVoid;
	case ID_COLT_BOOL:
		return &ColtBool;
	case ID_COLT_I8:
		return &ColtI8;
	case ID_COLT_I16:
		return &ColtI16;
	case ID_COLT_I32:
		return &ColtI32;
	case ID_COLT_I64:
		return &ColtI64;
	case ID_COLT_U8:
		return &ColtU8;
	case ID_COLT_U16:
		return &ColtU16;
	case ID_COLT_U32:
		return &ColtU32;
	case ID_COLT_U64:
		return &ColtU64;
	case ID_COLT_FLOAT:
		return &ColtFloat;
	case ID_COLT_DOUBLE:
		return &ColtFloat;
	case ID_COLT_LSTRING:
		return &ColtLString;
	default:
		colt_unreachable("Invalid argument!");
	}
}