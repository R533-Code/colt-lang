/** @file expr.h
* Contains skeleton of classes representing AST expressions.
* This header contains the struct Expr, which represents an unspecialized expression
* (as if it were a base class), whose members are in every expression-like struct.
* This allows to pass Expr*, regardless of the actual expression type.
* To be able to cast the Expr* to its actual type, a field 'identifier' (ExprIdentifier)
* contains actual expression type.
* 
* As all expression have a type, as with 'identifier', the 'expr_type' (which contains
* the actual type of the Colt expression) is a field contained in every expression struct.
* This is done to avoid the hassle of converting the Expr* to its actual expression-type,
* as this information is useless when just checking for the type of another expression.
*/

#ifndef HG_COLT_EXPR
#define HG_COLT_EXPR

#include "common.h"
#include "token.h"
#include "type/type.h"

/// @brief Compares an expression's type ID to an ID using '=='
#define ExprTypeEqualTypeID(expr, ID) ((expr)->expr_type.typeinfo->type_id == (ID))
/// @brief Compares an expression's type ID to another expression's type ID
#define ExprTypeEqualExprType(expr1, expr2) ((expr1)->expr_type.typeinfo->type_id == (expr2)->expr_type.typeinfo->type_id)
/// @brief Get the type id of an expression
#define ExprGetID(expr) ((expr)->expr_type.typeinfo->type_id)

/// @brief Represent the type of the expression holden by an Expr*.
/// As C does not have inheritance, every expression class contains
/// an 'identifier' member at the same layout location. By using this
/// information, we can cast that pointer to the initial expression struct.
typedef enum
{
	/// @brief Function expression
	EXPR_FN,
	/// @brief UnaryExpr type
	EXPR_UNARY,
	/// @brief BinaryExpr type
	EXPR_BINARY,
	/// @brief GlobalWriteExpr type
	EXPR_GLOB_WRITE,
	/// @brief GlobalReadExpr type
	EXPR_GLOB_READ,
	/// @brief ScopeExpr type
	EXPR_SCOPE,
	/// @brief LocalWriteExpr type
	EXPR_LOCAL_WRITE,
	/// @brief LocalReadExpr type
	EXPR_LOCAL_READ,
	/// @brief LiteralExpr type
	EXPR_LITERAL,
	/// @brief ConvertExpr type
	EXPR_CONVERT,
	/// @brief ConditionExpr type
	EXPR_CONDITION,
	/// @brief WhileExpr type
	EXPR_WHILE,
	/// @brief ContinueExpr type
	EXPR_CONTINUE,
	/// @brief BreakExpr type
	EXPR_BREAK,
} ExprIdentifier;

/// @brief Represents an unspecialized expression.
/// As C doesn't provide inheritance, we add its members to each
/// expression class. 'identifier' is used to know which type of expression
/// is the current expression. This allows us to cast an expression to its
/// actual type. 'expr_type' is the Colt type of the expression.
typedef struct
{
	/// @brief Allows to cast the expression to the right expression type
	ExprIdentifier identifier;
	/// @brief The expression type, which depends on its content
	Type expr_type;

	/// @brief The line number on which the expression begins
	uint64_t line_nb;
	/// @brief The line(s) where the expression was parsed
	StringView line;
	/// @brief The lexeme representing the whole expression
	StringView lexeme;
} Expr;


/**************************************************/


/// @brief A dynamic array of contiguous Expr*
typedef struct
{
	/// @brief The number of active Expr* in 'expressions'
	uint64_t count;
	/// @brief The capacity of the 'expressions'
	uint64_t capacity;
	/// @brief Pointer to the heap allocated memory buffer of Expr*
	Expr** expressions;
} ExprArray;

/// @brief Returns the first item in the array, without checking for boundaries
/// @param array The array from which to fetch the value
/// @return The first Expr* pushed
const Expr* expr_array_front(const ExprArray* array);

/// @brief Returns the last item in the array, without checking for boundaries
/// @param array The array from which to fetch the value
/// @return The last Expr* pushed
const Expr* expr_array_back(const ExprArray* array);

/// @brief Initializes an ExprArray
/// @param array The array to initialize
void ExprArrayInit(ExprArray* array);

/// @brief Frees resources used up by an ExprArray
/// @param array The array whose resources to free
void ExprArrayFree(ExprArray* array);

/// @brief Clears all the active Expr* stored
/// @param array The array whose resources to free
void ExprArrayClear(ExprArray* array);

/// @brief Pushes an Expr* at the end of an ExprArray, handling reallocations
/// @param array The array to modify
/// @param expr The Expr* to add
void ExprArrayPushBack(ExprArray* array, Expr* expr);


/**************************************************/


