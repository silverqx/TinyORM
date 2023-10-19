#pragma once
#ifndef ORM_VERSION_HPP
#define ORM_VERSION_HPP

// Excluded for the Resource compiler
#ifndef RC_INVOKED
#  include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER
#endif

/* Binary compatibility rules:
   https://community.kde.org/Policies/Binary_Compatibility_Issues_With_C++
   https://www.gnu.org/software/libtool/manual/html_node/Updating-version-info.html */
#define TINYORM_VERSION_MAJOR 0
#define TINYORM_VERSION_MINOR 36
#define TINYORM_VERSION_BUGFIX 1
#define TINYORM_VERSION_BUILD 0
// Should be empty for stable releases, and use the hyphen before to be compatible with SemVer!
#define TINYORM_VERSION_STATUS ""

// NOLINTNEXTLINE(bugprone-reserved-identifier)
#define TINYORM__STRINGIFY(x) #x
#define TINYORM_STRINGIFY(x) TINYORM__STRINGIFY(x)

#if TINYORM_VERSION_BUILD != 0
#  define TINYORM_PROJECT_VERSION TINYORM_STRINGIFY( \
     TINYORM_VERSION_MAJOR.TINYORM_VERSION_MINOR.TINYORM_VERSION_BUGFIX.TINYORM_VERSION_BUILD \
   ) TINYORM_VERSION_STATUS
#else
#  define TINYORM_PROJECT_VERSION TINYORM_STRINGIFY( \
     TINYORM_VERSION_MAJOR.TINYORM_VERSION_MINOR.TINYORM_VERSION_BUGFIX \
   ) TINYORM_VERSION_STATUS
#endif

/* Version Legend:
   M = Major, m = minor, p = patch, t = tweak, s = status ; [] - excluded if 0 */

// Format - M.m.p.t (used in Windows RC file)
#define TINYORM_FILEVERSION_STR TINYORM_STRINGIFY( \
    TINYORM_VERSION_MAJOR.TINYORM_VERSION_MINOR.TINYORM_VERSION_BUGFIX.TINYORM_VERSION_BUILD)
// Format - M.m.p[.t]-s
#define TINYORM_VERSION_STR TINYORM_PROJECT_VERSION
// Format - vM.m.p[.t]-s
#define TINYORM_VERSION_STR_2 "v" TINYORM_PROJECT_VERSION

/*! Version number macro, can be used to check API compatibility, format - MMmmpp. */
#define TINYORM_VERSION \
    (TINYORM_VERSION_MAJOR * 10000 + TINYORM_VERSION_MINOR * 100 + TINYORM_VERSION_BUGFIX)

#endif // ORM_VERSION_HPP
