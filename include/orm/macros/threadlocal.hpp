#pragma once
#ifndef ORM_MACROS_THREADLOCAL_HPP
#define ORM_MACROS_THREADLOCAL_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#if !defined(__clang__)
#  define T_THREAD_LOCAL thread_local
#endif

#if !defined(T_THREAD_LOCAL)
#  define T_THREAD_LOCAL
#endif

#endif // ORM_MACROS_THREADLOCAL_HPP
