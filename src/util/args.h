#ifndef HG_COLTC_ARGS
#define HG_COLTC_ARGS

#include <fmt/ranges.h>
#include <colt/io/print.h>
#include <colt/versions.h>
#include <colt_config.h>
#include <colt/io/args_parsing.h>
#include <colt/meta/traits.h>
#include <util/ffi/plugin_loader.h>
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

  namespace details
  {
    /// @brief Prints the current version of Colt and exits
    [[noreturn]] inline void print_version() noexcept
    {
      using namespace io;
      io::print(
          "{}coltc{} (v{} {}) on {}{}{} ({}{}{})\nusing: {}", BrightCyanF, Reset,
          vers::ColtcVersion, COLT_CONFIG_STRING, BrightBlueF, COLT_OS_STRING, Reset,
          BrightMagentaF, COLT_ARCH_STRING, Reset,
          fmt::join(cl::Versions, "\n       "));
      std::exit(0);
    }

    inline void print_plugins() noexcept
    {
      std::error_code err;
      size_t count = 0;
      size_t valid = 0;
      io::print("\n======================= Plugins =======================");
      for (auto& i : std::filesystem::directory_iterator{"plugins", err})
      {
        if (std::filesystem::is_regular_file(i, err))
        {
          auto plugin = ffi::ColtPlugin::open(i.path().generic_string().c_str());
          count++;
          if (plugin.is_error())
          {
            io::print(
                "{}Could not greet '{}{}{}'!{}", io::BrightYellowF, io::BrightGreenF,
                i.path().filename().generic_string(), io::BrightYellowF, io::Reset);
            continue;
          }
          io::print(
              "Hello '{}{}{}'! ({}{}{}, setup code {}, {:h})", io::BrightCyanF,
              plugin->name(), io::Reset, io::BrightGreenF,
              i.path().filename().generic_string(), io::Reset, plugin->run_setup(),
              plugin->advertised_purpose());
          valid++;
        }
      }
      if (count != 0)
        std::fputc('\n', stdout);
      if (err.default_error_condition().value() == ENOENT)
        io::print_warn("'plugins' directory not found!");
      else if (count == 0)
        io::print_message("No plugins to greet!");
      else if (count != valid)
        io::print_warn(
            "Only greeted {}/{} plugin{}", valid, count, count == 1 ? "!" : "s!");
      else
        io::print_message(
            "Greeted {}/{} plugin{}", valid, count, count == 1 ? "!" : "s!");
      io::print("=======================================================\n");
    }
  } // namespace details

  /// @brief The meta type used to generated command line argument handling function
  using CMDs = clt::meta::type_list<
      // --nocolor or -C
      cl::Opt<
          "-nocolor", cl::desc<"Turns off colored output">, cl::alias<"C">,
          cl::callback<[] { clt::io::OutputColor = false; }>>,
      // --nowait
      cl::Opt<"-nowait", cl::desc<"Do not wait for user input">, cl::callback<[] {
                clt::WaitForUserInput = false;
              }>>,

      // --version or -v
      cl::Opt<
          "-version", cl::alias<"v">, cl::desc<"Prints the version of the compiler">,
          cl::callback<&details::print_version>>,
      // --enum-plugins or -eP
      cl::Opt<
          "-enum-plugins", cl::alias<"ep">,
          cl::desc<"Enumerates the compiler's plugins">,
          cl::callback<&details::print_plugins>>,

      // -o <output>
      cl::Opt<"o", cl::desc<"Output file name">, cl::location<OutputFile>>,
      // <input_file>
      cl::OptPos<"input_file", cl::desc<"The input file">, cl::location<InputFile>>>;
} // namespace clt

#endif // !HG_COLTC_ARGS