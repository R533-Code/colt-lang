"""Generates an empty plugin for the Colt Compiler.
"""

# For file dialogs
import tkinter as tk
import tkinter.filedialog as filedialog
import os

if os.name == 'nt':
  from ctypes import windll
  windll.shcore.SetProcessDpiAwareness(1)

root = tk.Tk()
root.withdraw()

try:
  PLUGIN_NAME = input("Enter plugin name: ").strip()
  PLUGIN_PATH = filedialog.askdirectory(title="Folder in which to Generate")
  if PLUGIN_PATH == "":
    exit(0)
except Exception as E:
  print(f"Error: {str(E)}")
  exit(0)

PLUGIN_PATH = os.path.join(PLUGIN_PATH, PLUGIN_NAME)
os.makedirs(PLUGIN_PATH, exist_ok=True)
os.makedirs(os.path.join(PLUGIN_PATH, "src"), exist_ok=True)

cmake = open(os.path.join(PLUGIN_PATH, "CMakeLists.txt"), "w")
cmake.write(f"""cmake_minimum_required(VERSION 3.21)

include(FetchContent)

# C++20 is required
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project({PLUGIN_NAME}-Plugin VERSION 0.0.0.0 LANGUAGES CXX)
set(BUILD_SHARED_LIBS FALSE CACHE BOOL "" FORCE)

# Save all the files to compile
file(GLOB_RECURSE PluginHeaders "src/*.h")
file(GLOB_RECURSE PluginUnits "src/*.cpp")

# Create the shared library
add_library({PLUGIN_NAME} SHARED
  ${{PluginUnits}} ${{PluginHeaders}}
)

#########################################
# COLT-CPP
message(STATUS "Adding 'colt-cpp'...")
FetchContent_Declare(
  "coltcpp"
  GIT_REPOSITORY "https://github.com/R533-Code/colt-cpp"
  GIT_TAG main
)
FetchContent_MakeAvailable(coltcpp)

target_link_libraries({PLUGIN_NAME} PUBLIC colt::coltcpp)
message(STATUS "Added 'colt-cpp'!\n")
#########################################
""")
cmake.close()

cpp = open(os.path.join(PLUGIN_PATH, "src/main.cpp"), "w")
cpp.write(f"""#include <colt/macro/macro.h>
#include <colt/uni/unicode.h>

using namespace clt;

/// @brief The advertised features of the plugin
enum class PluginType : u8
{{
  /// @brief Plugin to which to transfer compiler generated errors
  REPORTER,
  /// @brief Plugin to which to pass a string to highlight
  HIGHLIGHT,
  /// @brief Plugin to which to pass the resulting AST
  BACKEND,
  /// @brief Plugin that only provides a name
  EMPTY,
}};

extern "C" CLT_EXPORT auto colt_name() {{
  return (const Char8*)"{PLUGIN_NAME}"_UTF8;
}}

extern "C" CLT_EXPORT auto colt_desc() {{
  return (const Char8*)"<No Description>"_UTF8;
}}

extern "C" CLT_EXPORT PluginType colt_purpose() {{
  return PluginType::EMPTY;
}}

/// @brief The setup function.
/// This function is called once to initialize the plugin.
/// @return 0 if initialization was successful.
extern "C" CLT_EXPORT i32 colt_setup() {{
  // Add setup code if needed
  return 0;
}}

/// @brief The shutdown function.
/// This function is called once when the plugin is unloaded.
extern "C" CLT_EXPORT void colt_shutdown() {{
  // Free any resources created by colt_setup
}}
""")