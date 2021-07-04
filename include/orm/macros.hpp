#pragma once
#ifndef ORM_MACROS_HPP
#define ORM_MACROS_HPP

// CUR use everywhere silverqx
#if !defined(__clang__) && (defined(_MSC_VER) || defined(__GNUG__))
#  define T_LIKELY [[likely]]
#  define T_UNLIKELY [[unlikely]]
#else
#  define T_LIKELY
#  define T_UNLIKELY
#endif

#endif // ORM_MACROS_HPP
