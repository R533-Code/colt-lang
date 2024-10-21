#ifndef HG_COLTC_LEX
#define HG_COLTC_LEX

#include <frontend/err/error_reporter.h>
#include <frontend/lex/lexemes_context.h>

namespace clt::lng
{
  /// @brief Lexes 'to_parse'.
  /// 'to_parse' is a byte view as the lexer has to handle invalid unicode.
  /// @param reporter The reporter used to generate error/warnings/messages
  /// @param to_parse The bytes to parse
  /// @return A TokenBuffer containing parsed lexemes
  LexemesContext lex(ErrorReporter& reporter, View<u8> to_parse) noexcept;  

  /// @brief Prints a token (used for debugging purposes)
  /// @param tkn The token to print
  /// @param buffer The lexemes context (owns 'tkn')
  void print_token(LexemeToken tkn, const LexemesContext& buffer) noexcept;

  class Lexer
  {
    /// @brief The bytes to parse
    View<u8> to_parse;
    /// @brief The error reporter
    ErrorReporter& reporter;
    /// @brief LexemesContext where to save the lexemes
    LexemesContext& ctx;
    /// @brief Temporary string used for literals
    std::string temp = {};
    /// @brief The offset into 'to_parse'
    u64 _parse_offset = 0;
    /// @brief The offset into 'to_parse' that points to the beginning
    ///        of the current line.
    u64 _line_offset = 0;
    /// @brief The current line
    u32 _line_nb = 0;
    /// @brief The current offset into the current line
    u32 _offset = 0;
    /// @brief The size of the current lexeme
    u32 size_lexeme = 0;
    /// @brief Recursion depth for parsing comments
    u8 comment_depth = 0;
    /// @brief Next character to parse
    u8 _next = '\0';

    static constexpr u8 U8_EOF = 255;

  public:
    /// @brief The type of the lexing functions callbacks
    using LexerDispatch_t = void (*)(Lexer&) noexcept;

    /// @brief Table containing lexing functions used for dispatch
    struct DispatchTable : public std::array<LexerDispatch_t, 256>
    {
    };

    /// @brief Constructor
    /// @param to_parse The bytes to parse
    /// @param reporter The error reporter to use
    /// @param ctx The context in which to store the lexemes
    Lexer(View<u8> to_parse, ErrorReporter& reporter, LexemesContext& ctx) noexcept
      : to_parse(to_parse)
      , reporter(reporter)
      , ctx(ctx)
    {
    }

    /// @brief Parses all lexemes and populates the context
    void parse() noexcept
    {
      _next = next();
      while (_next != U8_EOF)
        Lexer::LexingTable[_next](*this);
    }

  private:
    /// @brief Check if a valid UTF8 string is a valid identifier
    /// @param strv The string view to check against
    /// @return True if the identifier conforms to the allowed Colt identifiers
    static bool is_valid_identifier(u8StringView strv) noexcept;

    /// @brief Represents the beginning of a lexeme's location
    struct Snapshot
    {
      /// @brief The byte offset to the beginning of the current line
      u64 line_offset;
      /// @brief The line number (0-based)
      u32 line_nb;
      /// @brief The column number in the line represented by 'line_offset'
      u32 column_nb;
    };

    /**
	  * Functions starting with 'Consume' do not add any Token
	  * to the lexemes context.
	  * Functions starting with 'Parse' add a Token to the
	  * lexemes context.
	  */

    /// @brief Returns the next character to parse or EOF.
    /// This method also resets the offset when a newline is hit.
    /// This method does NOT always return an ASCII char:
    /// this can be part of a UTF8 sequence. The sequences generated
    /// is not guaranteed to be a valid UTF8.
    /// @return The next character or EOF
    u8 next() noexcept;

    /// @brief Returns the current offset.
    /// The offset represents the column of the current character.
    /// @return The current offset.
    u32 offset() const noexcept;

    /// @brief Starts a new lexeme.
    /// A Snapshot represents the line and column of the lexeme start.
    /// @return Informations about the start of the lexeme
    Snapshot start_lexeme() noexcept;

    /// @brief Returns the current identifier from a snapshot
    /// @param snap The snapshot from which to extract the identifier
    /// @return The current identifier (starting with 'snap')
    u8StringView current_identifier(const Snapshot& snap) const noexcept;

    /// @brief Look ahead in the string to scan
    /// @param offset The offset to add (0 being look ahead 1 character)
    /// @return The character 'offset + 1' after the current one
    u8 peek_next(u32 offset = 0) const noexcept;

    /// @brief Creates a SourceInfo over a single-line lexeme
    /// @param start The start offset of the lexeme
    /// @return SourceInfo over the lexeme
    SourceInfo make_source(const Snapshot& snap) const noexcept;

