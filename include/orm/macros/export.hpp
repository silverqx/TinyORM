#pragma once
#ifndef ORM_MACROS_EXPORT_HPP
#define ORM_MACROS_EXPORT_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include "orm/macros/export_common.hpp"

#ifdef TINYORM_BUILDING_SHARED
#  define TINYORM_EXPORT TINY_DECL_EXPORT
#elif defined(TINYORM_LINKING_SHARED)
#  define TINYORM_EXPORT TINY_DECL_IMPORT
#endif

// Building library archive (static)
#ifndef TINYORM_EXPORT
#  define TINYORM_EXPORT
#endif

#endif // ORM_MACROS_EXPORT_HPP
