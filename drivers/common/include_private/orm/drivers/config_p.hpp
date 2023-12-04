#pragma once
#ifndef ORM_DRIVERS_CONFIG_P_HPP
#define ORM_DRIVERS_CONFIG_P_HPP

// Check
#if defined(TINYDRIVERS_EXTERN_CONSTANTS) && defined(TINYDRIVERS_INLINE_CONSTANTS)
#  error Both TINYDRIVERS_EXTERN_CONSTANTS and TINYDRIVERS_INLINE_CONSTANTS defined.
#endif
/* clang-cl shared build crashes with extern constants, force to inline constants. 😕🤔
   Only one option with the clang-cl is inline constants for both shared/static builds.
   Look at NOTES.txt[inline constants] how this funckin machinery works. 😎 */
#if !defined(TINYDRIVERS_INLINE_CONSTANTS) && defined(_MSC_VER) && defined(__clang__)
#  undef TINYDRIVERS_EXTERN_CONSTANTS
#  define TINYDRIVERS_INLINE_CONSTANTS
// Enforce extern constants in shared build/linking when a user did not define it
#elif !defined(TINYDRIVERS_EXTERN_CONSTANTS) && \
      !defined(TINYDRIVERS_INLINE_CONSTANTS) && \
        (defined(TINYDRIVERS_BUILDING_SHARED) || defined(TINYDRIVERS_LINKING_SHARED))
#  define TINYDRIVERS_EXTERN_CONSTANTS
#endif

// Check
#if defined(TINYDRIVERS_NO_DEBUG) && defined(TINYDRIVERS_DEBUG)
#  error Both TINYDRIVERS_DEBUG and TINYDRIVERS_NO_DEBUG defined.
#endif
// Debug build
#if !defined(TINYDRIVERS_NO_DEBUG) && !defined(TINYDRIVERS_DEBUG)
#  define TINYDRIVERS_DEBUG
#endif

/* Log queries with a time measurement, no checks here, it's different than our defaults.
   We need to do it the same way as QtSql, so by default no C macro is defined and
   you must define the QT_DEBUG_SQL or TINYDRIVERS_DEBUG_SQL if you want to
   log queries. There is no TINYDRIVERS_NO_DEBUG_SQL C macro. */

#endif // ORM_DRIVERS_CONFIG_P_HPP
