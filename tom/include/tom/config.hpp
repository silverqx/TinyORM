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
#ifndef TINYTOM_MIGRATIONS_DIR
// NOLINTNEXTLINE(bugprone-macro-parentheses)
#  define TINYTOM_MIGRATIONS_DIR database/migrations
#endif

/* Default models path for the make:model command, the path can be absolute or
   relative (to the pwd at runtime) and will be stringified in the tom/application.cpp. */
#ifndef TINYTOM_MODELS_DIR
// NOLINTNEXTLINE(bugprone-macro-parentheses)
#  define TINYTOM_MODELS_DIR database/models
#endif

/* Default seeders path for the make:seeder command, the path can be absolute or
   relative (to the pwd at runtime) and will be stringified in the tom/application.cpp. */
#ifndef TINYTOM_SEEDERS_DIR
// NOLINTNEXTLINE(bugprone-macro-parentheses)
#  define TINYTOM_SEEDERS_DIR database/seeders
#endif

#endif // TOM_CONFIG_HPP
