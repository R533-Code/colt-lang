/*****************************************************************/ /**
 * @file   io_reporter.h
 * @brief  Helpers for error reporting from the compiler.
 *
 * @author RPC
 * @date   January 2024
 *********************************************************************/
#ifndef HG_COLT_IO_REPORTER
#define HG_COLT_IO_REPORTER

#include "colt/dsa/string_view.h"

namespace clt::lng
{
  /// @brief ID of error/warning/message
  using ReportNumber = u32;

  /// @brief The source code information of an expression.
  struct SourceInfo
  {
    /// @brief The beginning line number of the expression (1-based)
    u32 line_begin;
    /// @brief The end line number of the expression (1-based)
    u32 line_end;
    /// @brief u8StringView over all the lines on which the expression spans
    u8StringView lines;
    /// @brief u8StringView over the expression (included in lines)
    u8StringView expr;

    SourceInfo()                                                = delete;
    constexpr SourceInfo(const SourceInfo&) noexcept            = default;
    constexpr SourceInfo(SourceInfo&&) noexcept                 = default;
    constexpr SourceInfo& operator=(SourceInfo&&) noexcept      = default;
    constexpr SourceInfo& operator=(const SourceInfo&) noexcept = default;
    constexpr SourceInfo(u32 line, u8StringView expr, u8StringView line_str) noexcept
        : line_begin(line)
        , line_end(line)
        , lines(line_str)
        , expr(expr)
    {
    }
    constexpr SourceInfo(
        u32 line_s, u32 line_e, u8StringView expr, u8StringView line_str) noexcept
        : line_begin(line_s)
        , line_end(line_e)
        , lines(line_str)
        , expr(expr)
    {
    }

    /// @brief Concatenates two SourceInfo
    /// @param rhs The right hand side
    constexpr void concat(const SourceInfo& rhs) noexcept
    {
      line_end = rhs.line_end;
      lines    = u8StringView{lines.data(), rhs.lines.unit_len()};
      expr     = u8StringView{expr.data(), rhs.expr.unit_len()};
    }

    /// @brief Check if the information represents a single line
    /// @return True if line_begin == line_end
    constexpr bool is_single_line() const noexcept { return line_begin == line_end; }
  };

  /// @brief Function pointer type of generate_* functions
  using report_print_t = void (*)(
      u8StringView, const Option<SourceInfo>&, const Option<ReportNumber>&) noexcept;

  /// @brief Prints a message to the console, highlighting code
  /// @param str The message
  /// @param src_info The message information (or None)
  /// @param nb The message number (or None)
  void generate_message(
      u8StringView str, const Option<SourceInfo>& src_info,
      const Option<ReportNumber>& nb) noexcept;
  /// @brief Prints a warning to the console, highlighting code
  /// @param str The warning
  /// @param src_info The warning information (or None)
  /// @param nb The warning number (or None)
  void generate_warn(
      u8StringView str, const Option<SourceInfo>& src_info,
      const Option<ReportNumber>& nb) noexcept;
  /// @brief Prints an error to the console, highlighting code
  /// @param str The error
  /// @param src_info The error information (or None)
  /// @param nb The error number (or None)
  void generate_error(
      u8StringView str, const Option<SourceInfo>& src_info,
      const Option<ReportNumber>& nb) noexcept;
} // namespace clt::lng

#endif // !HG_COLT_IO_REPORTER
