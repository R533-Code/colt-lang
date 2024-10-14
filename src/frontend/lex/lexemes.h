/*****************************************************************/ /**
 * @file   lexemes.h
 * @brief  Contains an enum for all valid tokens (representing a lexeme)
 *				 of the Colt language.
 *
 * @author RPC
 * @date   January 2024
 *********************************************************************/
#ifndef HG_COLT_LEXEME
#define HG_COLT_LEXEME

#include <colt/typedefs.h>
#include <colt/meta/reflect.h>
#include <colt/meta/map.h>

namespace clt::lng
{
  /// @brief Represents the lexeme of the Colt language.
  enum class Lexeme : u8;
} // namespace clt::lng

DECLARE_ENUM_WITH_TYPE(
    u8, clt::lng, Lexeme,
    /********* BEGINNING OF BINARY OPERATORS *******/

    /// @brief +
    TKN_PLUS,
    /// @brief -
    TKN_MINUS,
    /// @brief *
    TKN_STAR,

    /// @brief /
    TKN_SLASH,
    /// @brief %
    TKN_PERCENT,

    /// @brief &
    TKN_AND,
    /// @brief |
    TKN_OR,
    /// @brief ^
    TKN_CARET,
    /// @brief <<
    TKN_LESS_LESS,
    /// @brief >>
    TKN_GREAT_GREAT,

    /// @brief &&
    TKN_AND_AND,
    /// @brief ||
    TKN_OR_OR,

    /// @brief <
    TKN_LESS,
    /// @brief <=
    TKN_LESS_EQUAL,

    /// @brief >
    TKN_GREAT,
    /// @brief >=
    TKN_GREAT_EQUAL,

    /// @brief !=
    TKN_EXCLAM_EQUAL,
    /// @brief ==
    TKN_EQUAL_EQUAL,

    /********* BEGINNING OF ASSIGNMENT OPERATORS *******/

    /// @brief =
    TKN_EQUAL,
    /// @brief +=
    TKN_PLUS_EQUAL,
    /// @brief -=
    TKN_MINUS_EQUAL,
    /// @brief *=
    TKN_STAR_EQUAL,
    /// @brief /=
    TKN_SLASH_EQUAL,
    /// @brief %=
    TKN_PERCENT_EQUAL,
    /// @brief &=
    TKN_AND_EQUAL,
    /// @brief |=
    TKN_OR_EQUAL,
    /// @brief ^=
    TKN_CARET_EQUAL,
    /// @brief <<=
    TKN_LESS_LESS_EQUAL,
    /// @brief >>=
    TKN_GREAT_GREAT_EQUAL,

    /********* END OF ASSIGNMENT OPERATORS *******/
    /********* END OF BINARY OPERATORS *******/

    /// @brief ,
    TKN_COMMA,
    /// @brief ;
    TKN_SEMICOLON,
    /// @brief end of file
    TKN_EOF,
    /// @brief error detected lexeme
    TKN_ERROR,
    /// @brief )
    TKN_RIGHT_PAREN,
    /// @brief (
    TKN_LEFT_PAREN,

    /// @brief :
    TKN_COLON,
    /// @brief ::
    TKN_COLON_COLON,
    /// @brief }
    TKN_RIGHT_CURLY,
    /// @brief {
    TKN_LEFT_CURLY,

    /// @brief ->
    TKN_MINUS_GREAT,
    /// @brief =>
    TKN_EQUAL_GREAT,

    /// @brief ++
    TKN_PLUS_PLUS,
    /// @brief --
    TKN_MINUS_MINUS,
    /// @brief ~
    TKN_TILDE,
    /// @brief !
    TKN_EXCLAM,

    /// @brief [
    TKN_LEFT_SQUARE,
    /// @brief ]
    TKN_RIGHT_SQUARE,

    /********* BEGINNING OF LITERAL TOKENS *******/

    /// @brief true/false
    TKN_BOOL_L,
    /// @brief '.'
    TKN_CHAR_L,
    /// @brief NUMu8
    TKN_U8_L,
    /// @brief NUMu16
    TKN_U16_L,
    /// @brief NUMu32
    TKN_U32_L,
    /// @brief NUMu64
    TKN_U64_L,
    /// @brief NUMi8
    TKN_I8_L,
    /// @brief NUMi16
    TKN_I16_L,
    /// @brief NUMi32
    TKN_I32_L,
    /// @brief NUMi64
    TKN_I64_L,
    /// @brief REALf
    TKN_F32_L,
    /// @brief REALd
    TKN_F64_L,
    /// @brief UTF8 string "..." or "..."
    TKN_STRING_L,

    /********* END OF LITERAL TOKENS *******/

    /********* LOOPS AND CONDITIONALS *******/
    // DO NOT ADD ANY KEYWORD BEFORE TKN_KEYWORD_IF
    // ALL KEYWORDS MUST START WITH TKN_KEYWORD_ FOLLOWED BY
    // THE ACTUAL KEYWORD

    /// @brief if
    TKN_KEYWORD_if,
    /// @brief elif (which is just expanded as else if in the lexer)
    TKN_KEYWORD_elif,
    /// @brief else
    TKN_KEYWORD_else,

    /// @brief for
    TKN_KEYWORD_for,
    /// @brief while
    TKN_KEYWORD_while,
    /// @brief break
    TKN_KEYWORD_break,
    /// @brief continue
    TKN_KEYWORD_continue,

    /********* VARIABLE DECLARATIONS *******/

    /// @brief var
    TKN_KEYWORD_var,
    /// @brief var
    TKN_KEYWORD_let,

    /********* BEGINNING OF BUILTIN TYPES *******/

    // DO NOT REORDER THESE TYPES

    /// @brief bool
    TKN_KEYWORD_bool,
    /// @brief char
    TKN_KEYWORD_char,
    /// @brief u8
    TKN_KEYWORD_u8,
    /// @brief u16
    TKN_KEYWORD_u16,
    /// @brief u32
    TKN_KEYWORD_u32,
    /// @brief u64
    TKN_KEYWORD_u64,
    /// @brief i8
    TKN_KEYWORD_i8,
    /// @brief i16
    TKN_KEYWORD_i16,
    /// @brief i32
    TKN_KEYWORD_i32,
    /// @brief i64
    TKN_KEYWORD_i64,
    /// @brief f32
    TKN_KEYWORD_f32,
    /// @brief f64
    TKN_KEYWORD_f64,

    /// @brief ptr
    TKN_KEYWORD_ptr,
    /// @brief mutptr
    TKN_KEYWORD_mutptr,
    /// @brief opaque
    TKN_KEYWORD_opaque,

    /********* END OF BUILTIN TYPES *******/

    /// @brief fn
    TKN_KEYWORD_fn,
    /// @brief return
    TKN_KEYWORD_return,
    /// @brief extern
    TKN_KEYWORD_extern,

    /// @brief const
    TKN_KEYWORD_const,
    /// @brief mut
    TKN_KEYWORD_mut,
    /// @brief In parameter
    TKN_KEYWORD_in,
    /// @brief Out parameter
    TKN_KEYWORD_out,
    /// @brief Inout parameter
    TKN_KEYWORD_inout,
    /// @brief Move parameter
    TKN_KEYWORD_move,
    /// @brief Copy parameter
    TKN_KEYWORD_copy,
    /// @brief Forward parameter
    TKN_KEYWORD_forward,

    /// @brief typeof
    TKN_KEYWORD_typeof,

    /// @brief sizeof
    TKN_KEYWORD_sizeof,
    /// @brief alignof
    //TKN_KEYWORD_alignof,
    /// @brief alignas
    //TKN_KEYWORD_alignas,

    /// @brief as
    TKN_KEYWORD_as,
    /// @brief bit_as
    TKN_KEYWORD_bit_as,
    /// @brief using
    TKN_KEYWORD_using,
    /// @brief public
    TKN_KEYWORD_public,
    /// @brief protected
    TKN_KEYWORD_protected,
    /// @brief private
    TKN_KEYWORD_private,

    /// @brief module
    TKN_KEYWORD_module,

    /// @brief switch
    TKN_KEYWORD_switch,
    /// @brief case
    TKN_KEYWORD_case,
    /// @brief goto
    TKN_KEYWORD_goto,
    /// @brief default
    TKN_KEYWORD_default,

    /// @brief undefined
    TKN_KEYWORD_undefined,

    /// @brief any utf8 identifier
    TKN_IDENTIFIER,
    /// @brief \.
    TKN_DOT,
    /// @brief //... or multi-line comments
    TKN_COMMENT);

