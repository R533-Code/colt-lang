#include "lex.h"

namespace clt::lng
{
  LexemesContext lex(ErrorReporter& reporter, View<u8> to_parse) noexcept
  {
    COLT_TRACE_FN_C(clt::Color::DarkCyan);
    // The result of lexing
    LexemesContext ctx;
    // Initialize the Lexer, which will populate the lexemes context
    Lexer lex = {to_parse, reporter, ctx};
    // Parse 'to_parse'
    lex.parse();

    // Add U8_EOF (even if there is already an U8_EOF)
    if (ctx.token_buffer().is_empty())
    {
      ctx.add_token(Lexeme::TKN_EOF, 0, 0, 0);
    }
    else
    {
      auto& token = ctx.token_buffer().back();
      ctx.add_token(
          Lexeme::TKN_EOF, ctx.line_nb(token) - 1, ctx.column_nb(token) + 1, 0);
    }
    return ctx;
  }

  bool Lexer::is_valid_identifier(u8StringView strv) noexcept
  {
    COLT_TRACE_FN_C(clt::Color::DarkCyan);
    auto begin               = strv.begin();
    auto end                 = strv.end();
    char32_t chr             = *begin;
    bool is_valid_identifier = chr == U'_' || uni::is_xid_start(chr);
    ++begin;
    if (is_valid_identifier)
    {
      while (begin != end)
      {
        chr = *begin;
        if (!(chr == U'_' || uni::is_xid_continue(chr)))
          return false;
        ++begin;
      }
    }
    return is_valid_identifier;
  }

  u8 Lexer::next() noexcept
  {
    COLT_TRACE_FN_C(clt::Color::DarkCyan);
    if (_parse_offset >= to_parse.size()) [[unlikely]]
    {
      // If next is called twice after hitting EOF,
      // we don't want to increment again
      bool should_increment = to_parse.size() == _parse_offset;
      size_lexeme += (u64)should_increment;
      _parse_offset += (u64)should_increment;
      _offset += (u64)should_increment;
      return U8_EOF;
    }
    ++size_lexeme;
    u8 chr = to_parse[_parse_offset];
    // On Windows the newline will be '\r\n'
    if (chr == '\n')
    {      
      bool previous_r = false;
      if (_parse_offset != 0)
        previous_r = to_parse[_parse_offset - 1] == '\r';
      // Save the newly parsed line (without '\r\n' or '\n')
      ctx.add_line(u8StringView{
          (const Char8*)to_parse.data() + _line_offset,
          (const Char8*)to_parse.data() + _parse_offset - (u64)previous_r});
      // Update current line beginning offset to point to the
      // character AFTER the newline
      ++_parse_offset;
      _line_offset = _parse_offset;
      // reset the current line offset
      _offset = 0;
      ++_line_nb;
      return chr;
    }
    ++_offset;
    ++_parse_offset;
    return chr;
  }

  u32 Lexer::offset() const noexcept
  {
    assert_true(
        "getOffset can only be called after a call to getNext!",
        !(_offset == 0 && _line_nb == 0));
    // We use unit_len as we are working with offsets, not code points
    return _offset ? _offset - 1
                   : static_cast<u32>(ctx.lines[_line_nb - 1].unit_len());
  }

  Lexer::Snapshot Lexer::start_lexeme() noexcept
  {
    assert_true("startLexeme can only be called after a call to getNext!", _parse_offset != 0);

    size_lexeme = 0;
    return Snapshot{_line_offset, _line_nb, offset()};
  }

  u8StringView Lexer::current_identifier(const Lexer::Snapshot& snap) const noexcept
  {
    return u8StringView{
        reinterpret_cast<const Char8*>(
            to_parse.data() + snap.line_offset + snap.column_nb),
        offset() - snap.column_nb};
  }

  u8 Lexer::peek_next(u32 offset) const noexcept
  {
    if (_parse_offset + offset < to_parse.size()) [[likely]]
      return to_parse[_parse_offset];
    return U8_EOF;
  }

