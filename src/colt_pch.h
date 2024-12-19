#ifndef HG_COLT_PCH
#define HG_COLT_PCH

// COLT/MACRO

#include <colt/macro/config.h>
#if defined(COLT_DEBUG) && defined(COLT_MSVC)
  // needed  as realloc will be defined as a macro
  #undef realloc
#endif

#include <colt/typedefs.h>
#include <util/tracing.h>

// COLT/BIT

#include <colt/num/bitfields.h>
#include <colt/algo/detect_simd.h>

// COLT/DSA

#include <colt/dsa/option.h>
#include <colt/dsa/expect.h>
#include <colt/dsa/string_view.h>

// COLT/IO

#include <colt/io/print.h>
#include <colt/io/dynamic_lib.h>
#include <colt/io/mmap.h>

// COLT/META

#include <colt/meta/map.h>
#include <colt/meta/reflect.h>
#include <colt/meta/string_literal.h>
#include <colt/meta/traits.h>

// COLT/NUM

#include <colt/num/math.h>
#include <colt/num/big_int.h>
#include <colt/num/big_rational.h>
#include <colt/num/overflow.h>

#endif // !HG_COLT_PCH
