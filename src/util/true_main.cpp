#include <colt_pch.h>
#include <util/args.h>

using namespace clt;

// The unified main
int colt_main(Span<const char8_t*> argv);

#ifdef COLT_WINDOWS
  #include <Windows.h>
  #include <fcntl.h>
  #include <io.h>

// Allocator for converting UTF16 arguments to UTF8.
// Global to avoid wasting stack space.
mem::FallbackAllocator<mem::StackAllocator<1024>, mem::Mallocator> wmain_allocator;

const char8_t** wmain_UTF16_to_UTF8(int argc, const wchar_t** argv)
{
  COLT_TRACE_FN();
  // The array of 'const char8_t**'
  auto pointers = wmain_allocator.alloc(sizeof(const char8_t*) * (argc + 1));

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
  auto str         = wmain_allocator.alloc(alloc_size);
  auto current_str = (char8_t*)str.ptr();
  for (int i = 0; i < argc; i++)
  {
    auto ptr = reinterpret_cast<const char16_t*>(argv[i]);
    // We convert back the length
    auto written = simdutf::convert_utf16_to_utf8(
        ptr, reinterpret_cast<uintptr_t*>(pointers.ptr())[i],
        reinterpret_cast<char*>(current_str));
    if (written == 0)
    {
      print_fatal("Argument '{}' is not valid Unicode!", i);
      return nullptr;
    }
    reinterpret_cast<const char8_t**>(pointers.ptr())[i] = current_str;
    current_str += written;
  }
  // Set the last argument to NULL
  reinterpret_cast<const char8_t**>(pointers.ptr())[argc] = nullptr;
  return reinterpret_cast<const char8_t**>(pointers.ptr());
}

// On Windows, we make use of 'wmain' to obtain
// the arguments as Unicode.
int wmain(int argc, const wchar_t** argv)
{
  COLT_TRACE_FN_C(clt::Color::Crimson);

  bool wait_for_tracy     = false;
  bool is_tracing_enabled = false;
  for (size_t i = 0; i < argc; i++)
  {
    if (std::wcscmp(argv[i], L"--enable-tracing") == 0)
    {
      is_tracing_enabled = true;
      continue;
    }
    if (std::wcscmp(argv[i], L"--wait-for-tracy") == 0)
    {
      wait_for_tracy = true;
      continue;
    }
  }
  #ifdef COLT_ENABLE_TRACING
  if (!is_tracing_enabled)
    COLT_STOP_TRACING();
  if (wait_for_tracy)
  {
    if (!is_tracing_enabled)
      print_warn(
          "'--wait-for-tracy' is inactive as '--enable-tracing' was not specified!");
    else
      wait_for_profiler();
  }
  #else
  if (wait_for_tracy || is_tracing_enabled)
    print_warn("'--wait-for-tracy' and '--enable-tracing' inactive as the compiler "
               "executable was not compiled with tracing support!");
  #endif // COLT_ENABLE_TRACING

  using namespace clt;

  // Set console code page to UTF-8 so console known how to interpret string data

  // Set support to wchar_t in Console
  //_setmode(_fileno(stdin), _O_U16TEXT);
  int return_value = -1;
  try
  {
    COLT_TRACE_EXPR(SetConsoleOutputCP(CP_UTF8));
    auto new_argv = wmain_UTF16_to_UTF8(argc, argv);
    if (new_argv != nullptr)
      return_value = colt_main(clt::Span<const char8_t*>{new_argv, (size_t)argc});
  }
  catch (const std::exception& e)
  {
    print_fatal(File::get_stderr(), "Uncaught Exception: {}", e.what());
  }

  return return_value;
}

#else

int main(int argc, const char** argv)
{
  COLT_TRACE_FN_C(clt::Color::Crimson);

  bool wait_for_tracy     = false;
  bool is_tracing_enabled = false;
  for (size_t i = 0; i < argc; i++)
  {
    if (std::strcmp(argv[i], "--enable-tracing") == 0)
    {
      is_tracing_enabled = true;
      continue;
    }
    if (std::strcmp(argv[i], "--wait-for-tracy") == 0)
    {
      wait_for_tracy = true;
      continue;
    }
  }
  #ifdef COLT_ENABLE_TRACING
  if (!is_tracing_enabled)
    COLT_STOP_TRACING();
  if (wait_for_tracy)
  {
    if (!is_tracing_enabled)
      io::print_warn(
          "'--wait-for-tracy' is inactive as '--enable-tracing' was not specified!");
    else
      details::wait_for_tracy();
  }
  #else
  if (wait_for_tracy || is_tracing_enabled)
    io::print_warn("'--wait-for-tracy' and '--enable-tracing' inactive as the "
                   "compiler executable was not compiled with tracing support!");
  #endif // COLT_ENABLE_TRACING

  int return_value = -1;
  try
  {
    return_value = colt_main(clt::Span<const char8_t*>{
        reinterpret_cast<const char8_t**>(argv), (size_t)argc});
  }
  catch (const std::exception& e)
  {
    std::printf("FATAL: Uncaught Exception: %s", e.what());
  }

  return return_value;
}

#endif // COLT_WINDOWS