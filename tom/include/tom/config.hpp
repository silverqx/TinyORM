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

#endif // TOM_CONFIG_HPP
