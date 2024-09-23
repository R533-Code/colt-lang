#include <colt_pch.h>
#include <util/args.h>

using namespace clt;

/// @brief This is the main entry point of the compiler.
/// The true main function is defined by colt-cpp, which
/// converts command line arguments to UTF8 and sets up
/// console support for Unicode on Windows.
/// @param argv The arguments value
/// @return The exit code
int colt_main(Span<const char8_t*> argv)
{
  cl::parse_command_line_options<CMDs>(
      argv, COLTC_EXECUTABLE_NAME, "The Colt compiler.");
  io::print_warn("REPL is not implemented.");
  return 0;
}