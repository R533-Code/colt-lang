/** @file memory.h
* Contains helper for managing memory allocations.
*/

#ifndef HG_COLTI_MEMORY
#define HG_COLTI_MEMORY

#include "console_colors.h"
#include <stdio.h>

/// @brief Allocates a block of size 'size' from the heap, but terminates if the pointer is NULL
/// @param size The size of the block to allocate
/// @return a non-NULL pointer
void* checked_malloc(size_t size);

/// @brief Frees the pointer if it isn't NULL, else terminates.
/// While this might seem not useful, any pointer obtained by checked_malloc
/// is not NULL. This means that if a pointer is NULL, a programming bug happened.
/// @param ptr The pointer to free
void checked_free(void* ptr);

#endif //HG_COLTI_MEMORY