#ifndef HG_COLTC_PLUGIN_LOADER
#define HG_COLTC_PLUGIN_LOADER

#include <colt/os/dynamic_lib.h>

namespace clt::ffi
{
  class ColtPlugin
  {
    template<typename Ty>
    friend class Option;

    os::DynamicLib lib;
    i32 errc            = 0;
    u8 setup_ran : 1    = false;
    u8 shutdown_ran : 1 = false;

    ColtPlugin() = delete;
    ColtPlugin(os::DynamicLib&& lib) noexcept
        : lib(std::move(lib))
    {
    }

  public:
    ColtPlugin(ColtPlugin&&) noexcept            = default;
    ColtPlugin& operator=(ColtPlugin&&) noexcept = default;

    /// @brief Possible failure on plugin opening
    enum class OpenError
    {
      /// @brief The path is either invalid, or the library could not be opened
      OS_ERR,
      /// @brief colt_setup returned a non-zero value.
      SETUP_ERR,
    };

    /// @brief The advertised features of the plugin
    enum class PluginType : u8
    {
      /// @brief Plugin that only provides a name
      EMPTY,
      /// @brief Plugin to which to transfer compiler generated errors
      REPORTER,
      /// @brief Plugin to which to pass a string to highlight
      HIGHLIGHT,
      /// @brief Plugin to which to pass the resulting AST
      BACKEND,
    };

    /// @brief Function that returns a string, without taking any arguments.
    /// Example: colt_desc, colt_name
    using fn_name_t = const Char8* (*)();
    /// @brief Function that returns an int without taking any arguments.
    using fn_setup_t = i32 (*)();
    /// @brief Function that takes no arguments and returns nothing
    using fn_shutdown_t = void (*)();
    /// @brief Function that returns the plugin type
    using fn_type_t = PluginType (*)();

    /// @brief Returns the name of the plugin
    /// @return The name of the plugin
    u8ZStringView name() const
    {
      return lib.find<fn_name_t>("colt_name")
          .and_then([](fn_name_t a) { return Option<u8ZStringView>(a()); })
          .value_or(u8ZStringView{});
    }

    /// @brief Returns the description of the plugin if it exist
    /// @return The description of the plugin or None
    Option<u8ZStringView> desc() const
    {
      return lib.find<fn_name_t>("colt_desc")
          .and_then([](fn_name_t a) { return u8ZStringView(a()); });
    }

    /// @brief Calls 'colt_setup' if it exist.
    /// This function is idempotent, and caches the result of its first run.
    /// @return 0 if colt_setup does not exist or the result of 'colt_setup'
    i32 run_setup()
    {
      if (setup_ran)
        return errc;
      errc = lib.find<fn_setup_t>("colt_setup")
                 .map([](fn_setup_t a) { return a(); })
                 .value_or(0);
      setup_ran = true;
      return errc;
    }

    /// @brief Calls 'colt_shutdown' if it exist.
    /// If run_setup was not called, this function does nothing.
    /// This function is idempotent, and is only run once.
    void run_shutdown()
    {
      if (lib.is_closed() || shutdown_ran || setup_ran == false)
        return;
      shutdown_ran = true;
      auto find    = lib.find<fn_shutdown_t>("colt_shutdown");
      if (find.is_value())
        (**find)();
    }

    /// @brief Calls run_shutdown on the plugin
    ~ColtPlugin() { run_shutdown(); }

    /// @brief Opens a ColtPlugin.
    /// If 'manual_setup' is false, then run_setup is automatically
    /// called, and on failure run_shutdown is run, then SETUP_ERR
    /// is returned
    /// @param path The path
    /// @param manual_setup If false, does not 'run_setup'
    /// @return None on errors
    static Expect<ColtPlugin, OpenError> open(
        const char* path, bool manual_setup = false) noexcept
    {
      auto lib = os::DynamicLib::open(path);
      if (lib == None || lib->find_symbol("colt_name") == nullptr)
        return {Error, OpenError::OS_ERR};
      auto plugin = ColtPlugin(std::move(*lib));
      if (manual_setup)
        return {std::move(plugin)};
      if (auto a = plugin.run_setup(); a == 0)
        return {std::move(plugin)};
      plugin.run_shutdown();
      return {Error, OpenError::SETUP_ERR};
    }

    /// @brief Opens a ColtPlugin
    /// @param path The path
    /// @return None on errors
    static Expect<ColtPlugin, OpenError> open(
        ZStringView path, bool manual_setup = false) noexcept
    {
      return open(path.data(), manual_setup);
    }
  };
} // namespace clt::ffi

#endif // !HG_COLTC_PLUGIN_LOADER