    /// @brief Saves a Token in the TokenBuffer
    /// @param lexeme The lexeme of the Token
    /// @param column_nb The starting column_nb of the Token
    void add_token(Lexeme lexeme, const Snapshot& snap) const noexcept;

    /// @brief Saves an identifier
    /// @param identifier The identifier (which is always valid UTF8)
    /// @param snap The snapshot representing the beginning of the identifier
    void add_identifier(u8StringView identifier, const Snapshot& snap) const noexcept;

    /// @brief Saves a literal integer
    /// @param value The literal integer
    /// @param snap The snapshot representing the beginning of the literal
    void add_int(num::BigInt&& value, const Snapshot& snap) const noexcept;

    /// @brief Saves a literal float
    /// @param value The literal float
    /// @param snap The snapshot representing the beginning of the literal
    void add_float(num::BigRational&& value, const Snapshot& snap) const noexcept;

    /// @brief Saves a literal bool
    /// @param value The literal boolean
    /// @param snap The snapshot representing the beginning of the literal
    void add_bool(bool value, const Snapshot& snap) const noexcept;

    /// @brief Saves a literal character
    /// @param value The literal character
    /// @param snap The snapshot representing the beginning of the literal
    void add_char(u32 value, const Snapshot& snap) const noexcept;
    
    /// @brief Consumes all characters till a whitespace is hit
    /// @param lexer The lexer used for parsing
    static void consume_till_whitespaces(Lexer& lexer) noexcept;

    /// @brief Consumes all characters till a whitespace or a punctuation is hit
    /// @param lexer The lexer used for parsing
    static void consume_till_space_or_punct(Lexer& lexer) noexcept;

    /// @brief Consumes all whitespaces till a non-whitespace is hit
    /// @param lexer The lexer used for parsing
    static void consume_whitespaces(Lexer& lexer) noexcept;

    /// @brief Consumes all multi-line comments (recursive)
    /// @param lexer The lexer used for parsing
    /// @pre The '/*' of the comment must be consumed
    static void consume_lines_comment(Lexer& lexer) noexcept;

    /// @brief Consumes multi-line comments recursively.
    /// This function can throw an ExitRecursionExcept.
    /// @param lexer The lexer used to parse
    static void consume_lines_comment_throw(Lexer& lexer);

    /// @brief Consumes all the digits (saving them in `lexer.temp`).
    /// This function does not clear `temp`.
    /// @param lexer The lexer used for parsing
    static void consume_digits(Lexer& lexer) noexcept;

    /// @brief Consumes all the digits (with base 'base'), saving them in `lexer.temp`.
    /// This function does not clear `temp`.
    /// @param lexer The lexer used for parsing
    static void consume_digits(Lexer& lexer, int base) noexcept;

    /// @brief Consumes all the alpha-numeric characters (saving them in `lexer.temp`).
    /// This function does not clear `temp`.
    /// @param lexer The lexer used for parsing
    static void consume_alnum(Lexer& lexer) noexcept;

    /// @brief Parses an invalid character (consuming till a whitespace is hit)
    /// @param lexer The lexer used for parsing
    static void parse_invalid(Lexer& lexer) noexcept;

    /// @brief Parses a '+'
    /// @param lexer The lexer used for parsing
    static void parse_plus(Lexer& lexer) noexcept;

    /// @brief Parses a '-'
    /// @param lexer The lexer used for parsing
    static void parse_minus(Lexer& lexer) noexcept;

    /// @brief Parses a '*'
    /// @param lexer The lexer used for parsing
    static void parse_star(Lexer& lexer) noexcept;

    /// @brief Parses a '/'
    /// @param lexer The lexer used for parsing
    static void parse_slash(Lexer& lexer) noexcept;

    /// @brief Parses a '%'
    /// @param lexer The lexer used for parsing
    static void parse_percent(Lexer& lexer) noexcept;

    /// @brief Parses a ':'
    /// @param lexer The lexer used for parsing
    static void parse_colon(Lexer& lexer) noexcept;

    /// @brief Parses a '='
    /// @param lexer The lexer used for parsing
    static void parse_equal(Lexer& lexer) noexcept;

    /// @brief Parses a '!'
    /// @param lexer The lexer used for parsing
    static void parse_exclam(Lexer& lexer) noexcept;

    /// @brief Parses a '<'
    /// @param lexer The lexer used for parsing
    static void parse_lt(Lexer& lexer) noexcept;

    /// @brief Parses a '>'
    /// @param lexer The lexer used for parsing
    static void parse_gt(Lexer& lexer) noexcept;

