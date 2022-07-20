/** @file colt_floating_value.h
* Contains typedefs representing floating point values in Colt.
*/

#ifndef HG_COLT_FLOATING_VALUE
#define HG_COLT_FLOATING_VALUE

/// @brief Colti Float (4 bytes single precision float)
typedef float ColtFloat_t;
/// @brief The type-id of a Colt float
#define COLTI_FLOAT_ID 10
/// @brief A string representing a Colt single-precision float
static const char ColtFloat_str[] = "float";

/// @brief Colti Double (8 bytes double precision float)
typedef double ColtDouble_t;
/// @brief The type-id of a Colt double
#define COLTI_DOUBLE_ID 11
/// @brief A string representing a Colt double-precision float
static const char ColtDouble_str[] = "double";

#endif //HG_COLT_FLOATING_VALUE