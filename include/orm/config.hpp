#pragma once
#ifndef ORM_CONFIG_HPP
#define ORM_CONFIG_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

// Check
#if defined(TINYORM_EXTERN_CONSTANTS) && defined(TINYORM_INLINE_CONSTANTS)
#  error Both TINYORM_EXTERN_CONSTANTS and TINYORM_INLINE_CONSTANTS defined.
#endif
/* Enforce inline/extern constants if neither is defined:
    - extern constants for shared builds
    - inline constants for static builds
   Look at NOTES.txt[inline constants] how this funckin machinery works. 😎 */
#if !defined(TINYORM_EXTERN_CONSTANTS) && !defined(TINYORM_INLINE_CONSTANTS)
#  if defined(TINYORM_BUILDING_SHARED) || defined(TINYORM_LINKING_SHARED)
#    define TINYORM_EXTERN_CONSTANTS
#  else
#    define TINYORM_INLINE_CONSTANTS
#  endif
#endif

// Check
#if defined(TINYORM_NO_DEBUG) && defined(TINYORM_DEBUG)
#  error Both TINYORM_DEBUG and TINYORM_NO_DEBUG defined.
#endif
// Debug build (default build)
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

/* The libstdc++ shipped with the GCC <12.1 doesn't allow an incomplete
   mapped_type (value) in the std::unordered_map. */
#if defined(__GNUG__) && defined(_GLIBCXX_RELEASE) && defined(__GLIBCXX__) && \
    (_GLIBCXX_RELEASE < 12 || (_GLIBCXX_RELEASE >= 12 && __GLIBCXX__ < 20220513))
#  define TINY_NO_INCOMPLETE_UNORDERED_MAP
#endif

#endif // ORM_CONFIG_HPP
