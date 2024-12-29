#include "plugin_loader.h"

namespace clt::ffi
{
  void print_plugins() noexcept
  {
    std::error_code err;
    size_t count = 0;
    size_t valid = 0;
    print("\n======================= Plugins =======================");
    for (auto& i : std::filesystem::directory_iterator{"plugins", err})
    {
      if (std::filesystem::is_regular_file(i, err))
      {
        auto plugin = ffi::ColtPlugin::open(i.path().generic_string().c_str());
        count++;
        if (plugin.is_error())
        {
          print(
              "{}Could not greet '{}{}{}'!{}", io::BrightYellowF, io::BrightGreenF,
              i.path().filename().generic_string(), io::BrightYellowF, io::Reset);
          continue;
        }
        print(
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
      print_warn("'plugins' directory not found!");
    else if (count == 0)
      print_message("No plugins to greet!");
    else if (count != valid)
      print_warn(
          "Only greeted {}/{} plugin{}", valid, count, count == 1 ? "!" : "s!");
    else
      print_message("Greeted {}/{} plugin{}", valid, count, count == 1 ? "!" : "s!");
    print("=======================================================\n");
  }

} // namespace clt::ffi