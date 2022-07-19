#include "type.h"

bool is_type_equal(Type lhs, Type rhs)
{
	uint64_t real_lhs = is_type_signed_int(lhs) ? TypeGetID(lhs) - 4 : TypeGetID(lhs);
	uint64_t real_rhs = is_type_signed_int(rhs) ? TypeGetID(rhs) - 4 : TypeGetID(rhs);
	if (real_lhs == real_rhs)
		return true;
	return false;
}

bool is_type_greater(Type lhs, Type rhs)
{
	uint64_t real_lhs = is_type_signed_int(lhs) ? TypeGetID(lhs) - 4 : TypeGetID(lhs);
	uint64_t real_rhs = is_type_signed_int(rhs) ? TypeGetID(rhs) - 4 : TypeGetID(rhs);
	if (real_lhs > real_rhs)
		return true;
	return false;
}

bool is_type_builtin(Type type)
{
	return type.typeinfo->type_id <= ID_COLT_VOID;
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

Type builtin_inter_type(Type lhs, Type rhs)
{
	colt_assert(TypeGetID(lhs) <= ID_COLT_DOUBLE && TypeGetID(rhs) <= ID_COLT_DOUBLE, "Type should be built-in types!");

	uint64_t real_lhs = is_type_signed_int(lhs) ? TypeGetID(lhs) - 4 : TypeGetID(lhs);
	uint64_t real_rhs = is_type_signed_int(rhs) ? TypeGetID(rhs) - 4 : TypeGetID(rhs);
	if (real_lhs > real_rhs)
		return builtin_inter_type(rhs, lhs); //swap so that the 'lhs' has the lowest type

	if (is_type_signed_int(rhs) && is_type_unsigned_int(lhs))
	{
		//We return the biggest integer be it signed or unsigned
		//The + 4 comes from the fact that unsigned integer ID is - 4
		//from that of signed integer ID
		return (TypeGetID(lhs) + 4 < TypeGetID(rhs)) ? rhs : lhs;
	}
	else //We return the greater type
		return rhs;
}