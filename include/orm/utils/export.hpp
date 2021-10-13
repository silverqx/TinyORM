#pragma once
#ifndef TINYORM_EXPORT_HPP
#define TINYORM_EXPORT_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/utils/export_common.hpp"

#if defined(TINYORM_BUILDING_SHARED)
#  define SHAREDLIB_EXPORT TINY_DECL_EXPORT
#elif defined(TINYORM_LINKING_SHARED)
#  define SHAREDLIB_EXPORT TINY_DECL_IMPORT
#endif

// Building library archive
#ifndef SHAREDLIB_EXPORT
#  define SHAREDLIB_EXPORT
#endif

#endif // TINYORM_EXPORT_HPP