    /// @brief Parses a '&'
    /// @param lexer The lexer used for parsing
    static void parse_and(Lexer& lexer) noexcept;

    /// @brief Parses a '|'
    /// @param lexer The lexer used for parsing
    static void parse_or(Lexer& lexer) noexcept;

    /// @brief Parses a '^'
    /// @param lexer The lexer used for parsing
    static void parse_caret(Lexer& lexer) noexcept;

    /// @brief Parses digits (floats or integrals)
    /// @param lexer The lexer used for parsing
    static void parse_digit(Lexer& lexer) noexcept;

    /// @brief Parses an identifier
    /// @param lexer The lexer used for parsing
    static void parse_identifier(Lexer& lexer) noexcept;

    template<Lexeme lexeme>
    /// @brief Parses a single character, adding 'lexeme' to the lexemes context.
    /// @tparam lexeme The lexeme to add to the lexemes context
    /// @param lexer The lexer used for parsing
    static void parse_single(Lexer& lexer) noexcept;

    /// @brief Parses a '\.'
    /// @param lexer The lexer used for parsing
    static void parse_dot(Lexer& lexer) noexcept;

    /// @brief Converts the floating point in 'lexer.temp' and reports errors.
    /// @param lexer The lexer used for parsing
    /// @param snap The source code informations of the integer
    static void parse_floating(Lexer& lexer, const Lexer::Snapshot& snap) noexcept;

    /// @brief Converts the integer in 'lexer.temp' and reports errors.
    /// @param lexer The lexer used for parsing
    /// @param snap The source code informations of the integer
    static void parse_integral(
        Lexer& lexer, const Lexer::Snapshot& snap, int base = 10) noexcept;


    /// @brief Size of the beginning of a multiline comment (SLASH STAR)
    static constexpr u32 MultilineCommentSize = 2;
    /// @brief The lexing table used to dispatch.
    /// It should be indexed by the next character to parse.
    static constexpr auto LexingTable = []() consteval {
        using enum Lexeme;
        
        DispatchTable table{};
        for (size_t i = 0; i < table.size(); i++)
        {
          // ASCII space
          if (clt::isspace((char)i))
            table[i] = &Lexer::consume_whitespaces;
          // ASCII digit
          else if (clt::isdigit((char)i))
            table[i] = &Lexer::parse_digit;
          // ASCII alpha
          else if (clt::isalpha((char)i))
            table[i] = &Lexer::parse_identifier;
          // UTF8 continuation (which is invalid)
          else if ((i & 0b11'10'00'00) == 0b11'00'00'00)
            table[i] = &Lexer::parse_invalid;
          // Probably UTF8 multibyte character
          else
            table[i] = &Lexer::parse_identifier; // Probably UTF8 identifier
        }

        table['_'] = &Lexer::parse_identifier;
        table['+'] = &Lexer::parse_plus;
        table['-'] = &Lexer::parse_minus;
        table['*'] = &Lexer::parse_star;
        table['/'] = &Lexer::parse_slash;
        table['%'] = &Lexer::parse_percent;
        table[':'] = &Lexer::parse_colon;
        table['='] = &Lexer::parse_equal;
        table['!'] = &Lexer::parse_exclam;
        table['.'] = &Lexer::parse_dot;
        table['<'] = &Lexer::parse_lt;
        table['>'] = &Lexer::parse_gt;
        table['&'] = &Lexer::parse_and;
        table['|'] = &Lexer::parse_or;
        table['^'] = &Lexer::parse_caret;
        table['~'] = &Lexer::parse_single<TKN_TILDE>;
        table[';'] = &Lexer::parse_single<TKN_SEMICOLON>;
        table[','] = &Lexer::parse_single<TKN_COLON>;
        table['{'] = &Lexer::parse_single<TKN_LEFT_CURLY>;
        table['}'] = &Lexer::parse_single<TKN_RIGHT_CURLY>;
        table['('] = &Lexer::parse_single<TKN_LEFT_PAREN>;
        table[')'] = &Lexer::parse_single<TKN_RIGHT_PAREN>;
        table['['] = &Lexer::parse_single<TKN_LEFT_SQUARE>;
        table[']'] = &Lexer::parse_single<TKN_RIGHT_SQUARE>;

        return table;
      }();
  };

  /**
	* Template function implementations
	*/

  template<Lexeme lexeme>
  void Lexer::parse_single(Lexer& lexer) noexcept
  {
    auto snap   = lexer.start_lexeme();
    lexer._next = lexer.next();
    lexer.add_token(lexeme, snap);
  }
}

#endif // !HG_COLTC_LEX
