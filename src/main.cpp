#include <colt_pch.h>
#include <colt_config.h>

int main(int argc, const char** argv)
{
  using namespace clt::io;

  print(
      "{}coltc{} (v{}.{}.{}.{} {}) on {}{}{} ({}{}{})\n"
      "using: {}colt-cpp{} (v{}.{}.{}.{}), {}unicode{} (v{}.{}.{}), "
      "{}simdutf{} (v{}), {}fmt{} (v{}.{}.{})",
      BrightCyanF, Reset, COLTC_VERSION_MAJOR, COLTC_VERSION_MINOR,
      COLTC_VERSION_PATCH, COLTC_VERSION_TWEAK, COLT_CONFIG_STRING, BrightBlueF,
      COLT_OS_STRING, Reset, BrightMagentaF, COLT_ARCH_STRING, Reset, BrightGreenF,
      Reset, COLT_VERSION_MAJOR, COLT_VERSION_MINOR, COLT_VERSION_PATCH,
      COLT_VERSION_TWEAK, BrightYellowF, Reset, COLT_UNICODE_VERSION_MAJOR,
      COLT_UNICODE_VERSION_MINOR, COLT_UNICODE_VERSION_UPDATE, BrightYellowF, Reset,
      SIMDUTF_VERSION, BrightYellowF, Reset, FMT_VERSION / 10'000,
      (FMT_VERSION % 10'000) / 100, FMT_VERSION % 100);
}