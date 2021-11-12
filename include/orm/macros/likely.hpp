#pragma once
#ifndef ORM_MACROS_LIKELY_HPP
#define ORM_MACROS_LIKELY_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#ifndef __has_cpp_attribute
#  define T_LIKELY
#elif __has_cpp_attribute(likely) >= 201803L
#  define T_LIKELY [[likely]]
#else
#  define T_LIKELY
#endif

#ifndef __has_cpp_attribute
#  define T_UNLIKELY
#elif __has_cpp_attribute(unlikely) >= 201803L
#  define T_UNLIKELY [[unlikely]]
#else
#  define T_UNLIKELY
#endif

#endif // ORM_MACROS_LIKELY_HPP
