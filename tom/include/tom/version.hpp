#pragma once
#ifndef TOM_VERSION_HPP
#define TOM_VERSION_HPP

// Excluded for the Resource compiler
#ifndef RC_INVOKED
#  include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER
#endif

#define TINYTOM_VERSION_MAJOR 0
#define TINYTOM_VERSION_MINOR 5
#define TINYTOM_VERSION_BUGFIX 7
#define TINYTOM_VERSION_BUILD 0
// Should be empty for stable releases, and use hypen before for SemVer compatibility!
#define TINYTOM_VERSION_STATUS ""

// NOLINTNEXTLINE(bugprone-reserved-identifier)
#define TINYTOM__STRINGIFY(x) #x
#define TINYTOM_STRINGIFY(x) TINYTOM__STRINGIFY(x)

#if TINYTOM_VERSION_BUILD != 0
#  define TINYTOM_PROJECT_VERSION TINYTOM_STRINGIFY( \
     TINYTOM_VERSION_MAJOR.TINYTOM_VERSION_MINOR.TINYTOM_VERSION_BUGFIX.TINYTOM_VERSION_BUILD \
   ) TINYTOM_VERSION_STATUS
#else
#  define TINYTOM_PROJECT_VERSION TINYTOM_STRINGIFY( \
     TINYTOM_VERSION_MAJOR.TINYTOM_VERSION_MINOR.TINYTOM_VERSION_BUGFIX \
   ) TINYTOM_VERSION_STATUS
#endif

/* Version Legend:
   M = Major, m = minor, p = patch, t = tweak, s = status ; [] - excluded if 0 */

// Format - M.m.p.t (used in Windows RC file)
#define TINYTOM_FILEVERSION_STR TINYTOM_STRINGIFY( \
    TINYTOM_VERSION_MAJOR.TINYTOM_VERSION_MINOR.TINYTOM_VERSION_BUGFIX.TINYTOM_VERSION_BUILD)
// Format - M.m.p[.t]-s
#define TINYTOM_VERSION_STR TINYTOM_PROJECT_VERSION
// Format - vM.m.p[.t]-s
#define TINYTOM_VERSION_STR_2 "v" TINYTOM_PROJECT_VERSION

/*! Version number macro, can be used to check API compatibility, format - MMmmpp. */
#define TINYTOM_VERSION \
    (TINYTOM_VERSION_MAJOR * 10000 + TINYTOM_VERSION_MINOR * 100 + TINYTOM_VERSION_BUGFIX)

#endif // TOM_VERSION_HPP
