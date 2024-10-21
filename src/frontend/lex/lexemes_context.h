#ifndef HG_COLT_TOKEN_BUFFER
#define HG_COLT_TOKEN_BUFFER

#include "colt/dsa/set.h"
#include "colt/dsa/trie.h"
#include "colt/dsa/vector.h"
#include "colt/dsa/string.h"
#include "colt/bit/bitfields.h"
#include "err/error_reporter.h"
#include "err/compiler_limits.h"
#include "lexemes.h"

namespace clt::lng
{
  // Forward declaration
  class LexemesContext;
  // Forward declaration
  struct Lexer;

  /// @brief Lexes 'to_parse'.
  /// 'to_parse' is a byte view as the lexer has to handle invalid unicode.
  /// @param reporter The reporter used to generate error/warnings/messages
  /// @param to_parse The bytes to parse
  /// @return A LexemesContext containing parsed lexemes
  LexemesContext lex(ErrorReporter& reporter, View<u8> to_parse) noexcept;

  /// @brief Contains information about a single Token
  struct LexemeInfo
  {
    /// @brief 0-based column_nb
    u32 column_nb;
    /// @brief Size of the lexeme
    u32 size;
    /// @brief 0-based start line number
    u32 line_start;
    /// @brief 0-based end line number
    u32 line_end;

    /// @brief Check if the Token spans on a single line
    /// @return True if the Token spans on a single line
    constexpr bool is_single_line() const noexcept { return line_end == line_start; }
  };

  /// @brief Token representing a Lexeme
  class LexemeToken
  {
    /// @brief The bitfield representing the token.
    /// 0 -> (8 bits) The actual lexeme
    /// 1 -> (24 bits) The literal index (this depends on the actual lexeme)
    /// 2 -> (32 bits) The TokenInfo index
    using Fields = bit::Bitfields<
        u64, bit::Bitfield<0, 8>, bit::Bitfield<1, 24>, bit::Bitfield<2, 32>>;

    static_assert(
        meta::reflect<Lexeme>::max() < 256, "Cannot fit Lexeme in 8 bits!");

    /// @brief The bit fields storing the data
    Fields field;

    /// @brief The lexeme information index
    /// @return The information index
    constexpr u32 info_index() const noexcept { return (u32)field.get<2>(); }
    /// @brief The literal index
    /// @return The literal index
    constexpr u32 literal_index() const noexcept { return (u32)field.get<1>(); }

    /// @brief Constructor
    /// @param lexeme The lexeme
    /// @param info_index The info index
    /// @param literal The literal index
    constexpr LexemeToken(Lexeme lexeme, u32 info_index, u32 literal = 0) noexcept
        : field(InPlace, static_cast<u8>(lexeme), literal, info_index)
    {
      compiler_assert_true(
          "Too much source code literals in a single file!",
          info_index == field.get<2>());
    }

  public:
    friend class LexemesContext;

    LexemeToken()                                                 = delete;
    constexpr LexemeToken(LexemeToken&&) noexcept                 = default;
    constexpr LexemeToken(const LexemeToken&) noexcept            = default;
    constexpr LexemeToken& operator=(LexemeToken&&) noexcept      = default;
    constexpr LexemeToken& operator=(const LexemeToken&) noexcept = default;

    /// @brief Converts a Token to the Lexeme it represents
    constexpr operator Lexeme() const noexcept
    {
      return static_cast<Lexeme>(field.get<0>());
    }

    /// @brief Returns the Lexeme the Token represents
    /// @return The Lexeme representing the Token
    constexpr Lexeme lexeme() const noexcept { return static_cast<Lexeme>(*this); }

    COLT_ENABLE_REFLECTION();
  };

  /// @brief Represents a range of Token
  class LexemeTokenRange
  {
    /// @brief The beginning of the range
    u32 start_index;
    /// @brief The end of the range (non-inclusive)
    u32 end_index;

    /// @brief Constructs a range of Token
    /// @param start The start of the range
    /// @param end The end of the range
    constexpr LexemeTokenRange(u32 start, u32 end) noexcept
        : start_index(start)
        , end_index(end)
    {
    }

    friend class LexemesContext;

  public:
    LexemeTokenRange()                                                 = delete;
    constexpr LexemeTokenRange(LexemeTokenRange&&) noexcept            = default;
    constexpr LexemeTokenRange(const LexemeTokenRange&) noexcept       = default;
    constexpr LexemeTokenRange& operator=(LexemeTokenRange&&) noexcept = default;
    constexpr LexemeTokenRange& operator=(const LexemeTokenRange&) noexcept =
        default;

    COLT_ENABLE_REFLECTION();
  };

  /// @brief The result of lexing a Colt source.
  class LexemesContext
  {
    /// @brief The array of literal integers
    Vector<num::BigInt> int_literals = make_vector<num::BigInt>();
    /// @brief The array of literal reals
    Vector<num::BigRational> float_literals = make_vector<num::BigRational>();
    /// @brief The array of literal chars
    Vector<u32> char_literals = make_vector<u32>();
    /// @brief The array of identifiers
    Vector<u8StringView> identifiers = make_vector<u8StringView>();
    /// @brief The array of string literals
    Vector<u8String> str_literals = make_vector<u8String>();
    /// @brief The array of lines
    Vector<u8StringView> lines = make_vector<u8StringView>();
    /// @brief The array of token information
    Vector<LexemeInfo> tokens_info = make_vector<LexemeInfo>();
    /// @brief The array of tokens
    Vector<LexemeToken> tokens = make_vector<LexemeToken>();