  SourceInfo Lexer::make_source(const Lexer::Snapshot& snap) const noexcept
  {
    COLT_TRACE_FN_C(clt::Color::DarkCyan);
    const auto to_parse_data = reinterpret_cast<const Char8*>(
        to_parse.data());
    // As the current line is most likely not set, we need to search for 
    // the end of the line.
    auto end = ptr_to<const Char8*>(std::memchr(
        to_parse_data  + snap.line_offset, '\n',
        to_parse.size() - snap.line_offset));
    if (end == nullptr)
      end = to_parse_data + to_parse.size();
    // If the line end is '\r\n' go back a character.
    else if (end != to_parse_data && end[-1] == '\r')
      --end;

    // Quite ugly code because of conversions
    return SourceInfo{
        snap.line_nb + 1,
        u8StringView{to_parse_data + snap.line_offset + snap.column_nb, size_lexeme},
        u8StringView{to_parse_data + snap.line_offset, end}};
  }

  void Lexer::add_identifier(
      u8StringView identifier, const Snapshot& snap) const noexcept
  {
    COLT_TRACE_FN_C(clt::Color::DarkCyan);
    assert_true("Invalid call to addToken", size_lexeme != 0);
    ctx.add_identifier(
        identifier, Lexeme::TKN_IDENTIFIER, snap.line_nb, snap.column_nb,
        size_lexeme);
  }

  void Lexer::add_token(Lexeme lexeme, const Snapshot& snap) const noexcept
  {
    COLT_TRACE_FN_C(clt::Color::DarkCyan);
    assert_true("Invalid call to add_token", size_lexeme != 0);
    ctx.add_token(lexeme, snap.line_nb, snap.column_nb, size_lexeme);
  }

  void Lexer::add_int(num::BigInt&& value, const Snapshot& snap) const noexcept
  {
    COLT_TRACE_FN_C(clt::Color::DarkCyan);
    //Hello 1.e Hello 1.e2
    assert_true("Invalid call to add_token", size_lexeme != 0);
    ctx.add_int_literal(std::move(value), snap.line_nb, snap.column_nb, size_lexeme);
  }

  void Lexer::add_float(num::BigRational&& value, const Snapshot& snap) const noexcept
  {
    COLT_TRACE_FN_C(clt::Color::DarkCyan);
    assert_true("Invalid call to add_token", size_lexeme != 0);
    ctx.add_float_literal(std::move(value), snap.line_nb, snap.column_nb, size_lexeme);
  }

  void Lexer::add_bool(bool value, const Snapshot& snap) const noexcept
  {
    COLT_TRACE_FN_C(clt::Color::DarkCyan);
    assert_true("Invalid call to add_token", size_lexeme != 0);
    ctx.add_bool_literal(value, snap.line_nb, snap.column_nb, size_lexeme);
  }

  void Lexer::add_char(u32 value, const Snapshot& snap) const noexcept
  {
    COLT_TRACE_FN_C(clt::Color::DarkCyan);
    assert_true("Invalid call to add_token", size_lexeme != 0);
    ctx.add_char_literal(value, snap.line_nb, snap.column_nb, size_lexeme);
  }

  void Lexer::consume_till_whitespaces(Lexer& lexer) noexcept
  {
    COLT_TRACE_FN_C(clt::Color::DarkCyan);
    // Consume till a whitespace or U8_EOF is hit
    while (!clt::isspace(lexer._next) && lexer._next != U8_EOF)
      lexer._next = lexer.next();
  }

  void Lexer::consume_till_space_or_punct(Lexer& lexer) noexcept
  {
    COLT_TRACE_FN_C(clt::Color::DarkCyan);
    // Consume till a whitespace or a punctuation or U8_EOF is hit
    while (!clt::isspace(lexer._next) && !clt::ispunct(lexer._next)
           && lexer._next != U8_EOF)
      lexer._next = lexer.next();
  }

  void Lexer::consume_whitespaces(Lexer& lexer) noexcept
  {
    COLT_TRACE_FN_C(clt::Color::DarkCyan);
    // Consume while whitespace and not U8_EOF is hit
    while (clt::isspace(lexer._next) && lexer._next != U8_EOF)
      lexer._next = lexer.next();
  }

