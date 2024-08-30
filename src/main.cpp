#include <colt_pch.h>
#include <colt_config.h>
#include <util/ffi/ffi_caller.h>

struct MyStruct2
{
  int a;
  int b;
  COLT_ENABLE_REFLECTION();
};
COLT_DECLARE_TYPE(MyStruct2, a, b);

struct MyStruct
{

  int a;
  void* b;
  MyStruct2 c;

  COLT_ENABLE_REFLECTION();
};
COLT_DECLARE_TYPE(MyStruct, a, b, c);

MyStruct test()
{
  return {10, new char{12}, {100, 200}};
}

int main(int argc, const char** argv)
{
  using namespace clt;  
  using namespace clt::io;  
  
  print(
      "{}coltc{} (v{}.{}.{}.{} {}) on {}{}{} ({}{}{})\n"
      "using: {}colt-cpp{} (v{}.{}.{}.{}), {}unicode{} (v{}.{}.{}), "
      "{}simdutf{} (v{}), {}fmt{} (v{}.{}.{})",
      BrightCyanF, Reset, COLTC_VERSION_MAJOR, COLTC_VERSION_MINOR,
      COLTC_VERSION_PATCH, COLTC_VERSION_TWEAK, COLT_CONFIG_STRING, BrightBlueF,
      COLT_OS_STRING, Reset, BrightMagentaF, COLT_ARCH_STRING, Reset, BrightGreenF,
      Reset, COLT_VERSION_MAJOR, COLT_VERSION_MINOR, COLT_VERSION_PATCH,
      COLT_VERSION_TWEAK, BrightYellowF, Reset, COLT_UNICODE_VERSION_MAJOR,
      COLT_UNICODE_VERSION_MINOR, COLT_UNICODE_VERSION_UPDATE, BrightYellowF, Reset,
      SIMDUTF_VERSION, BrightYellowF, Reset, FMT_VERSION / 10'000,
        (FMT_VERSION % 10'000) / 100, FMT_VERSION % 100);
  
  auto value = FFICaller::call<MyStruct>(&test);
  print("{}", value);
}