    // Friend declaration to use add_token
    friend struct Lexer;

  public:
    /// @brief Default constructor
    LexemesContext() noexcept {}

    LexemesContext(const LexemesContext&)                = delete;
    LexemesContext& operator=(const LexemesContext&)     = delete;
    LexemesContext(LexemesContext&&) noexcept            = default;
    LexemesContext& operator=(LexemesContext&&) noexcept = default;

    /// @brief Clears the LexemesContext
    void unsafe_clear() noexcept
    {
      lines.clear();
      str_literals.clear();
      int_literals.clear();
      float_literals.clear();
      tokens_info.clear();
      tokens.clear();
    }

    /// @brief Adds a line
    /// @param line The line to save
    void add_line(u8StringView line) noexcept { lines.push_back(line); }

    /// @brief Generates a Token
    /// @param lexeme The lexeme of the Token
    /// @param line The line number
    /// @param column_nb The column_nb of the line
    /// @param literal The literal index
    /// @return Generated Token
    void add_token(Lexeme lexeme, u32 line, u32 column_nb, u32 size) noexcept
    {
      compiler_assert_true(
          "Too many lexemes in a single source!",
          tokens_info.size() < std::numeric_limits<u32>::max());
      // Add token
      tokens.push_back(LexemeToken{lexeme, static_cast<u32>(tokens_info.size())});
      tokens_info.push_back(LexemeInfo{column_nb, size, line, line});
    }

    /// @brief Create a range from a single lexeme
    /// @param lex The lexeme
    /// @return LexemeTokenRange
    LexemeTokenRange range_from(LexemeToken lex) const noexcept
    {
      assert_true("Invalid range!", lex != Lexeme::TKN_EOF);
      return LexemeTokenRange{lex.info_index(), lex.info_index() + 1};
    }

    /// @brief Create a range from two lexemes
    /// @param start The starting lexeme
    /// @param end The ending lexeme (non-inclusive)
    /// @return LexemeTokenRange
    LexemeTokenRange range_from(LexemeToken start, LexemeToken end) const noexcept
    {
      assert_true("Invalid range!", start.info_index() <= end.info_index());
      return LexemeTokenRange{start.info_index(), end.info_index()};
    }

    void add_identifier(
        u8StringView value, Lexeme lexeme, u32 line, u32 column_nb,
        u32 size) noexcept
    {
      u64 ret = identifiers.size();
      identifiers.push_back(value);
      compiler_assert_true(
          "Too many lexemes in a single source!",
          ret <= std::numeric_limits<u32>::max());

      tokens.push_back(LexemeToken{
          lexeme, static_cast<u32>(tokens_info.size()), static_cast<u32>(ret)});
      tokens_info.push_back(LexemeInfo{column_nb, size, line, line});
    }

    void add_bool_literal(bool value, u32 line, u32 column_nb, u32 size) noexcept
    {
      u64 ret = char_literals.size();
      char_literals.push_back(value);
      compiler_assert_true(
          "Too many literals in a single source!",
          ret <= std::numeric_limits<u32>::max());

      tokens.push_back(LexemeToken{
          Lexeme::TKN_BOOL_L, static_cast<u32>(tokens_info.size()),
          static_cast<u32>(ret)});
      tokens_info.push_back(LexemeInfo{column_nb, size, line, line});
    }

    void add_char_literal(
        u32 value, u32 line, u32 column_nb, u32 size) noexcept
    {
      u64 ret = char_literals.size();
      char_literals.push_back(value);
      compiler_assert_true(
          "Too many literals in a single source!",
          ret <= std::numeric_limits<u32>::max());

      tokens.push_back(LexemeToken{
          Lexeme::TKN_CHAR_L, static_cast<u32>(tokens_info.size()),
          static_cast<u32>(ret)});
      tokens_info.push_back(LexemeInfo{column_nb, size, line, line});
    }

    void add_int_literal(
        num::BigInt&& value, u32 line, u32 column_nb, u32 size) noexcept
    {
      u64 ret = int_literals.size();
      int_literals.push_back(std::move(value));
      compiler_assert_true(
          "Too many literals in a single source!",
          ret <= std::numeric_limits<u32>::max());

      tokens.push_back(LexemeToken{
          Lexeme::TKN_INT_L, static_cast<u32>(tokens_info.size()),
          static_cast<u32>(ret)});
      tokens_info.push_back(LexemeInfo{column_nb, size, line, line});
    }

    void add_float_literal(
        num::BigRational&& value, u32 line, u32 column_nb, u32 size) noexcept
    {
      u64 ret = float_literals.size();
      float_literals.push_back(std::move(value));
      compiler_assert_true(
          "Too many literals in a single source!",
          ret <= std::numeric_limits<u32>::max());

      tokens.push_back(LexemeToken{
          Lexeme::TKN_FLOAT_L, static_cast<u32>(tokens_info.size()),
          static_cast<u32>(ret)});
      tokens_info.push_back(LexemeInfo{column_nb, size, line, line});
    }