  void Lexer::consume_lines_comment_throw(Lexer& lexer)
  {
    COLT_TRACE_FN_C(clt::Color::DarkCyan);
    const u32 line_nb = lexer._line_nb;

    if (lexer.comment_depth == std::numeric_limits<u8>::max())
    {
      lexer.reporter.error("Exceeded recursion depth while parsing /**/ comments!"_UTF8);
      lexer._next = U8_EOF; // To stop parsing
      throw ExitRecursionException{};
    }
    lexer.comment_depth++;
    assert_true(
        "Invalid call to ConsumeLinesComment!",
        lexer._offset >= Lexer::MultilineCommentSize);
    const auto start_offset = lexer._offset - 1;
    const auto line_offset  = lexer._line_offset;

    do
    {
      // We hit a nested comment
      if (lexer._next == '/' && lexer.peek_next() == '*')
      {
        lexer.next();               // consume '/'
        lexer._next = lexer.next(); // consume '*'
        consume_lines_comment_throw(lexer);
        continue;
      }
      if (lexer._next == '*' && lexer.peek_next() == '/')
      {
        lexer.next(); // consume '/'
        lexer._next = lexer.next();
        lexer.comment_depth--;
        return;
      }
      lexer._next = lexer.next();
    } while (lexer._next != U8_EOF);

    // We hit U8_EOF
    lexer.reporter.error(
        "Unterminated multi-line comment!"_UTF8,
        lexer.make_source(Snapshot{line_offset, line_nb, start_offset}));
    throw ExitRecursionException{};
  }

  void Lexer::consume_lines_comment(Lexer& lexer) noexcept
  {
    try
    {
      consume_lines_comment_throw(lexer);
    }
    catch (...)
    {
      // We catch the exception:
      // consume_lines_comment_throw will have reported the exception.
      // This allows all other functions to be noexcept.
    }
  }

  void Lexer::consume_digits(Lexer& lexer) noexcept
  {
    COLT_TRACE_FN_C(clt::Color::DarkCyan);
    // Consume while is digit and not U8_EOF is hit
    while (clt::isdigit(lexer._next) && lexer._next != U8_EOF)
    {
      lexer.temp.push_back(lexer._next);
      lexer._next = lexer.next();
    }
  }

  void Lexer::consume_digits(Lexer& lexer, int base) noexcept
  {
    COLT_TRACE_FN_C(clt::Color::DarkCyan);
    assert_true("Invalid base!", base > 1, base <= 16);
    if (base <= 10)
    {
      while (('0' <= lexer._next && lexer._next < '0' + base) && lexer._next != U8_EOF)
      {
        lexer.temp.push_back(lexer._next);
        lexer._next = lexer.next();
      }
    }
    else
    {
      const int minus_10 = base - 10;
      while ((('0' <= lexer._next && lexer._next < '0' + base)
              || ('A' <= clt::toupper(lexer._next)
                  && clt::toupper(lexer._next) < 'A' + minus_10))
             && lexer._next != U8_EOF)
      {
        lexer.temp.push_back(lexer._next);
        lexer._next = lexer.next();
      }
    }
  }

  void Lexer::consume_alnum(Lexer& lexer) noexcept
  {
    COLT_TRACE_FN_C(clt::Color::DarkCyan);
    // Consume while is alnum and not U8_EOF is hit
    while (clt::isalnum(lexer._next) && lexer._next != U8_EOF)
    {
      lexer.temp.push_back(lexer._next);
      lexer._next = lexer.next();
    }
  }

  void Lexer::parse_invalid(Lexer& lexer) noexcept
  {
    COLT_TRACE_FN_C(clt::Color::DarkCyan);
    auto snap = lexer.start_lexeme();

    lexer._next = lexer.next();
    consume_till_space_or_punct(lexer);
    lexer.add_token(Lexeme::TKN_ERROR, snap);

    // TODO: add error number
    lexer.reporter.error("Invalid character!"_UTF8, lexer.make_source(snap));
  }

  void Lexer::parse_plus(Lexer& lexer) noexcept
  {
    COLT_TRACE_FN_C(clt::Color::DarkCyan);
    auto snap = lexer.start_lexeme();

    lexer._next = lexer.next();
    switch (lexer._next)
    {
      break;
    case '=':
      lexer._next = lexer.next(); // consume '='
      lexer.add_token(Lexeme::TKN_PLUS_EQUAL, snap);
      break;
    case '+':
      lexer._next = lexer.next(); // consume '+'
      lexer.add_token(Lexeme::TKN_PLUS_PLUS, snap);
      break;
    default:
      lexer.add_token(Lexeme::TKN_PLUS, snap);
    }
  }

