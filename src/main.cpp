#include <colt_pch.h>
#include <util/args.h>
#include <colt/os/mmap_file.h>
#include <frontend/lex/lex.h>
#include <frontend/err/composable_reporter.h>

using namespace clt;

/// @brief This is the main entry point of the compiler.
/// The true main function is defined by later, which
/// converts command line arguments to UTF8 and sets up
/// console support for Unicode on Windows.
/// @param argv The arguments value
/// @return The exit code
int colt_main(Span<const char8_t*> argv)
{
  COLT_TRACE_FN_C(clt::Color::Crimson);
  cl::parse_command_line_options<CMDs>(
      argv, COLTC_EXECUTABLE_NAME, "The Colt compiler.");  
  auto val = os::ViewOfFile::open("test.txt");
  if (val.is_value())
  {
    io::print_message("Opened 'test.txt'!");
    auto reporter = lng::make_error_reporter<lng::ConsoleReporter>();
    auto value    = lng::lex(*reporter, *val->view());
    for (auto& i : value.token_buffer())
      lng::print_token(i, value);
  }
  else
    io::print_warn("Could not open 'test.txt'!");

  io::print_warn("REPL is not implemented.");
  return 0;
}

#ifdef COLT_WINDOWS
  #include <Windows.h>
  #include <fcntl.h>
  #include <io.h>

// On Windows, we make use of 'wmain' to obtain
// the arguments as Unicode.
int wmain(int argc, const wchar_t** argv)
{
  COLT_TRACE_FN_C(clt::Color::Crimson);
  
  using namespace clt;

  // Set console code page to UTF-8 so console known how to interpret string data
  SetConsoleOutputCP(CP_UTF8);
  // Set support to wchar_t in Console
  //_setmode(_fileno(stdin), _O_U16TEXT);

  // Allocator used for needed memory to convert 'argv' to UTF8
  mem::FallbackAllocator<mem::StackAllocator<1024>, mem::Mallocator> allocator;
  // The array of 'const char8_t**'
  auto pointers = allocator.alloc(sizeof(const char8_t*) * (argc + 1));

  // The number of bytes necessary for storing UTF8 converted 'argv'.
  size_t alloc_size = 0;
  for (int i = 0; i < argc; i++)
  {
    auto ptr  = reinterpret_cast<const char16_t*>(argv[i]);
    auto size = uni::unitlen(ptr) + 1;
    // Little trick: we store the unit length as it is needed after
    reinterpret_cast<uintptr_t*>(pointers.ptr())[i] = size;
    // Compute the necessary size
    alloc_size += simdutf::utf8_length_from_utf16(ptr, size);
  }
  // The array of 'char8_t'
  auto str         = allocator.alloc(alloc_size);
  auto current_str = (char8_t*)str.ptr();
  for (int i = 0; i < argc; i++)
  {
    auto ptr     = reinterpret_cast<const char16_t*>(argv[i]);
    // We convert back the length
    auto written = simdutf::convert_utf16_to_utf8(
        ptr, reinterpret_cast<uintptr_t*>(pointers.ptr())[i],
        reinterpret_cast<char*>(current_str));
    if (written == 0)
    {
      io::print_fatal("Argument '{}' is not valid Unicode!", i);
      return -1;
    }
    reinterpret_cast<const char8_t**>(pointers.ptr())[i] = current_str;
    current_str += written;
  }
  // Set the last argument to NULL
  reinterpret_cast<const char8_t**>(pointers.ptr())[argc] = nullptr;

  try
  {
    return colt_main(
        clt::Span{reinterpret_cast<const char8_t**>(pointers.ptr()), (size_t)argc});
  }
  catch (const std::exception& e)
  {
    io::print_fatal(stderr, "Uncaught Exception: {}", e.what());
    return -1;
  }
}

#else

int main(int argc, const char** argv)
{
  try
  {
    return colt_main(clt::Span{reinterpret_cast<const char8_t**>(argv), argc});
  }
  catch (const std::exception& e)
  {
    std::printf("FATAL: Uncaught Exception: %s", e.what());
    return -1;
  }
}

#endif // COLT_WINDOWS