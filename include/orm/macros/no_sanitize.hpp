#pragma once
#ifndef ORM_MACROS_NO_SANITIZE_HPP
#define ORM_MACROS_NO_SANITIZE_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

/* UBSan in TinyORM is only supported for Clang on Linux. I don't know how these
   -fsanitize= options work or behave on different platforms and don't have time to
   figure it out deeply. Disabling on MSYS2 because -fsanitize= doesn't work there
   with GCC or Clang. Also defined TINYORM_USING_UBSAN because the compiler can
   throw an error/warning that attribute is ignored if these -fsanitize= options are not
   enabled (happens on MSYS2). Currently, the TINYORM_NO_SANITIZE() macro is only used
   for one method and this method isn't even called anywhere (so not a big deal). */
#if defined(__MINGW32__) || !defined(TINYORM_USING_UBSAN) || !defined(__has_attribute)
#  define TINYORM_NO_SANITIZE(sanitizer)
#elif __has_attribute(no_sanitize)
#  define TINYORM_NO_SANITIZE(sanitizer) __attribute__((no_sanitize(sanitizer)))
#else
#  define TINYORM_NO_SANITIZE(sanitizer)
#endif

#endif // ORM_MACROS_NO_SANITIZE_HPP