  void Lexer::parse_minus(Lexer& lexer) noexcept
  {
    COLT_TRACE_FN_C(clt::Color::DarkCyan);
    auto snap = lexer.start_lexeme();

    lexer._next = lexer.next();
    switch (lexer._next)
    {
      break;
    case '=':
      lexer._next = lexer.next(); // consume '='
      lexer.add_token(Lexeme::TKN_MINUS_EQUAL, snap);
      break;
    case '-':
      lexer._next = lexer.next(); // consume '-'
      lexer.add_token(Lexeme::TKN_MINUS_MINUS, snap);
      break;
    default:
      lexer.add_token(Lexeme::TKN_MINUS, snap);
    }
  }

  void Lexer::parse_star(Lexer& lexer) noexcept
  {
    COLT_TRACE_FN_C(clt::Color::DarkCyan);
    auto snap = lexer.start_lexeme();

    lexer._next = lexer.next();
    if (lexer._next == '=')
    {
      lexer._next = lexer.next(); // consume '='
      lexer.add_token(Lexeme::TKN_STAR_EQUAL, snap);
    }
    else
      lexer.add_token(Lexeme::TKN_STAR, snap);
  }

  void Lexer::parse_slash(Lexer& lexer) noexcept
  {
    COLT_TRACE_FN_C(clt::Color::DarkCyan);
    auto snap = lexer.start_lexeme();

    lexer._next = lexer.next();
    switch (lexer._next)
    {
      break;
    case '=':
      lexer._next = lexer.next(); // consume '='
      lexer.add_token(Lexeme::TKN_SLASH_EQUAL, snap);

      /****  COMMENTS HANDLING  ****/
      break;
    case '/':
      // Go to next line
      lexer._line_nb++;
      lexer._offset = 0;
      break;
    case '*':
      lexer._next = lexer.next(); // consume '*'
      consume_lines_comment(lexer);
      break;
    default:
      lexer.add_token(Lexeme::TKN_SLASH, snap);
    }
  }

  void Lexer::parse_percent(Lexer& lexer) noexcept
  {
    COLT_TRACE_FN_C(clt::Color::DarkCyan);
    auto snap = lexer.start_lexeme();

    lexer._next = lexer.next();
    if (lexer._next == '=')
    {
      lexer._next = lexer.next(); // consume '='
      lexer.add_token(Lexeme::TKN_PERCENT_EQUAL, snap);
    }
    else
      lexer.add_token(Lexeme::TKN_PERCENT, snap);
  }

  void Lexer::parse_colon(Lexer& lexer) noexcept
  {
    COLT_TRACE_FN_C(clt::Color::DarkCyan);
    auto snap = lexer.start_lexeme();

    lexer._next = lexer.next();
    if (lexer._next == ':')
    {
      lexer._next = lexer.next(); // consume ':'
      lexer.add_token(Lexeme::TKN_COLON_COLON, snap);
    }
    else
      lexer.add_token(Lexeme::TKN_COLON, snap);
  }

  void Lexer::parse_equal(Lexer& lexer) noexcept
  {
    COLT_TRACE_FN_C(clt::Color::DarkCyan);
    using enum Lexeme;
    auto snap = lexer.start_lexeme();

    lexer._next = lexer.next();
    switch (lexer._next)
    {
      break;
    case '=':
      lexer._next = lexer.next(); // consume '='
      lexer.add_token(TKN_EQUAL_EQUAL, snap);
      break;
    case '>':
      lexer._next = lexer.next(); // consume '>'
      lexer.add_token(TKN_EQUAL_GREAT, snap);
      break;
    default:
      lexer.add_token(TKN_EQUAL, snap);
    }
  }

  void Lexer::parse_exclam(Lexer& lexer) noexcept
  {
    COLT_TRACE_FN_C(clt::Color::DarkCyan);
    auto snap = lexer.start_lexeme();

    lexer._next = lexer.next();
    if (lexer._next == '=')
    {
      lexer._next = lexer.next(); // consume '='
      lexer.add_token(Lexeme::TKN_EXCLAM_EQUAL, snap);
    }
    else
      lexer.add_token(Lexeme::TKN_EXCLAM, snap);
  }

