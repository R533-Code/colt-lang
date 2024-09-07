#include <colt_pch.h>
#include <colt_config.h>
#include <fmt/ranges.h>
#include <colt/os/dynamic_lib.h>

struct ColoredVersion
{
  clt::io::ANSIEffect color;
  const char* name;
  clt::vers::Version version;
};

template<>
struct fmt::formatter<ColoredVersion>
{
  template<typename ParseContext>
  constexpr auto parse(ParseContext& ctx)
  {
    // TODO: add format option
    return ctx.begin();
  }

  template<typename FormatContext>
  auto format(const ColoredVersion& version, FormatContext& ctx) const
  {
    return fmt::format_to(
        ctx.out(), "{}{:<8}{} : v{}", version.color, version.name, clt::io::Reset, version.version);
  }
};


extern "C" CLT_EXPORT u32 colt_test_dl()
{
  return 32;
}

int main(int argc, const char** argv)
{
  using namespace clt;
  using namespace clt::io;
  
  static std::array Versions = {
      ColoredVersion{BrightGreenF, "coltcpp", vers::ColtCppVersion},
      ColoredVersion{BrightYellowF, "fmt", vers::FmtVersion},
      ColoredVersion{BrightYellowF, "scn", vers::ScnVersion},
      ColoredVersion{BrightYellowF, "simdutf", vers::SimdUtfVersion},
      ColoredVersion{BrightYellowF, "unialgo", vers::UniAlgoVersion},
      ColoredVersion{BrightYellowF, "unicode", vers::UnicodeVersion},
  };


  io::print("{}coltc{} (v{} {}) on {}{}{} ({}{}{})\nusing: {}",
      BrightCyanF, Reset, vers::ColtcVersion, COLT_CONFIG_STRING, BrightBlueF, COLT_OS_STRING, Reset,
      BrightMagentaF, COLT_ARCH_STRING, Reset, fmt::join(Versions, "\n       "));

  auto err = os::DynamicLib::open();
  if (err.is_none())
  {
    io::print_fatal("Could not introspect!");
    return 1;
  }
  auto ptr = err->find<u32(*)()>("colt_test_dl");
  io::print("Value: {}", (*ptr)());
}