#ifndef HG_COLTI_BYTE_CODE_GENERATOR
#define HG_COLTI_BYTE_CODE_GENERATOR

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

bool impl_gen_byte_code(Chunk* chunk, const Expr* expr);

bool impl_gen_code_unary(Chunk* chunk, const UnaryExpr* ptr);

bool impl_gen_code_binary(Chunk* chunk, const BinaryExpr* ptr);

bool impl_gen_code_literal(Chunk* chunk, const LiteralExpr* ptr);

#endif //HG_COLTI_BYTE_CODE_GENERATOR