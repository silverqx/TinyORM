#pragma once
#ifndef ORM_MACROS_THREADLOCAL_HPP
#define ORM_MACROS_THREADLOCAL_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

// BUG TLS, clang linux crashes, gcc mingw lld/ld doesn't compile (duplicit tls), clang mingw ld stdlib weird 100,000 linker errors (COMDAT symbol xyz does not match section name), clang mingw lld stdlib works, but I'm giving up this clang MinGW support because still occasional crashes on TLS exit/destruction, it crashes around 1 time from 5, and now I have discovered that Clang 13 on Linux works, something was fixed in Clang 13, so I'm enabling support for Clang >=13 on Linux 🤯 silverqx
#if !(defined(__clang__) && defined(__MINGW32__)) && \
    !(defined(__clang__) && !defined(__MINGW32__) && __clang_major__ < 13) && \
    !(defined(__GNUG__) && !defined(__clang__) && defined(__MINGW32__))
#  define T_THREAD_LOCAL thread_local
#endif

#if !defined(T_THREAD_LOCAL)
#  define T_THREAD_LOCAL
#endif

#endif // ORM_MACROS_THREADLOCAL_HPP
