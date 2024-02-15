#pragma once
#ifndef TINYUTILS_EXPORT_HPP
#define TINYUTILS_EXPORT_HPP

#include "orm/macros/export_common.hpp"

#ifdef TINYUTILS_BUILDING_SHARED
#  define TINYUTILS_EXPORT TINY_DECL_EXPORT
#elif defined(TINYUTILS_LINKING_SHARED)
#  define TINYUTILS_EXPORT TINY_DECL_IMPORT
#endif

// Building library archive
#ifndef TINYUTILS_EXPORT
#  define TINYUTILS_EXPORT
#endif

#endif // TINYUTILS_EXPORT_HPP