    /// @brief Returns a StringView over the line in which the token appears
    /// @param tkn The Token whose line to return
    /// @return The line in which appears the token
    u8StringView line_str(LexemeToken tkn) const noexcept
    {
      // TODO: add overload for more lines
      return lines[tokens_info[tkn.info_index()].line_start];
    }

    /// @brief Returns the line number on which the token appears
    /// @param tkn The Token whose line to return
    /// @return The line in which appears the token (1-based)
    u32 line_nb(LexemeToken tkn) const noexcept
    {
      auto ret = tokens_info[tkn.info_index()].line_start;
      compiler_assert_true(
          "Too many lines in a single source!",
          ret != std::numeric_limits<u32>::max());
      return ret + 1;
    }

    /// @brief Returns the column_nb on which the token appears
    /// @param tkn The Token whose column_nb to return
    /// @return The column_nb to return (1-based)
    u32 column_nb(LexemeToken tkn) const noexcept
    {
      auto ret = tokens_info[tkn.info_index()].column_nb;
      compiler_assert_true(
          "Too many columns in a single line!",
          ret != std::numeric_limits<u32>::max());
      return ret + 1;
    }

    u8StringView extract_identifier(LexemeToken tkn) const noexcept
    {
      assert_true(
          "Token does not represent an identifier!",
          tkn.lexeme() == Lexeme::TKN_IDENTIFIER);
      return identifiers[tkn.literal_index()];
    }

    bool extract_bool_literal(LexemeToken tkn) const noexcept
    {
      assert_true(
          "Token does not represent an int!", tkn.lexeme() == Lexeme::TKN_BOOL_L);
      return char_literals[tkn.literal_index()];
    }

    u32 extract_char_literal(LexemeToken tkn) const noexcept
    {
      assert_true(
          "Token does not represent an int!", tkn.lexeme() == Lexeme::TKN_CHAR_L);
      return char_literals[tkn.literal_index()];
    }

    const num::BigInt& extract_int_literal(LexemeToken tkn) const noexcept
    {
      assert_true(
          "Token does not represent an int!", tkn.lexeme() == Lexeme::TKN_INT_L);
      return int_literals[tkn.literal_index()];
    }

    const num::BigRational& extract_float_literal(LexemeToken tkn) const noexcept
    {
      assert_true(
          "Token does not represent a float!", tkn.lexeme() == Lexeme::TKN_FLOAT_L);
      return float_literals[tkn.literal_index()];
    }

    /// @brief Returns the size of the Token
    /// @param tkn The Token whose size to return
    /// @return The column_nb to return (1-based)
    LexemeInfo info(LexemeToken tkn) const noexcept
    {
      return tokens_info[tkn.info_index()];
    }

    /// @brief Constructs a source information from a token range
    /// @param range The token range
    /// @return SourceInfo represented by the token range
    SourceInfo make_source_info(LexemeTokenRange range) const noexcept
    {
      auto& tkn1_info = tokens_info[range.start_index];
      auto& tkn2_info = tokens_info[range.end_index - 1];
      auto line       = u8StringView{
          lines[tkn1_info.line_start].data(),
          lines[tkn2_info.line_end].data() + lines[tkn2_info.line_end].size()};
      auto expr = u8StringView{
          lines[tkn1_info.line_start].data() + tkn1_info.column_nb,
          lines[tkn2_info.line_end].data() + tkn2_info.size + tkn2_info.column_nb};
      return SourceInfo{
          tkn1_info.line_start + 1, tkn2_info.line_end + 1, expr, line};
    }

    /// @brief Constructs a source information from a token
    /// @param tkn The token
    /// @return SourceInfo represented by the token
    SourceInfo make_source_info(LexemeToken tkn) const noexcept
    {
      auto& tkn_info = tokens_info[tkn.info_index()];
      auto expr      = u8StringView{
          lines[tkn_info.line_start].data() + tkn_info.column_nb,
          lines[tkn_info.line_end].data() + tkn_info.size + tkn_info.column_nb};
      return SourceInfo{tkn_info.line_start + 1, expr, lines[tkn_info.line_start]};
    }

    /// @brief Returns the list of tokens
    /// @return List of tokens
    auto& token_buffer() const noexcept { return tokens; }

    /// @brief Returns the list of lines
    /// @return The list of lines
    auto& line_buffer() const noexcept { return lines; }

    COLT_ENABLE_REFLECTION();
  };

} // namespace clt::lng

// Declare reflection for serialization

COLT_DECLARE_TYPE(clt::lng::LexemeToken, field);
COLT_DECLARE_TYPE(clt::lng::LexemeTokenRange, start_index, end_index);
COLT_DECLARE_TYPE(
    clt::lng::LexemesContext, int_literals, float_literals, char_literals,
    identifiers, str_literals, lines, tokens_info, tokens);

#endif // !HG_COLT_TOKEN_BUFFER