/// @brief Represents a unary expression.
/// A unary expression is composed of a unary operator (-, ~, +, @, !),
/// and the expression to which the operator is applied
typedef struct
{
	/// @brief should be EXPR_UNARY
	ExprIdentifier identifier;
	/// @brief The expression type, which depends on its content
	Type expr_type;
	
	/// @brief The line number on which the expression begins
	uint64_t line_nb;
	/// @brief The line(s) where the expression was parsed
	StringView line;
	/// @brief The lexeme representing the whole expression
	StringView lexeme;
	
	/// @brief The unary operator
	Token expr_operator;
	/// @brief The expression on which the unary operator is applied
	Expr* child;
} UnaryExpr;

/// @brief Represents a binary expression.
/// A binary expression is composed of a binary operator, and both expression
/// on which the operator is applied
typedef struct
{
	/// @brief should be EXPR_BINARY
	ExprIdentifier identifier;
	/// @brief The expression type, which depends on its content
	Type expr_type;

	/// @brief The line number on which the expression begins
	uint64_t line_nb;
	/// @brief The line(s) where the expression was parsed
	StringView line;
	/// @brief The lexeme representing the whole expression
	StringView lexeme;

	/// @brief The unary operator
	Token expr_operator;
	/// @brief The left hand side of the expression
	Expr* lhs;
	/// @brief The right hand side of the expression
	Expr* rhs;
} BinaryExpr;

/// @brief Represents a literal expression.
/// A LiteralExpr can contain literal integer, floating point, or boolean
typedef struct
{
	/// @brief should be EXPR_LITERAL
	ExprIdentifier identifier;
	/// @brief The expression type, which depends on the type of 'value'
	Type expr_type;

	/// @brief The line number on which the expression begins
	uint64_t line_nb;
	/// @brief The line(s) where the expression was parsed
	StringView line;
	/// @brief The lexeme representing the whole expression
	StringView lexeme;

	/// @brief The literal value
	QWORD value;
} LiteralExpr;

/// @brief Represents a conversion expression
typedef struct
{
	/// @brief Allows to cast the expression to the right expression type
	ExprIdentifier identifier;
	/// @brief The expression type, which depends on its content
	Type expr_type;

	/// @brief The line number on which the expression begins
	uint64_t line_nb;
	/// @brief The line(s) where the expression was parsed
	StringView line;
	/// @brief The lexeme representing the whole expression (or the cast)
	StringView lexeme;

	/// @brief The expression to be casted
	Expr* child;
} ConvertExpr;

/// @brief Represents a global variable read
typedef struct
{
	/// @brief should be EXPR_GLOB_READ
	ExprIdentifier identifier;
	/// @brief The expression type, which depends on the type of 'value'
	Type expr_type;

	/// @brief The line number on which the expression begins
	uint64_t line_nb;
	/// @brief The line(s) where the expression was parsed
	StringView line;
	/// @brief The lexeme representing the whole expression
	StringView lexeme;

	/// @brief The literal value
	StringView var_name;
} GlobalReadExpr;

/// @brief Represents a global variable write
typedef struct
{
	/// @brief should be EXPR_GLOB_READ
	ExprIdentifier identifier;
	/// @brief The expression type, which depends on the type of 'var_name'
	Type expr_type;

	/// @brief The line number on which the expression begins
	uint64_t line_nb;
	/// @brief The line(s) where the expression was parsed
	StringView line;
	/// @brief The lexeme representing the whole expression
	StringView lexeme;

	/// @brief The literal value
	StringView var_name;
	/// @brief The value to write to the global
	Expr* value;
} GlobalWriteExpr;

/// @brief Represents a local variable read.
/// The position of the 'var_name' member is the same as in LocalWriteExpr
/// which allows some optimizations in functions such as ScopeExprIsVarDeclared.
typedef struct
{
	/// @brief should be EXPR_GLOB_READ
	ExprIdentifier identifier;
	/// @brief The expression type, which depends on the type of 'value'
	Type expr_type;

	/// @brief The line number on which the expression begins
	uint64_t line_nb;
	/// @brief The line(s) where the expression was parsed
	StringView line;
	/// @brief The lexeme representing the whole expression
	StringView lexeme;

	/// @brief The literal value
	StringView var_name;
	/// @brief The stack offset of the variable
	uint64_t offset;
} LocalReadExpr;

/// @brief Represents a local variable write.
/// The position of the 'var_name' member is the same as in LocalReadExpr
/// which allows some optimizations in functions such as ScopeExprIsVarDeclared.
typedef struct
{
	/// @brief should be EXPR_GLOB_READ
	ExprIdentifier identifier;
	/// @brief The expression type, which depends on the type of 'var_name'
	Type expr_type;

	/// @brief The line number on which the expression begins
	uint64_t line_nb;
	/// @brief The line(s) where the expression was parsed
	StringView line;
	/// @brief The lexeme representing the whole expression
	StringView lexeme;

	/// @brief The variable name
	StringView var_name;
	/// @brief The stack offset of the variable
	uint64_t offset;
	/// @brief The value to write to the global
	Expr* value;
} LocalWriteExpr;

