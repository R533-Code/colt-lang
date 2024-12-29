#ifndef HG_COLTC_ARGS
#define HG_COLTC_ARGS

#include <fmt/ranges.h>
#include <colt/io/print.h>
#include <colt/versions.h>
#include <colt_config.h>
#include <colt/io/args_parsing.h>
#include <colt/meta/traits.h>
#include <util/ffi/plugin_loader.h>
#include <util/tracing.h>
#include <lua.h>

namespace clt::vers
{
  static constexpr Version LuaVersion = {
      parse_major(LUA_VERSION_MAJOR), parse_major(LUA_VERSION_MINOR),
      parse_major(LUA_VERSION_RELEASE)};
}

namespace clt::cl
{
  struct ColoredVersion
  {
    clt::io::ANSIEffect color;
    const char* name;
    clt::vers::Version version;
  };

  static constexpr std::array Versions = {
      ColoredVersion{io::BrightGreenF, "coltcpp", vers::ColtCppVersion},
      ColoredVersion{io::BrightYellowF, "fmt", vers::FmtVersion},
      ColoredVersion{io::BrightYellowF, "scn", vers::ScnVersion},
      ColoredVersion{io::BrightYellowF, "lua", vers::LuaVersion},
      ColoredVersion{io::BrightYellowF, "simdutf", vers::SimdUtfVersion},
      ColoredVersion{io::BrightYellowF, "unialgo", vers::UniAlgoVersion},
      ColoredVersion{io::BrightYellowF, "unicode", vers::UnicodeVersion},
  };
} // namespace clt::cl

template<>
struct fmt::formatter<clt::cl::ColoredVersion>
{
  template<typename ParseContext>
  constexpr auto parse(ParseContext& ctx)
  {
    // TODO: add format option
    return ctx.begin();
  }

  template<typename FormatContext>
  auto format(const clt::cl::ColoredVersion& version, FormatContext& ctx) const
  {
    return fmt::format_to(
        ctx.out(), "{}{:<8}{} : v{}", version.color, version.name, clt::io::Reset,
        version.version);
  }
};

namespace clt
{
  /// @brief Flag to check if the application must wait for input before exiting.
  inline bool WaitForUserInput = true;
  /// @brief The output file name
  inline std::string_view OutputFile = {};
  /// @brief The input file name
  inline std::string_view InputFile = {};

  /// @brief Prints the current version of Colt and exits
  [[noreturn]] inline void print_version() noexcept
  {
    using namespace io;
    clt::print(
        "{}coltc{} (v{} {}) on {}{}{} ({}{}{})\nusing: {}", BrightCyanF, Reset,
        vers::ColtcVersion, COLT_CONFIG_STRING, BrightBlueF, COLT_OS_STRING, Reset,
        BrightMagentaF, COLT_ARCH_STRING, Reset,
        fmt::join(cl::Versions, "\n       "));
    std::exit(0);
  }

  /// @brief The meta type used to generated command line argument handling function
  using CMDs = clt::meta::type_list<

      // --version or -v
      cl::Opt<
          "-version", cl::alias<"v">, cl::desc<"Prints the version of the compiler">,
          cl::callback<&print_version>>,

      // -o <output>
      cl::Opt<"o", cl::desc<"Output file name">, cl::location<OutputFile>>,
      // <input_file>
      cl::OptPos<"input_file", cl::desc<"The input file">, cl::location<InputFile>>,

      ///////////////////////////////////////////

      cl::Group<"Compiler Behavior", io::BrightBlueF>,

      // --nocolor or -C
      cl::Opt<
          "-nocolor", cl::desc<"Turns off colored output">, cl::alias<"C">,
          cl::callback<[] { clt::io::OutputColor = false; }>>,
      // --nowait
      cl::Opt<"-nowait", cl::desc<"Do not wait for user input">, cl::callback<[] {
                clt::WaitForUserInput = false;
              }>>,

      ///////////////////////////////////////////

      ///////////////////////////////////////////
      cl::Group<"Compiler Plugins", io::BrightBlueF>,

      // --enum-plugins or -ep
      cl::Opt<
          "-enum-plugins", cl::alias<"ep">,
          cl::desc<"Enumerates the compiler's plugins">,
          cl::callback<&print_plugins>>,

      ///////////////////////////////////////////

      cl::Group<"Compiler Tracing", io::BrightBlueF>,

      cl::Opt<
          "-wait-for-tracy",
          cl::desc<"Waits for the Tracy profiler to be connected">,
          cl::callback<[]() { /*handled in `true_main.cpp`*/ }>>,
      cl::Opt<
          "-enable-tracing", cl::desc<"Enables tracing of the compiler.">,
          cl::callback<[]() { /*handled in `true_main.cpp`*/ }>>

      ///////////////////////////////////////////

      >;
} // namespace clt

#endif // !HG_COLTC_ARGS