  void Lexer::parse_lt(Lexer& lexer) noexcept
  {
    COLT_TRACE_FN_C(clt::Color::DarkCyan);
    using enum Lexeme;
    auto snap = lexer.start_lexeme();

    lexer._next = lexer.next();
    switch (lexer._next)
    {
      break;
    case '=':
      lexer._next = lexer.next(); // consume '='
      lexer.add_token(TKN_LESS_EQUAL, snap);

      break;
    case '<':
      lexer._next = lexer.next(); // consume '<'
      if (lexer._next == '=')
      {
        lexer._next = lexer.next(); // consume '='
        lexer.add_token(TKN_LESS_LESS_EQUAL, snap);
      }
      else
        lexer.add_token(TKN_LESS_LESS, snap);

      break;
    default:
      lexer.add_token(TKN_LESS, snap);
    }
  }

  void Lexer::parse_gt(Lexer& lexer) noexcept
  {
    COLT_TRACE_FN_C(clt::Color::DarkCyan);
    using enum Lexeme;
    auto snap = lexer.start_lexeme();

    lexer._next = lexer.next();
    switch (lexer._next)
    {
      break;
    case '=':
      lexer._next = lexer.next(); // consume '='
      lexer.add_token(TKN_GREAT_EQUAL, snap);

      break;
    case '>':
      lexer._next = lexer.next(); // consume '>'
      if (lexer._next == '=')
      {
        lexer._next = lexer.next(); // consume '='
        lexer.add_token(TKN_GREAT_GREAT_EQUAL, snap);
      }
      else
        lexer.add_token(TKN_GREAT_GREAT, snap);

      break;
    default:
      lexer.add_token(TKN_GREAT, snap);
    }
  }

  void Lexer::parse_and(Lexer& lexer) noexcept
  {
    COLT_TRACE_FN_C(clt::Color::DarkCyan);
    using enum Lexeme;
    auto snap = lexer.start_lexeme();

    lexer._next = lexer.next();
    switch (lexer._next)
    {
      break;
    case '=':
      lexer._next = lexer.next(); // consume '='
      lexer.add_token(TKN_AND_EQUAL, snap);
      break;
    case '&':
      lexer._next = lexer.next(); // consume '&'
      lexer.add_token(TKN_AND_AND, snap);
      break;
    default:
      lexer.add_token(TKN_AND, snap);
    }
  }

  void Lexer::parse_or(Lexer& lexer) noexcept
  {
    COLT_TRACE_FN_C(clt::Color::DarkCyan);
    using enum Lexeme;
    auto snap = lexer.start_lexeme();

    lexer._next = lexer.next();
    switch (lexer._next)
    {
      break;
    case '=':
      lexer._next = lexer.next(); // consume '='
      lexer.add_token(TKN_OR_EQUAL, snap);
      break;
    case '|':
      lexer._next = lexer.next(); // consume '|'
      lexer.add_token(TKN_OR_OR, snap);
      break;
    default:
      lexer.add_token(TKN_OR, snap);
    }
  }

  void Lexer::parse_caret(Lexer& lexer) noexcept
  {
    COLT_TRACE_FN_C(clt::Color::DarkCyan);
    auto snap = lexer.start_lexeme();

    lexer._next = lexer.next();
    if (lexer._next == '=')
    {
      lexer._next = lexer.next(); // consume '='
      lexer.add_token(Lexeme::TKN_CARET_EQUAL, snap);
    }
    else
      lexer.add_token(Lexeme::TKN_CARET, snap);
  }