namespace clt::lng
{
  /// @brief Check if a Lexeme represents any assignment Lexeme (=, +=, ...)
  /// @param tkn The token to check for
  /// @return True if the Lexeme is an assignment Lexeme
  constexpr bool is_assignment(Lexeme tkn) noexcept
  {
    return Lexeme::TKN_EQUAL <= tkn && tkn <= Lexeme::TKN_GREAT_GREAT_EQUAL;
  }

  /// @brief Check if Lexeme represents any direct assignment (+=, -=, ...)
  /// @param tkn The token to check for
  /// @return True if the Lexeme is an direct assignment Lexeme
  constexpr bool is_direct_assignment(Lexeme tkn) noexcept
  {
    return Lexeme::TKN_EQUAL < tkn && tkn <= Lexeme::TKN_GREAT_GREAT_EQUAL;
  }

  /// @brief Converts a direct assignment to its non-assigning equivalent.
  /// Example: '+=' -> '+'
  /// @param tkn The direct assignment Lexeme
  /// @return Non-assigning Lexeme
  /// @pre is_direct_assignment(tkn)
  constexpr Lexeme direct_assignment_to_operation(Lexeme tkn) noexcept
  {
    assert_true("Invalid token!", is_direct_assignment(tkn));
    using enum Lexeme;
    constexpr u8 OFFSET = (u8)TKN_PLUS_EQUAL - (u8)TKN_PLUS;
    return static_cast<Lexeme>(static_cast<u8>(tkn) - OFFSET);
  }