/// @brief Forward declaration of ScopeExpr as ScopeExpr includes a pointer to itself
typedef struct ScopeExpr ScopeExpr;

/// @brief Represents a scope.
struct ScopeExpr
{
	/// @brief should be EXPR_SCOPE
	ExprIdentifier identifier;
	/// @brief The expression type, which should be 'void'
	Type expr_type;

	/// @brief The line number on which the expression begins
	uint64_t line_nb;
	/// @brief The line(s) where the expression was parsed
	StringView line;
	/// @brief The lexeme representing the whole expression
	StringView lexeme;

	/// @brief The array of expressions contained in the scope
	ExprArray array;
	/// @brief The variable count
	uint64_t var_count;
	/// @brief Pointer to the parent scope, which can be NULL
	ScopeExpr* parent_scope;
};

/// @brief Check if a variable already exists in a Scope or its parent scope
/// @param scope Pointer to the scope in which to check
/// @param name The variable name to check for
/// @return True if the variable has already been declared
bool ScopeExprIsVarDeclared(ScopeExpr* scope, StringView name);

/// @brief Searches for the first instance of a LocalWriteExpr/LocalReadExpr of name 'name'
/// @param scope The scope in which to search before recursing in its parent scope
/// @param name The name of the variable to search for
/// @return LocalWriteExpr*/LocalReadExpr* or NULL if not found
Expr* ScopeExprFindVar(ScopeExpr* scope, StringView name);

/// @brief Returns the offset to the current scope.
/// @param scope The scope whose begin offset is to be computed
/// @return An uint64_t representing the count of variable of all the parents scopes added together
uint64_t ScopeExprGetOffset(ScopeExpr* scope);

/// @brief Represents an if/elif/else chain
typedef struct
{
	/// @brief should be EXPR_CONDITION
	ExprIdentifier identifier;
	/// @brief The expression type, which should be 'void'
	Type expr_type;

	/// @brief The line number on which the expression begins
	uint64_t line_nb;
	/// @brief The line(s) where the expression was parsed
	StringView line;
	/// @brief The lexeme representing the whole expression
	StringView lexeme;

	/// @brief The if condition (which cannot be NULL)
	Expr* if_condition;
	/// @brief The expression to execute if 'if_condition' evaluates to true
	Expr* if_execute;
	/// @brief The conditions of each elif
	ExprArray elif_conditions;
	/// @brief The expression to execute for each of the 'elif_conditions'
	ExprArray elif_executes;
	/// @brief The expression to execute if all the preceding elif conditions evaluates to false (which can be NULL)
	Expr* else_execute;
} ConditionExpr;

/// @brief Represents a While-loop
typedef struct
{
	/// @brief should be EXPR_WHILE
	ExprIdentifier identifier;
	/// @brief The expression type, which should be 'void'
	Type expr_type;

	/// @brief The line number on which the expression begins
	uint64_t line_nb;
	/// @brief The line(s) where the expression was parsed
	StringView line;
	/// @brief The lexeme representing the whole expression
	StringView lexeme;

	/// @brief The while condition (which cannot be NULL)
	Expr* while_condition;
	/// @brief The expression to execute while 'while_condition' evaluates to true
	Expr* while_body;
} WhileExpr;

/// @brief Represents a 'continue' in a loop
typedef struct
{
	/// @brief should be EXPR_CONTINUE
	ExprIdentifier identifier;
	/// @brief The expression type, which should be 'void'
	Type expr_type;

	/// @brief The line number on which the expression begins
	uint64_t line_nb;
	/// @brief The line(s) where the expression was parsed
	StringView line;
	/// @brief The lexeme representing the whole expression
	StringView lexeme;
} ContinueExpr;

/// @brief Represents a 'break' in a loop
typedef struct
{
	/// @brief should be EXPR_BREAK
	ExprIdentifier identifier;
	/// @brief The expression type, which should be 'void'
	Type expr_type;

	/// @brief The line number on which the expression begins
	uint64_t line_nb;
	/// @brief The line(s) where the expression was parsed
	StringView line;
	/// @brief The lexeme representing the whole expression
	StringView lexeme;
} BreakExpr;

/// @brief Allocates a new literal expression on the heap, initializing it
/// @param value The value of the literal expression
/// @param type The type of the literal expression
/// @param line_nb The line number from which the expression is extracted
/// @param line A StringView over the line containing the expression
/// @param lexeme A StringView over the lexeme representing the expression
/// @return A pointer to a heap allocated LiteralExpr
Expr* makeLiteralExpr(QWORD value, Type type, uint64_t line_nb, StringView line, StringView lexeme);

