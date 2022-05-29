/** @file colti_floating_value.h
* Contains typedefs representing floating point values in Colt.
*/

#ifndef HG_COLTI_FLOATING_VALUE
#define HG_COLTI_FLOATING_VALUE

/// @brief Colti Float (4 bytes single precision float)
typedef float ColtiFloat;
/// @brief The operand representing a float
#define COLTI_FLOAT_ID 9
/// @brief A string representing a Colt single-precision float
static const char* const ColtFloat_str = "float";

/// @brief Colti Double (8 bytes double precision float)
typedef double ColtiDouble;
/// @brief The operand representing a double
#define COLTI_DOUBLE_ID 10
/// @brief A string representing a Colt double-precision float
static const char* const ColtDouble_str = "double";

#endif //HG_COLTI_FLOATING_VALUE