  /// @brief Check if a Lexeme represents any comparison Lexeme (==, !=, ...)
  /// '||' and '&&' are not considered comparison tokens.
  /// @param tkn The token to check for
  /// @return True if the Lexeme is a comparison Lexeme
  constexpr bool is_comparison(Lexeme tkn) noexcept
  {
    return Lexeme::TKN_LESS <= tkn && tkn <= Lexeme::TKN_EQUAL_EQUAL;
  }

  /// @brief Check if a Lexeme represents any comparison Lexeme (==, !=, ...)
  /// '||' and '&&' are considered comparison tokens.
  /// @param tkn The token to check for
  /// @return True if the Lexeme is a comparison Lexeme
  constexpr bool is_bool_producer(Lexeme tkn) noexcept
  {
    return Lexeme::TKN_AND_AND <= tkn && tkn <= Lexeme::TKN_EQUAL_EQUAL;
  }

  /// @brief Check if a Lexeme represents any literal token ('.', "...", ...)
  /// @param tkn The token to check for
  /// @return True if the Lexeme is a literal token
  constexpr bool is_literal(Lexeme tkn) noexcept
  {
    return Lexeme::TKN_BOOL_L <= tkn && tkn <= Lexeme::TKN_STRING_L;
  }

  /// @brief Check if a Lexeme represents any unary operator (&, ++, ...)
  /// @param tkn The token to check for
  /// @return True if the Lexeme is a unary operator token
  constexpr bool is_unary(Lexeme tkn) noexcept
  {
    using enum clt::lng::Lexeme;
    return (TKN_PLUS <= tkn && tkn <= TKN_STAR) || tkn == TKN_AND
           || tkn == TKN_PLUS_PLUS || tkn == TKN_MINUS_MINUS || tkn == TKN_TILDE
           || tkn == TKN_EXCLAM;
  }

  /// @brief Check if a Lexeme represents any binary operator (+, -, ...).
  /// This does not check for assignment operators.
  /// @param tkn The token to check for
  /// @return True if the Lexeme is a binary operator token
  constexpr bool is_binary(Lexeme tkn) noexcept
  {
    return tkn <= Lexeme::TKN_EQUAL_EQUAL;
  }

