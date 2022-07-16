#include "type.h"

bool is_type_builtin(Type type)
{
	return type.typeinfo->type_id <= ID_COLT_CHAR;
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