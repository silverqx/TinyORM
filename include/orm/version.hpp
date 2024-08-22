#pragma once
#ifndef ORM_VERSION_HPP
#define ORM_VERSION_HPP

// Excluded for the Resource compiler
#ifndef RC_INVOKED
#  include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER
#endif

#include "orm/macros/stringify.hpp"

/* Binary compatibility rules:
   https://community.kde.org/Policies/Binary_Compatibility_Issues_With_C++
   https://www.gnu.org/software/libtool/manual/html_node/Updating-version-info.html */
#define TINYORM_VERSION_MAJOR 0
#define TINYORM_VERSION_MINOR 38
#define TINYORM_VERSION_BUGFIX 1
#define TINYORM_VERSION_BUILD 0
// Should be empty for stable releases, and use the hyphen before to be compatible with SemVer!
#define TINYORM_VERSION_STATUS ""

#if TINYORM_VERSION_BUILD != 0
#  define TINYORM_PROJECT_VERSION TINY_STRINGIFY( \
     TINYORM_VERSION_MAJOR.TINYORM_VERSION_MINOR.TINYORM_VERSION_BUGFIX.TINYORM_VERSION_BUILD \
   ) TINYORM_VERSION_STATUS
#else
#  define TINYORM_PROJECT_VERSION TINY_STRINGIFY( \
     TINYORM_VERSION_MAJOR.TINYORM_VERSION_MINOR.TINYORM_VERSION_BUGFIX \
   ) TINYORM_VERSION_STATUS
#endif

/* Version Legend:
   M = Major, m = minor, p = patch, t = tweak, s = status ; [] - excluded if 0 */

// Format - M.m.p.t (used in Windows RC file)
#define TINYORM_FILEVERSION_STR TINY_STRINGIFY( \
    TINYORM_VERSION_MAJOR.TINYORM_VERSION_MINOR.TINYORM_VERSION_BUGFIX.TINYORM_VERSION_BUILD)
// Format - M.m.p[.t]-s
#define TINYORM_VERSION_STR TINYORM_PROJECT_VERSION
// Format - vM.m.p[.t]-s
#define TINYORM_VERSION_STR_2 "v" TINYORM_PROJECT_VERSION

/*! Version number macro, can be used to check API compatibility, format - MMmmpp. */
#define TINYORM_VERSION \
    (TINYORM_VERSION_MAJOR * 10000 + TINYORM_VERSION_MINOR * 100 + TINYORM_VERSION_BUGFIX)

/*! Compute the HEX representation from the given version numbers (for comparison).
    Can be used like:
    #if TINYORM_VERSION_HEX >= TINYORM_VERSION_CHECK(0, 37, 3) */
#define TINYORM_VERSION_CHECK(major, minor, bugfix) \
    ((major << 16) | (minor << 8) | (bugfix))

/*! HEX representation of the current TinyORM version (for comparison).
    TINYORM_VERSION_HEX is (major << 16) | (minor << 8) | bugfix. */
#define TINYORM_VERSION_HEX TINYORM_VERSION_CHECK(TINYORM_VERSION_MAJOR,    \
                                                  TINYORM_VERSION_MINOR,    \
                                                  TINYORM_VERSION_BUGFIX)

#endif // ORM_VERSION_HPP