  /// @brief Check if a Lexeme is a built-in type keyword (TKN_KEYWORD_bool...)
  /// @param tkn The token to check for
  /// @return True if any TKN_KEYWORD_* that represents a type (PTR and VOID are not types)
  constexpr bool is_builtin(Lexeme tkn) noexcept
  {
    return Lexeme::TKN_KEYWORD_bool <= tkn && tkn <= Lexeme::TKN_KEYWORD_f64;
  }

  /// @brief Returns the keywords count (in Lexeme).
  /// This function counts the number of lexeme declared
  /// as TKN_KEYWORD_* in code.
  /// @return The number of keywords in Lexeme
  consteval u8 keyword_count() noexcept
  {
    u8 count = 0;
    for (auto i : meta::reflect<Lexeme>::iter())
      if (meta::reflect<Lexeme>::to_str(i).starts_with("TKN_KEYWORD_"))
        count++;
    return count;
  }

  /// @brief Returns a sorted map of the keywords string to
  /// their corresponding lexeme.
  /// @return Table from keyword string to lexeme
  consteval auto keyword_map() noexcept
  {
    // Offset to the first keyword
    constexpr u8 first_key_offset = static_cast<u8>(Lexeme::TKN_KEYWORD_if);
    std::array<std::pair<std::string_view, Lexeme>, keyword_count()> array{};
    for (size_t i = 0; i < array.size(); i++)
    {
      Lexeme lex           = static_cast<Lexeme>(first_key_offset + i);
      std::string_view str = meta::reflect<Lexeme>::to_str(lex);
      assert_true(
          "Invalid Lexeme: all keywords must be contiguous!",
          str.find("TKN_KEYWORD_") != str.npos);
      // 12 for TKN_KEYWORD_
      str.remove_prefix(12);
      assert_true("Keyword size must at least be greater than 1!", str.size() > 1);
      array[i] = std::pair{str, lex};
    }
    return meta::Map{array};
  }
} // namespace clt::lng

DECLARE_ENUM_WITH_TYPE(
    u8, clt::lng, UnaryOp,
    /// @brief ++...
    OP_INC,
    /// @brief --...
    OP_DEC,
    /// @brief -...
    OP_NEGATE,
    /// @brief !...
    OP_BOOL_NOT,
    /// @brief ~...
    OP_BIT_NOT);

DECLARE_ENUM_WITH_TYPE(
    u8, clt::lng, BinaryOp,
    /*********** ARITHMETIC ***********/

    /// @brief +
    OP_SUM,
    /// @brief -
    OP_SUB,
    /// @brief *
    OP_MUL,
    /// @brief /
    OP_DIV,
    /// @brief %
    OP_MOD,

    /*********** BITWISE ***********/

    /// @brief &
    OP_BIT_AND,
    /// @brief |
    OP_BIT_OR,
    /// @brief ^
    OP_BIT_XOR,
    /// @brief <<
    OP_BIT_LSHIFT,
    /// @brief >>
    OP_BIT_RSHIFT,

    /*********** BOOLEANS ***********/

    /// @brief &&
    OP_BOOL_AND,
    /// @brief ||
    OP_BOOL_OR,
    /// @brief <
    OP_LESS,
    /// @brief <=
    OP_LESS_EQUAL,
    /// @brief >
    OP_GREAT,
    /// @brief >=
    OP_GREAT_EQUAL,
    /// @brief !=
    OP_NOT_EQUAL,
    /// @brief ==
    OP_EQUAL, );

DECLARE_ENUM_WITH_TYPE(
    u8, clt::lng, OpFamily, ARITHMETIC, BIT_LOGIC, BOOL_LOGIC, COMPARISON);

namespace clt::lng
{
  namespace details
  {
    /// @brief The operator precedence table
    constexpr std::array<u8, 18> OPERATOR_PRECEDENCE_TABLE = {
        12, 12, 13, 13, 13,   // + - * / %
        10, 10, 10, 11, 11,   // & | ^ << >>
        1,  1,                // '&&' '||'
        2,  2,  2,  2,  2,  2 // < <= > >= != ==
    };
  } // namespace details

  /// @brief Returns the precedence of a Lexeme
  /// @param tkn The token whose precedence to return
  /// @return The precedence of the token
  constexpr u8 op_precedence(BinaryOp op) noexcept
  {
    return op <= BinaryOp::OP_EQUAL
               ? details::OPERATOR_PRECEDENCE_TABLE[static_cast<u8>(op)]
               : 0;
  }

