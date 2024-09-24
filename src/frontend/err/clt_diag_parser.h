/*****************************************************************//**
 * @file   clt_diag_parser.h
 * @brief  Contains a parser for '.colt.diag' files.
 * To allow the diagnostics to be changed without recompilation,
 * and to support multiple languages, all diagnostics are stored
 * in '.colt.diag' files.
 * 
 * The actual format is quite simple:
 * COLTCDIAG:{ISO 639-1}:{Language}
 * ([MWE][0-9]{ENTRY_ID_DIGITS}:{DIAGNOSTIC}\n)*
 * 
 * For an example, see `resources/diag/en.clt.diag`.
 * 
 * @author RPC
 * @date   September 2024
 *********************************************************************/
#ifndef HG_COLTC_CLT_DIAG_PARSER
#define HG_COLTC_CLT_DIAG_PARSER

#include <frontend/err/error_reporter.h>
#include <colt/os/mmap_file.h>

namespace clt::lng
{
  /// @brief Represents a diagnostic entry in a '.colt.diag' file
  struct DiagnosticEntry
  {
    /// @brief The number of digits representing the ID
    static constexpr u8 ENTRY_ID_DIGITS = 4;

    /// @brief The entry kind ([M]essage, [W]arning, [E]rror)
    ReportKind entry_kind;
    /// @brief The number of arguments expected by the string
    u8 arg_count;
    /// @brief The entry ID [0, 10**4)
    u16 entry_id;
    /// @brief The actual diagnostic
    u8StringView diagnostic;
  };

  /// @brief Represents a parsed '.colt.diag' file
  class DiagnosticDatabase
  {
    u8StringView lang_abrv;
    u8StringView language;
  };
}

#endif // !HG_COLTC_CLT_DIAG_PARSER
