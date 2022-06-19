#ifndef HG_COLT_OPTION
#define HG_COLT_OPTION

#include <stdbool.h>

/// @brief Options used by the AST for warnings, messages and errors.
/// The name of the members is chosen so that `memset`ing the whole struct
/// to 0 gives the default values of each member.
typedef struct
{
	/// @brief If false, warns if an expression result is not used.
	/// When REPLing should be true: we don't want to warn if the user is doing arithmetic
	bool no_warn_unused_result;
	/// @brief If false, warns if a variable is not initialized
	bool no_warn_uninitialized;
	/// @brief If false, optimizes constant expressions
	bool no_constant_folding;
	/// @brief If false, allows messages to be printed
	bool no_message;
	/// @brief If false, allows warnings to be printed
	bool no_warning;
	/// @brief If false, allows errors to be printed
	bool no_error;
} ColtScanOptions;

#endif //HG_COLT_OPTION