#pragma once
#ifndef ORM_MACROS_SYSTEMHEADER_HPP
#define ORM_MACROS_SYSTEMHEADER_HPP

/* The system_header pragma is and must be disabled for all our libraries and executables.
   Which means that every library/executable must define the private C macro
   TINYORM_PRAGMA_SYSTEM_HEADER_OFF, if this is not the case, all warnings will be
   suppressed, so this needs a special care if a new library/executable is added
   to the TinyORM project. */
#ifndef TINYORM_PRAGMA_SYSTEM_HEADER_OFF
// Clang masquerades as GCC 4.2.0 so it has to be first
#  ifdef __clang__
#    define TINY_SYSTEM_HEADER _Pragma("clang system_header")
#  elif __GNUC__ * 100 + __GNUC_MINOR__ > 301
#    define TINY_SYSTEM_HEADER _Pragma("GCC system_header")
#  elif defined(_MSC_VER)
#    define TINY_SYSTEM_HEADER _Pragma("system_header")
#  endif
#endif

#ifndef TINY_SYSTEM_HEADER
#  define TINY_SYSTEM_HEADER
#endif

TINY_SYSTEM_HEADER

#endif // ORM_MACROS_SYSTEMHEADER_HPP
