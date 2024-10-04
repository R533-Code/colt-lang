#include <colt/macro/macro.h>
#include <colt/uni/unicode.h>

using namespace clt;

/// @brief The advertised features of the plugin
enum class PluginType : u8
{
  /// @brief Plugin to which to transfer compiler generated errors
  REPORTER,
  /// @brief Plugin to which to pass a string to highlight
  HIGHLIGHT,
  /// @brief Plugin to which to pass the resulting AST
  BACKEND,
  /// @brief Plugin that only provides a name
  EMPTY,
};

extern "C" CLT_EXPORT const Char8* colt_name()
{
  return (const Char8*)"EmptyPlugin"_UTF8;
}

extern "C" CLT_EXPORT const Char8* colt_desc()
{
  return (const Char8*)"This is an empty plugin to verify the plugin system"
                       " is functional."_UTF8;
}

extern "C" CLT_EXPORT PluginType colt_purpose()
{
  return PluginType::EMPTY;
}

/// @brief The setup function.
/// This function is called once to initialize the plugin.
/// @return 0 if initialization was successful.
extern "C" CLT_EXPORT i32 colt_setup()
{
  // Add setup code if needed
  return 0;
}

/// @brief The shutdown function.
/// This function is called once when the plugin is unloaded.
extern "C" CLT_EXPORT void colt_shutdown()
{
  // Free any resources created by colt_setup
}
