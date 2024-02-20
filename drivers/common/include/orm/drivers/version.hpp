#pragma once
#ifndef ORM_DRIVERS_VERSION_HPP
#define ORM_DRIVERS_VERSION_HPP

// Excluded for the Resource compiler
#ifndef RC_INVOKED
#  include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER
#endif

#include <orm/macros/stringify.hpp>

/* Binary compatibility rules:
   https://community.kde.org/Policies/Binary_Compatibility_Issues_With_C++
   https://www.gnu.org/software/libtool/manual/html_node/Updating-version-info.html */
#define TINYDRIVERS_VERSION_MAJOR 0
#define TINYDRIVERS_VERSION_MINOR 1
#define TINYDRIVERS_VERSION_BUGFIX 0
#define TINYDRIVERS_VERSION_BUILD 0
// Should be empty for stable releases, and use the hyphen before to be compatible with SemVer!
#define TINYDRIVERS_VERSION_STATUS ""

#if TINYDRIVERS_VERSION_BUILD != 0
#  define TINYDRIVERS_PROJECT_VERSION TINY_STRINGIFY( \
     TINYDRIVERS_VERSION_MAJOR.TINYDRIVERS_VERSION_MINOR.TINYDRIVERS_VERSION_BUGFIX.TINYDRIVERS_VERSION_BUILD \
   ) TINYDRIVERS_VERSION_STATUS
#else
#  define TINYDRIVERS_PROJECT_VERSION TINY_STRINGIFY( \
     TINYDRIVERS_VERSION_MAJOR.TINYDRIVERS_VERSION_MINOR.TINYDRIVERS_VERSION_BUGFIX \
   ) TINYDRIVERS_VERSION_STATUS
#endif

/* Version Legend:
   M = Major, m = minor, p = patch, t = tweak, s = status ; [] - excluded if 0 */

// Format - M.m.p.t (used in Windows RC file)
#define TINYDRIVERS_FILEVERSION_STR TINY_STRINGIFY( \
    TINYDRIVERS_VERSION_MAJOR.TINYDRIVERS_VERSION_MINOR.TINYDRIVERS_VERSION_BUGFIX.TINYDRIVERS_VERSION_BUILD)
// Format - M.m.p[.t]-s
#define TINYDRIVERS_VERSION_STR TINYDRIVERS_PROJECT_VERSION
// Format - vM.m.p[.t]-s
#define TINYDRIVERS_VERSION_STR_2 "v" TINYDRIVERS_PROJECT_VERSION

/*! Version number macro, can be used to check API compatibility, format - MMmmpp. */
#define TINYDRIVERS_VERSION \
    (TINYDRIVERS_VERSION_MAJOR * 10000 + TINYDRIVERS_VERSION_MINOR * 100 + TINYDRIVERS_VERSION_BUGFIX)

#endif // ORM_DRIVERS_VERSION_HPP
