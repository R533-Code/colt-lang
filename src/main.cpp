#include <colt_pch.h>
#include <util/args.h>
#include <frontend/lex/lex.h>
#include <frontend/err/composable_reporter.h>

using namespace clt;

/// @brief This is the main entry point of the compiler.
/// The true main function is defined by `true_main.cpp`, which
/// converts command line arguments to UTF8 and sets up
/// console support for Unicode on Windows.
/// @param argv The arguments value
/// @return The exit code
int colt_main(Span<const char8_t*> argv)
{
  COLT_TRACE_FN_C(clt::Color::Crimson);
  COLT_TRACE_EXPR(cl::parse_command_line_options<CMDs>(
      argv, COLTC_EXECUTABLE_NAME, "The Colt compiler."));
  auto val = COLT_TRACE_EXPR(ViewOfFile::open("test.txt"));
  if (val.is_value())
  {
    print_message("Opened 'test.txt'!");
    auto reporter = lng::make_error_reporter<lng::ConsoleReporter>();
    auto value    = lng::lex(*reporter, *val->view());
    COLT_TRACE_BLOCK_C("print_token", clt::Color::Chartreuse3)
    {
      for (auto& i : value.token_buffer())
        lng::print_token(i, value);
    };
  }
  else
    print_warn("Could not open 'test.txt'!");

  print_warn("REPL is not implemented.");
  return 0;
}