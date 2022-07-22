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

typedef struct
{
	Chunk* chunk;
	const ASTTable* table;
} ByteCodeGenerator;

Chunk generateByteCode(const ASTTable* table, const ExprArray* array);

/*************************************
IMPLEMENTATION HELPERS
*************************************/

void gen_byte_code(const Expr* expr, ByteCodeGenerator* gen);

void gen_code_unary(const UnaryExpr* ptr, ByteCodeGenerator* gen);

void gen_code_binary(const BinaryExpr* ptr, ByteCodeGenerator* gen);

void gen_code_literal(const LiteralExpr* ptr, ByteCodeGenerator* gen);

void gen_code_convert(const ConvertExpr* ptr, ByteCodeGenerator* gen);

void gen_code_condition(const ConditionExpr* ptr, ByteCodeGenerator* gen);

void gen_local_read(const LocalReadExpr* ptr, ByteCodeGenerator* gen);

void gen_local_write(const LocalWriteExpr* ptr, ByteCodeGenerator* gen);

void gen_code_while(const WhileExpr* ptr, ByteCodeGenerator* gen);

void gen_code_scope(const ScopeExpr* ptr, ByteCodeGenerator* gen);

void gen_code_convert(const ConvertExpr* ptr, ByteCodeGenerator* gen);

void gen_global_read(const GlobalReadExpr* ptr, ByteCodeGenerator* gen);

void gen_global_write(const GlobalWriteExpr* ptr, ByteCodeGenerator* gen);

void gen_and_and_bool_comparison(const BinaryExpr* ptr, ByteCodeGenerator* gen);

void gen_or_or_bool_comparison(const BinaryExpr* ptr, ByteCodeGenerator* gen);

/// @brief Generates the global and const memory pool, and return the offset to its beginning
/// @param chunk The Chunk where to write the byte-code
/// @param glob_table The GlobalTable whose non-const entries to write
void gen_global_pool(Chunk* chunk, const GlobalTable* glob_table);

/// @brief Write the offsets and string literals
/// @param chunk The Chunk where to write the byte-code
/// @param str_table The StringTable whose entries to write
void gen_string_literal_pool(Chunk* chunk, const StringTable* str_table);

/// @brief Generates the debug pool, and return the offset to its beginning
/// @param chunk The Chunk where to write the byte-code
/// @param glob_table The GlobalTable whose entries to write
void gen_debug_pool(Chunk* chunk, const ASTTable* glob_table);

#endif //HG_COLT_BYTE_CODE_GENERATOR