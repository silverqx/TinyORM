#pragma once
#ifndef TOM_CONFIG_HPP
#define TOM_CONFIG_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

// Check
#if defined(TINYTOM_NO_DEBUG) && defined(TINYTOM_DEBUG)
#  error Both TINYTOM_DEBUG and TINYTOM_NO_DEBUG defined.
#endif
// Debug build
#if !defined(TINYTOM_NO_DEBUG) && !defined(TINYTOM_DEBUG)
#  define TINYTOM_DEBUG
#endif

/* Default migrations path for the make:migration command, the path can be absolute or
   relative (to the pwd at runtime) and will be stringified in the tom/application.cpp. */
#ifndef TINYTOM_MIGRATIONS_PATH
// NOLINTNEXTLINE(bugprone-macro-parentheses)
#  define TINYTOM_MIGRATIONS_PATH database/migrations
#endif

#endif // TOM_CONFIG_HPP
