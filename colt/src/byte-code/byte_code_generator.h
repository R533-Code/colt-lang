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
#include "type/type.h"	//for built-in types

/// @brief Recursively converts an expression to byte-code in a Chunk
/// @param chunk The Chunk in which to write the byte-code
/// @param var_table VariableTable containing variables
/// @param array The array of Expr* expression to convert
/// @param print_last_expr If true, prints the last expression
/// @return True if no error are encountered
bool generateByteCode(Chunk* chunk, const ASTTable* var_table, const ExprArray* array, bool print_last_expr);

/*************************************
IMPLEMENTATION HELPERS
*************************************/

/// @brief Generates the global memory pool, and return the offset to its beginning
/// @param chunk The Chunk where to write the byte-code
/// @param var_table The VariableTable whose non-const entries to write
/// @return The offset to the GLOBAL section
uint64_t gen_global_pool(Chunk* chunk, const VariableTable* var_table);

/// @brief Generates the const memory pool, and return the offset to its beginning
/// @param chunk The Chunk where to write the byte-code
/// @param var_table The VariableTable whose const entries to write
/// @return The offset to the CONST section
uint64_t gen_const_pool(Chunk* chunk, const VariableTable* var_table);

/// @brief Write the offsets and string literals
/// @param chunk The Chunk where to write the byte-code
/// @param str_table The StringTable whose entries to write
/// @return The offset to the GLOBAL section
uint64_t gen_string_literal_pool(Chunk* chunk, const StringTable* str_table);

/// @brief Generates the debug pool, and return the offset to its beginning
/// @param chunk The Chunk where to write the byte-code
/// @param var_table The VariableTable whose entries to write
/// @return The offset to the DEBUG section
uint64_t gen_debug_pool(Chunk* chunk, const ASTTable* var_table);

/// @brief Function which dispatches the expression to the write `gen_code_...`
/// @param chunk The Chunk where to write the byte-code
/// @param table ASTTable containing variables and strings
/// @param expr The expression to dispatch
/// @return True if no error are encountered
bool gen_byte_code(Chunk* chunk, const ASTTable* table, const Expr* expr);

/// @brief Generate code necessary for unary operators
/// @param chunk The Chunk where to write the byte-code
/// @param table VariableTable containing variables
/// @param ptr The expression to convert to byte-code
/// @return True if no error are encountered
bool impl_gen_code_unary(Chunk* chunk, const ASTTable* table, const UnaryExpr* ptr);

/// @brief Generate code necessary for binary operators
/// @param chunk The Chunk where to write the byte-code
/// @param table VariableTable containing variables
/// @param ptr The expression to convert to byte-code
/// @return True if no error are encountered
bool impl_gen_code_binary(Chunk* chunk, const ASTTable* table, const BinaryExpr* ptr);

/// @brief Generate code necessary for literals.
/// As literals never have child expressions, it doesn't need the variable
/// VariableTable.
/// @param chunk The Chunk where to write the byte-code
/// @param table VariableTable containing variables
/// @param ptr The expression to convert to byte-code
/// @return True if no error are encountered
bool impl_gen_code_literal(Chunk* chunk, const ASTTable* table, const LiteralExpr* ptr);

/// @brief Generate code necessary for built-in conversions
/// @param chunk The Chunk where to write the byte-code
/// @param table VariableTable containing variables
/// @param ptr The expression to convert to byte-code
/// @return True if no error are encountered
bool impl_gen_code_convert(Chunk* chunk, const ASTTable* table, const ConvertExpr* ptr);

/// @brief Generate code necessary for variable to r-value conversion
/// @param chunk The Chunk where to write the byte-code
/// @param table The variable table
/// @param ptr The pointer to the expression
/// @return True if no error are encountered
bool gen_global_variable_load(Chunk* chunk, const ASTTable* table, const VariableExpr* ptr);

/// @brief Generate the code necessary for a global variable assignment
/// @param chunk The Chunk where to write the byte-code
/// @param table The variable table from which to extract the offsets
/// @param ptr The pointer to the expression
/// @return True if no error are encountered
bool gen_global_variable_assigment(Chunk* chunk, const ASTTable* table, const BinaryExpr* ptr);

/// @brief Generate the code necessary for an bool and (&&) comparison
/// @param chunk The Chunk where to write the byte-code
/// @param table The ASTTable of the AST whose being parsed
/// @param ptr The BinaryExpr whose operator is &&
/// @return True
bool gen_and_and_bool_comparison(Chunk* chunk, const ASTTable* table, const BinaryExpr* ptr);

/// @brief Generate the code necessary for an bool or (||) comparison
/// @param chunk The Chunk where to write the byte-code
/// @param table The ASTTable of the AST whose being parsed
/// @param ptr The BinaryExpr whose operator is ||
/// @return True
bool gen_or_or_bool_comparison(Chunk* chunk, const ASTTable* table, const BinaryExpr* ptr);

/// @brief Generate short jump code to ensure the last pushed operand is 'short_jump' 'cmp_to'
/// @param chunk The Chunk where to write the byte-code
/// @param short_jump The short jump OpCode to use
/// @param cmp_to The value to compare to
/// @param type The expression's type
void gen_integral_short_jmp(Chunk* chunk, OpCode short_jump, QWORD cmp_to, BuiltinTypeID type);

/// @brief Generates short jump codes to ensure the last pushed operand is a valid operand for a bit-shift operation performed on 'type'
/// @param chunk The Chunk where to write the byte-code
/// @param type The type of the expression
void gen_bitshift_ub_checks(Chunk* chunk, Type type);

void gen_signed_addition_checks(Chunk* chunk, BuiltinTypeID type);

void gen_signed_subtraction_checks(Chunk* chunk, BuiltinTypeID type);

void gen_signed_multiplication_checks(Chunk* chunk, BuiltinTypeID type);

void gen_signed_division_checks(Chunk* chunk, BuiltinTypeID type);


#endif //HG_COLT_BYTE_CODE_GENERATOR