#pragma once
#ifndef ORM_DRIVERS_MACROS_EXPORT_HPP
#define ORM_DRIVERS_MACROS_EXPORT_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <orm/macros/export_common.hpp>

#ifdef TINYDRIVERS_BUILDING_SHARED
#  define TINYDRIVERS_EXPORT TINY_DECL_EXPORT
#elif defined(TINYDRIVERS_LINKING_SHARED)
#  define TINYDRIVERS_EXPORT TINY_DECL_IMPORT
#endif

// Building library archive (static)
#ifndef TINYDRIVERS_EXPORT
#  define TINYDRIVERS_EXPORT
#endif

#endif // ORM_DRIVERS_MACROS_EXPORT_HPP
