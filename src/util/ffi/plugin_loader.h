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

    ColtPlugin() = delete;
    ColtPlugin(os::DynamicLib&& lib) noexcept
        : lib(std::move(lib))
    {
    }
    ColtPlugin(ColtPlugin&&) noexcept = default;
    ColtPlugin& operator=(ColtPlugin&&) noexcept = default;

  public:
    using fn_name_t = const char8_t*(*)();
    using fn_setup_t = int(*)();

    /// @brief Returns the name of the plugin
    /// @return The name of the plugin
    const char8_t* name()
    {
      return (**lib.find<fn_name_t>("colt_name"))();
    }

    /// @brief Calls 'colt_setup' if it exist
    /// @return 0 if colt_setup does not exist or the result of 'colt_setup'
    int run_setup()
    {
      auto ret = lib.find<fn_setup_t>("colt_setup");
      if (ret.is_none())
        return 0;
      return (*ret)();
    }

    /// @brief Opens a ColtPlugin
    /// @param path The path
    /// @return None on errors
    static Option<ColtPlugin> open(const char* path) noexcept
    {
      auto lib = os::DynamicLib::open(path);
      if (lib == None || lib->find_symbol("colt_name") == nullptr)
        return None;
      return Option{ColtPlugin(std::move(*lib))};
    }
    
    /// @brief Opens a ColtPlugin
    /// @param path The path
    /// @return None on errors
    static Option<ColtPlugin> open(ZStringView path) noexcept
    {
      return open(path.data());
    }
  };
}

#endif // !HG_COLTC_PLUGIN_LOADER
