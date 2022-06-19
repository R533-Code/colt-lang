/** @file precomph.h
* Precompiled header file containing (mostly) static includes
*/

#ifndef HG_COLT_PRECOMPH
#define HG_COLT_PRECOMPH

// NON-STD INCLUDES
#include "common.h"
#include "byte_code.h"
#include "disassemble.h"
#include "chunk.h"

#include "lang/scanner.h"
#include "lang/option.h"
#include "lang/ast.h"

#include "byte-code/byte_code_generator.h"

//VMs
#include "vm/stack_based_vm.h"

//UTILITIES
#include "structs/struct_string.h"
#include "util/parse_args.h"

#endif //HG_COLT_PRECOMPH