/// @brief Allocates a new unary expression on the heap, initializing it
/// @param unary_operator The operator of the unary expression (+, -, !, @, ~)
/// @param child The expression on which the apply the unary operator
/// @param type The type of the expression
/// @param line_nb The line number from which the expression is extracted
/// @param line A StringView over the line containing the expression
/// @param lexeme A StringView over the lexeme representing the expression
/// @return A pointer to a heap allocated UnaryExpr
Expr* makeUnaryExpr(Token unary_operator, Expr* child, Type type, uint64_t line_nb, StringView line, StringView lexeme);

/// @brief Allocates a new binary expression on the heap initializing it
/// @param lhs The left hand side of the operator
/// @param binary_operator The operator
/// @param rhs The right hand side of the operator
/// @param expr_type The expression type
/// @param line_nb The line number from which the expression is extracted
/// @param line A StringView over the line containing the expression
/// @param lexeme A StringView over the lexeme representing the expression
/// @return A pointer to a heap allocated BinaryExpr
Expr* makeBinaryExpr(Expr* lhs, Token binary_operator, Expr* rhs, Type expr_type, uint64_t line_nb, StringView line, StringView lexeme);

/// @brief Allocates a new convert expression on the heap, initializing it
/// @param expr The expression to cast
/// @param convert_to The Type to which to cast to
/// @param line_nb The line number from which the expression is extracted
/// @param line A StringView over the line containing the expression
/// @param lexeme A StringView over the lexeme representing the expression
/// @return A pointer to a heap allocated ConvertExpr
Expr* makeConvertExpr(Expr* expr, Type convert_to, uint64_t line_nb, StringView line, StringView lexeme);

/// @brief Allocates a new ConditionExpr on the heap, initializing some of its fields
/// @return A pointer to a heap allocated ConditionExpr
Expr* makeConditionExpr();

/// @brief Allocates a new variable expression on the heap, initializing it
/// @param var_name The variable name
/// @param var_type The variable type
/// @param line_nb The line number from which the expression is extracted
/// @param line A StringView over the line containing the expression
/// @param lexeme A StringView over the lexeme representing the expression
/// @return A pointer to a heap allocated GlobalReadExpr
Expr* makeGlobalReadExpr(StringView var_name, Type var_type, uint64_t line_nb, StringView line, StringView lexeme);

/// @brief Allocates a new variable expression on the heap, initializing it
/// @param var_name The variable name
/// @param var_type The variable type
/// @param value The value to write to the global
/// @param line_nb The line number from which the expression is extracted
/// @param line A StringView over the line containing the expression
/// @param lexeme A StringView over the lexeme representing the expression
/// @return A pointer to a heap allocated GlobalWriteExpr
Expr* makeGlobalWriteExpr(StringView var_name, Type var_type, Expr* value, uint64_t line_nb, StringView line, StringView lexeme);

/// @brief Allocates a new variable expression on the heap, initializing it
/// @param var_name The variable name
/// @param var_type The variable type
/// @param var_offset The variable offset on the stack
/// @param line_nb The line number from which the expression is extracted
/// @param line A StringView over the line containing the expression
/// @param lexeme A StringView over the lexeme representing the expression
/// @return A pointer to a heap allocated LocalReadExpr
Expr* makeLocalReadExpr(StringView var_name, Type var_type, uint64_t var_offset, uint64_t line_nb, StringView line, StringView lexeme);

/// @brief Allocates a new variable expression on the heap, initializing it
/// @param var_name The variable name
/// @param var_type The variable type
/// @param var_offset The variable offset on the stack
/// @param value The value to write to the global
/// @param line_nb The line number from which the expression is extracted
/// @param line A StringView over the line containing the expression
/// @param lexeme A StringView over the lexeme representing the expression
/// @return A pointer to a heap allocated LocalWriteExpr
Expr* makeLocalWriteExpr(StringView var_name, Type var_type, uint64_t var_offset, Expr* value, uint64_t line_nb, StringView line, StringView lexeme);

/// @brief Allocates a new While expression on the heap, initializing it
/// @param cond The condition of the while-loop
/// @param body The body to execute if the condition is true
/// @return A pointer to a heap allocated WhileExpr
Expr* makeWhileExpr(Expr* cond, Expr* body);

/// @brief Allocates a new scope expression on the heap, initializing it
/// @param parent_scope The parent scope, or NULL
/// @return A pointer to a heap allocated ScopeExpr
Expr* makeScopeExpr(ScopeExpr* parent_scope);

/// @brief Recursively frees a heap-allocated expression.
/// As an Expr* can be a BinaryExpr, or an expression with child expression,
/// we cannot just free the pointer received from a make...Expr.
/// @param ptr The Expr* to free
void freeExpr(Expr* ptr);

#endif //HG_COLT_EXPR