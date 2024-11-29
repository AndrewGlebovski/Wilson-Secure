/**
 * \file
 * \brief Contains common functions.
*/

#pragma once

// ============================================================================

#include <cstdio>         // printf
#include <cstdlib>        // exit

// ============================================================================

#define ASSERT(condition, ...)    \
do {                              \
  if (!(condition)) {             \
    fprintf(stderr, __VA_ARGS__); \
    exit(1);                      \
  }                               \
} while(0)
