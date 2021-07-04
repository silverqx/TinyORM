#pragma once
#ifndef ORM_MACROS_HPP
#define ORM_MACROS_HPP

#if !defined(__clang__) && (defined(_MSC_VER) || defined(__GNUG__))
#  define T_LIKELY [[likely]]
#  define T_UNLIKELY [[unlikely]]
#else
#  define T_LIKELY
#  define T_UNLIKELY
#endif

#endif // ORM_MACROS_HPP
