#pragma once
#ifndef ORM_MACROS_THREADLOCAL_HPP
#define ORM_MACROS_THREADLOCAL_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

// CUR TLS, clang linux crashes, gcc mingw lld/ld doesn't compile (duplicit tls), clang mingw ld stdlib weird 100,000 linker errors (COMDAT symbol xyz does not match section name), clang mingw lld stdlib works silverqx
#if !(defined(__clang__) && !defined(__MINGW32__)) && \
    !(defined(__GNUG__) && !defined(__clang__) && defined(__MINGW32__))
#  define T_THREAD_LOCAL thread_local
#endif

#if !defined(T_THREAD_LOCAL)
#  define T_THREAD_LOCAL
#endif

#endif // ORM_MACROS_THREADLOCAL_HPP