  void Lexer::parse_digit(Lexer& lexer) noexcept
  {
    COLT_TRACE_FN_C(clt::Color::DarkCyan);
    auto snap = lexer.start_lexeme();
    lexer.temp.clear();
    lexer.temp.push_back(lexer._next);

    if (lexer._next == '0') //Could be 0x, 0b, 0o
    {
      lexer._next = lexer.next();
      char symbol = lexer._next;
      int base    = 10;
      switch (clt::tolower(symbol))
      {
        break;
      case 'x': //HEXADECIMAL
        base = 16;
        break;
      case 'b': //BINARY
        base = 2;
        break;
      case 'o': //OCTAL
        base = 8;
        break;
      default:
        //If not any 'x', 'b' or 'o', parse normally
        if (clt::isdigit(symbol) || symbol == '.')
          goto NORM;
        else //If not digit nor '.', then simply '0'
          return parse_integral(lexer, snap);
      }
      lexer._next = lexer.next(); //Consume symbol
      //Pop the leading '0'
      lexer.temp.clear();
      consume_digits(lexer, base);

      if (lexer.temp.size() == 0) //Contains only the '0'
      {
        const Char8* range_str;
        switch_no_default(symbol)
        {
          break;
        case 'x':
          range_str = "Integral literals starting with '0x' should be followed by "
                      "characters in range [0-9] or [a-f]!"_UTF8;
          break;
        case 'b':
          range_str = "Integral literals starting with '0b' should be followed by "
                      "characters in range [0-1]!"_UTF8;
          break;
        case 'o':
          range_str = "Integral literals starting with '0o' should be followed by "
                      "characters in range [0-7]!"_UTF8;
        }
        consume_till_space_or_punct(lexer);
        lexer.reporter.error(range_str, lexer.make_source(snap));
        return lexer.add_token(Lexeme::TKN_ERROR, snap);
      }
      return parse_integral(lexer, snap, base);
    }
    lexer._next = lexer.next();
  NORM:
    //Parse as many digits as possible
    consume_digits(lexer);

    bool is_float = false;
    // [0-9]+ followed by a .[0-9] is a float
    if (lexer._next == '.')
    {
      //Snapshot for '.' character
      // TODO: fix snapshot...
      //auto snap_dot = lexer.start_lexeme();

      lexer._next = lexer.next();
      if (clt::isdigit(lexer._next))
      {
        is_float = true;
        lexer.temp.push_back('.');
        lexer.temp.push_back(lexer._next);
        lexer._next = lexer.next();

        //Parse as many digits as possible
        consume_digits(lexer);
      }
      else
      {
        //We parse the integer
        parse_integral(lexer, snap);

        //The dot is not followed by a digit, this is not a float,
        //but rather should be a dot followed by an identifier for a function call
        //lexer._next = lexer.next();
        return lexer.add_token(Lexeme::TKN_DOT, snap);
      }
    }

    // [0-9]+(.[0-9]+)?e[+-][0-9]+ is a float
    // We are possibly parsing an exponent
    if (lexer._next == 'e')
    {
      char after_e = lexer.peek_next();
      if (clt::isdigit(after_e))
      {
        is_float    = true;
        lexer._next = lexer.next(); // consume 'e'
        lexer.temp.push_back('e');
        consume_digits(lexer);
      }
      else if (after_e == '+' && clt::isdigit(lexer.peek_next(1)))
      {
        is_float = true;
        lexer.next();               // consume 'e'
        lexer._next = lexer.next(); // consume '+'
        lexer.temp.push_back('e');
        consume_digits(lexer);
      }
      else if (after_e == '-' && clt::isdigit(lexer.peek_next(1)))
      {
        is_float = true;
        lexer.next();               // consume 'e'
        lexer._next = lexer.next(); // consume '-'
        lexer.temp += "e-";
        consume_digits(lexer);
      }
    }

    if (is_float)
      parse_floating(lexer, snap);
    else
      parse_integral(lexer, snap);
  }

  /// @brief Map from keyword string to lexeme
  static constexpr auto KeywordMap = keyword_map();
 
  void Lexer::parse_identifier(Lexer& lexer) noexcept
  {
    COLT_TRACE_FN_C(clt::Color::DarkCyan);
    auto snap = lexer.start_lexeme();

    // Consume till a whitespace or U8_EOF is hit
    // > 127 for characters over the ASCII range
    while (clt::isalnum(lexer._next) || (lexer._next > 127 && lexer._next != U8_EOF) || lexer._next == '_')
      lexer._next = lexer.next();

    u8StringView identifier = lexer.current_identifier(snap);
    // Not UB as char can alias anything
    std::string_view strv_identifier = {(const char*)identifier.data(), identifier.unit_len()};
    
    // TODO: add TKN_ERROR
    if (!simdutf::validate_utf8(strv_identifier.data(), strv_identifier.size()))
      lexer.reporter.error("Invalid UTF8 identifier!"_UTF8, lexer.make_source(snap));
    else if (!una::norm::is_nfc_utf8(strv_identifier))
      lexer.reporter.error("UTF8 identifier must be normalized using NFC!"_UTF8, lexer.make_source(snap));
    else if (!is_valid_identifier(identifier))
      lexer.reporter.error(
          "This is not a valid UTF8 identifier!"_UTF8,
          lexer.make_source(snap));

    if (identifier == "true"_UTF8)
      return lexer.add_bool(true, snap);
    if (identifier == "false"_UTF8)
      return lexer.add_bool(false, snap);

    // We can just compare the actual bytes
    // (as UTF8 is backwards compatible with ASCII)
    if (auto keyword_opt = KeywordMap.find(strv_identifier); keyword_opt.is_value())      
      return lexer.add_token(*keyword_opt, snap); // This is a keyword

    if (strv_identifier.starts_with("__"))
    {
      lexer.reporter.error(
          "Identifiers starting with '__' are reserved for the compiler!"_UTF8,
          lexer.make_source(snap));
      // TODO: add identifier but increment error count
      return lexer.add_token(Lexeme::TKN_ERROR, snap);
    }
    lexer.add_identifier(identifier, snap);
  }

