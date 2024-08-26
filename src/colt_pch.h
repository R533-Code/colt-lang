#ifndef HG_COLT_PCH
#define HG_COLT_PCH

// COLT/MACRO

#include <colt/macro/config.h>
#if defined(COLT_DEBUG) && defined(COLT_MSVC)
  // needed  as realloc will be defined as a macro
  #undef realloc
#endif
#include <colt/macro/macro.h>
#include <colt/macro/assert.h>
#include <colt/macro/on_scope_exit.h>

#include <colt/typedefs.h>

// COLT/BIT

#include <colt/bit/bitfields.h>
#include <colt/bit/endian.h>
#include <colt/bit/operations.h>
#include <colt/bit/detect_simd.h>

// COLT/DSA

#include <colt/dsa/option.h>
#include <colt/dsa/expect.h>
#include <colt/dsa/string_view.h>

// COLT/IO

#include <colt/io/print.h>

// COLT/META

#include <colt/meta/map.h>
#include <colt/meta/reflect.h>
#include <colt/meta/string_literal.h>
#include <colt/meta/traits.h>

// COLT/NUM

#include <colt/num/math.h>
#include <colt/num/big_int.h>
#include <colt/num/big_rational.h>
#include <colt/num/check_overflow.h>

// COLT/OS

#include <colt/os/dynamic_lib.h>

#endif // !HG_COLT_PCH
