#include <colt_pch.h>
#include <util/args.h>

using namespace clt;

int main(int argc, const char** argv)
{
  cl::parse_command_line_options<CMDs>(
      argc, argv, COLTC_EXECUTABLE_NAME, "The Colt compiler.");
}