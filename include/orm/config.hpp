#pragma once
#ifndef ORM_CONFIG_HPP
#define ORM_CONFIG_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

// Check
#if defined(TINYORM_EXTERN_CONSTANTS) && defined(TINYORM_INLINE_CONSTANTS)
#  error Both TINYORM_EXTERN_CONSTANTS and TINYORM_INLINE_CONSTANTS defined.
#endif
/* clang-cl shared build crashes with extern constants, force to inline constants. 😕🤔
   Only one option with the clang-cl is inline constants for both shared/static builds.
   Look at NOTES.txt[inline constants] how this funckin machinery works. 😎 */
#if !defined(TINYORM_INLINE_CONSTANTS) && defined(_MSC_VER) && defined(__clang__)
#  undef TINYORM_EXTERN_CONSTANTS
#  define TINYORM_INLINE_CONSTANTS
// Enforce extern constants in shared build/linking when a user did not define it
#elif !defined(TINYORM_EXTERN_CONSTANTS) && !defined(TINYORM_INLINE_CONSTANTS) && \
    (defined(TINYORM_BUILDING_SHARED) || defined(TINYORM_LINKING_SHARED))
#  define TINYORM_EXTERN_CONSTANTS
#endif

// Check
#if defined(TINYORM_NO_DEBUG) && defined(TINYORM_DEBUG)
#  error Both TINYORM_DEBUG and TINYORM_NO_DEBUG defined.
#endif
// Debug build
#if !defined(TINYORM_NO_DEBUG) && !defined(TINYORM_DEBUG)
#  define TINYORM_DEBUG
#endif

// Check
#if defined(TINYORM_NO_DEBUG_SQL) && defined(TINYORM_DEBUG_SQL)
#  error Both TINYORM_DEBUG_SQL and TINYORM_NO_DEBUG_SQL defined.
#endif
// Log queries with a time measurement
#if !defined(TINYORM_NO_DEBUG_SQL) && !defined(TINYORM_DEBUG_SQL)
#  define TINYORM_DEBUG_SQL
#endif

#endif // ORM_CONFIG_HPP