  void Lexer::parse_dot(Lexer& lexer) noexcept
  {
    COLT_TRACE_FN_C(clt::Color::DarkCyan);
    auto snap = lexer.start_lexeme();

    lexer._next = lexer.next();
    if (!clt::isdigit(lexer._next))
      return lexer.add_token(Lexeme::TKN_DOT, snap);

    //Clear the string
    lexer.temp.clear();
    lexer.temp.push_back('.');
    consume_digits(lexer);

    // We are possibly parsing an exponent
    if (lexer._next == 'e')
    {
      char after_e = lexer.peek_next();
      if (clt::isdigit(after_e))
      {
        lexer._next = lexer.next(); // consume 'e'
        lexer.temp.push_back('e');
        consume_digits(lexer);
      }
      else if (after_e == '+' && clt::isdigit(lexer.peek_next(1)))
      {
        lexer.next();               // consume 'e'
        lexer._next = lexer.next(); // consume '+'
        lexer.temp.push_back('e');
        consume_digits(lexer);
      }
      else if (after_e == '-' && clt::isdigit(lexer.peek_next(1)))
      {
        lexer.next();               // consume 'e'
        lexer._next = lexer.next(); // consume '-'
        lexer.temp += "e-";
        consume_digits(lexer);
      }
    }
    parse_floating(lexer, snap);
  }

  void Lexer::parse_floating(Lexer& lexer, const Lexer::Snapshot& snap) noexcept
  {
    COLT_TRACE_FN_C(clt::Color::DarkCyan);
    // TODO: actual parsing
    lexer.add_float(std::move(num::BigRational(1)), snap);

    //lexer.add_token(Lexeme::TKN_ERROR, snap);
    //lexer.reporter.error("Invalid floating point literal!", lexer.make_source(snap));
  }

  void Lexer::parse_integral(
      Lexer& lexer, const Lexer::Snapshot& snap, int base) noexcept
  {
    COLT_TRACE_FN_C(clt::Color::DarkCyan);
    using namespace num;
    auto value = BigInt::from(lexer.temp.c_str(), base);

    if (value.is_value())
      return lexer.add_int(std::move(*value), snap);

    lexer.add_token(Lexeme::TKN_ERROR, snap);
    lexer.reporter.error("Invalid integer literal!"_UTF8, lexer.make_source(snap));
  }

  void print_token(LexemeToken tkn, const LexemesContext& buffer) noexcept
  {
    COLT_TRACE_FN_C(clt::Color::DarkCyan);
    using enum Lexeme;

    if (is_literal(tkn) && tkn != TKN_STRING_L)
    {
      switch_no_default(tkn.lexeme())
      {
      case TKN_BOOL_L:
        return io::print("{:h} {}", tkn.lexeme(), buffer.extract_bool_literal(tkn));
      case TKN_CHAR_L:
        return io::print("{:h} U+{:04x}", tkn.lexeme(), buffer.extract_char_literal(tkn));
      case TKN_FLOAT_L:
        return io::print("{:h} {}", tkn.lexeme(), buffer.extract_float_literal(tkn));
      case TKN_INT_L:
        return io::print("{:h} {}", tkn.lexeme(), buffer.extract_int_literal(tkn));
      }
    }
    else if (tkn == TKN_IDENTIFIER)
      return io::print("{:h} {}", tkn.lexeme(), buffer.extract_identifier(tkn));
    return io::print("{:h}", tkn.lexeme());
  }
} // namespace clt::lng