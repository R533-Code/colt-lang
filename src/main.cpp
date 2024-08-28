#include <colt_pch.h>
#include <colt_config.h>

#include <ffi.h>

unsigned char foo(unsigned int, float)
{
  return 129;
}

int main(int argc, const char** argv)
{
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

  ffi_cif cif;
  ffi_type* arg_types[2];
  void* arg_values[2];
  ffi_status status;

  // Because the return value from foo() is smaller than sizeof(long), it
  // must be passed as ffi_arg or ffi_sarg.
  ffi_arg result;

  // Specify the data type of each argument. Available types are defined
  // in <ffi/ffi.h>.
  arg_types[0] = &ffi_type_uint;
  arg_types[1] = &ffi_type_float;

  // Prepare the ffi_cif structure.
  if ((status = ffi_prep_cif(&cif, FFI_DEFAULT_ABI, 2, &ffi_type_uint8, arg_types))
      != FFI_OK)
  {
    // Handle the ffi_status error.
  }

  // Specify the values of each argument.
  unsigned int arg1 = 42;
  float arg2        = 5.1f;

  arg_values[0] = &arg1;
  arg_values[1] = &arg2;

  // Invoke the function.
  ffi_call(&cif, FFI_FN(foo), &result, arg_values);

  // The ffi_arg 'result' now contains the unsigned char returned from foo(),
  // which can be accessed by a typecast.
  printf("result is %hhu", (unsigned char)result);

}