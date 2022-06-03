/** @file byte_code_generator.h
* Contains the function responsible of generating byte-code from an expression.
* The `generateByteCode` functions is responsible of writing byte-code to a Chunk.
* It does so recursively for all its members and returns true if generation was successful.
* No checks are performed against NULL pointers. This is why `generateByteCode`
* should only be called when a valid AST is obtained.
*/

#ifndef HG_COLT_BYTE_CODE_GENERATOR
#define HG_COLT_BYTE_CODE_GENERATOR

#include "byte_code.h"	//byte-code to write
#include "chunk.h"		//for where to write the byte-code
#include "lang/expr.h"	//we convert expressions to byte-code
#include "lang/type.h"	//for built-in types

/// @brief Recursively converts an expression to byte-code in a Chunk
/// @param chunk The Chunk in which to write the byte-code
/// @param expr The expression to convert
bool generateByteCode(Chunk* chunk, const Expr* expr);

/*************************************
IMPLEMENTATION HELPERS
*************************************/

/// @brief Function which dispatches the expression to the write `gen_code_...`
/// @param chunk The Chunk where to write the byte-code
/// @param expr The expression to dispatch
/// @return True
bool impl_gen_byte_code(Chunk* chunk, const Expr* expr);

/// @brief Generate code necessary for unary operators
/// @param chunk The Chunk where to write the byte-code
/// @param ptr The expression to convert to byte-code
/// @return True if no error are encountered
bool impl_gen_code_unary(Chunk* chunk, const UnaryExpr* ptr);

/// @brief Generate code necessary for binary operators
/// @param chunk The Chunk where to write the byte-code
/// @param ptr The expression to convert to byte-code
/// @return True if no error are encountered
bool impl_gen_code_binary(Chunk* chunk, const BinaryExpr* ptr);

/// @brief Generate code necessary for literals
/// @param chunk The Chunk where to write the byte-code
/// @param ptr The expression to convert to byte-code
/// @return True if no error are encountered
bool impl_gen_code_literal(Chunk* chunk, const LiteralExpr* ptr);

/// @brief Generate code necessary for built-in conversions
/// @param chunk The Chunk where to write the byte-code
/// @param ptr The expression to convert to byte-code
/// @return True if no error are encountered
bool impl_gen_code_convert(Chunk* chunk, const ConvertExpr* ptr);

/// @brief Generate short jump code to ensure the last pushed operand is 'short_jump' 'cmp_to'
/// @param chunk The Chunk where to write the byte-code
/// @param short_jump The short jump OpCode to use
/// @param cmp_to The value to compare to
/// @param type The expression's type
void gen_integral_short_jmp(Chunk* chunk, OpCode short_jump, QWORD cmp_to, BuiltinTypeID type);

void gen_bitshift_ub_checks(Chunk* chunk, Type type);

#endif //HG_COLT_BYTE_CODE_GENERATOR