  /// @brief Returns the precedence of a Lexeme
  /// @param tkn The token whose precedence to return
  /// @return The precedence of the token
  constexpr u8 op_precedence(Lexeme tkn) noexcept
  {
    // break Pratt's parsing
    return tkn <= Lexeme::TKN_EQUAL_EQUAL
               ? details::OPERATOR_PRECEDENCE_TABLE[static_cast<u8>(tkn)]
               : 0;
  }

  /// @brief Returns the family of a binary operator
  /// @param op The operator whose family to return
  /// @return The family of 'op'
  constexpr OpFamily family_of(BinaryOp op) noexcept
  {
    using enum OpFamily;
    using enum BinaryOp;

    if (OP_SUM <= op && op <= OP_MOD)
      return ARITHMETIC;
    if (OP_BIT_AND <= op && op <= OP_BIT_RSHIFT)
      return BIT_LOGIC;
    if (op == OP_BOOL_AND || op == OP_BOOL_OR)
      return BOOL_LOGIC;
    if (OP_LESS <= op && op <= OP_EQUAL)
      return COMPARISON;
    unreachable("Unknow operator!");
  }

  /// @brief Converts a valid unary token to a UnaryOp
  /// @param tkn The Lexeme to convert
  /// @return Assertion failure or a valid UnaryOp
  constexpr UnaryOp token_to_unary(Lexeme tkn) noexcept
  {
    using enum Lexeme;

    switch_no_default(tkn)
    {
    case TKN_PLUS_PLUS:
      return UnaryOp::OP_INC;
    case TKN_MINUS_MINUS:
      return UnaryOp::OP_DEC;
    case TKN_MINUS:
      return UnaryOp::OP_NEGATE;
    case TKN_EXCLAM:
      return UnaryOp::OP_BOOL_NOT;
    case TKN_TILDE:
      return UnaryOp::OP_BIT_NOT;
    }
  }

  /// @brief Converts a valid binary token to a BinaryOp
  /// @param tkn The Lexeme to convert
  /// @return Assertion failure or a valid BinaryOp
  constexpr BinaryOp token_to_binary(Lexeme tkn) noexcept
  {
    assert_true("Expected a valid binary token!", is_binary(tkn));
    return static_cast<BinaryOp>(tkn);
  }

  /// @brief Converts a binary operator to its string equivalent
  /// @param op The operator
  /// @return The string representing the operator
  constexpr const char* to_str(lng::BinaryOp op) noexcept
  {
    using enum lng::BinaryOp;

    switch_no_default(op)
    {
    case OP_SUM:
      return "+";
    case OP_SUB:
      return "-";
    case OP_MUL:
      return "*";
    case OP_DIV:
      return "/";
    case OP_MOD:
      return "%";
    case OP_BIT_AND:
      return "&";
    case OP_BIT_OR:
      return "|";
    case OP_BIT_XOR:
      return "^";
    case OP_BIT_LSHIFT:
      return "<<";
    case OP_BIT_RSHIFT:
      return ">>";
    case OP_BOOL_AND:
      return "&&";
    case OP_BOOL_OR:
      return "||";
    case OP_LESS:
      return "<";
    case OP_LESS_EQUAL:
      return "<=";
    case OP_GREAT:
      return ">";
    case OP_GREAT_EQUAL:
      return ">=";
    case OP_NOT_EQUAL:
      return "!=";
    case OP_EQUAL:
      return "==";
    }
  }

  /// @brief Converts a unary operator to its string equivalent
  /// @param op The unary operator
  /// @return The string representing the operator
  constexpr const char* to_str(lng::UnaryOp op) noexcept
  {
    using enum lng::UnaryOp;

    switch_no_default(op)
    {
    case OP_INC:
      return "++";
    case OP_DEC:
      return "--";
    case OP_NEGATE:
      return "-";
    case OP_BOOL_NOT:
      return "!";
    case OP_BIT_NOT:
      return "~";
    }
  }
} // namespace clt::lng

#endif //!HG_COLT_LEXEME