#pragma once
#ifndef ORM_DRIVERS_MYSQL_VERSION_HPP
#define ORM_DRIVERS_MYSQL_VERSION_HPP

// Excluded for the Resource compiler
#ifndef RC_INVOKED
#  include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER
#endif

#include <orm/macros/stringify.hpp>

/* Binary compatibility rules:
   https://community.kde.org/Policies/Binary_Compatibility_Issues_With_C++
   https://www.gnu.org/software/libtool/manual/html_node/Updating-version-info.html */
#define TINYMYSQL_VERSION_MAJOR 0
#define TINYMYSQL_VERSION_MINOR 1
#define TINYMYSQL_VERSION_BUGFIX 1
#define TINYMYSQL_VERSION_BUILD 0
// Should be empty for stable releases, and use the hyphen before to be compatible with SemVer!
#define TINYMYSQL_VERSION_STATUS ""

#if TINYMYSQL_VERSION_BUILD != 0
#  define TINYMYSQL_PROJECT_VERSION TINY_STRINGIFY( \
     TINYMYSQL_VERSION_MAJOR.TINYMYSQL_VERSION_MINOR.TINYMYSQL_VERSION_BUGFIX.TINYMYSQL_VERSION_BUILD \
   ) TINYMYSQL_VERSION_STATUS
#else
#  define TINYMYSQL_PROJECT_VERSION TINY_STRINGIFY( \
     TINYMYSQL_VERSION_MAJOR.TINYMYSQL_VERSION_MINOR.TINYMYSQL_VERSION_BUGFIX \
   ) TINYMYSQL_VERSION_STATUS
#endif

/* Version Legend:
   M = Major, m = minor, p = patch, t = tweak, s = status ; [] - excluded if 0 */

// Format - M.m.p.t (used in Windows RC file)
#define TINYMYSQL_FILEVERSION_STR TINY_STRINGIFY( \
    TINYMYSQL_VERSION_MAJOR.TINYMYSQL_VERSION_MINOR.TINYMYSQL_VERSION_BUGFIX.TINYMYSQL_VERSION_BUILD)
// Format - M.m.p[.t]-s
#define TINYMYSQL_VERSION_STR TINYMYSQL_PROJECT_VERSION
// Format - vM.m.p[.t]-s
#define TINYMYSQL_VERSION_STR_2 "v" TINYMYSQL_PROJECT_VERSION

/*! Version number macro, can be used to check API compatibility, format - MMmmpp. */
#define TINYMYSQL_VERSION \
    (TINYMYSQL_VERSION_MAJOR * 10000 + TINYMYSQL_VERSION_MINOR * 100 + TINYMYSQL_VERSION_BUGFIX)

/*! Compute the HEX representation from the given version numbers (for comparison).
    Can be used like:
    #if TINYMYSQL_VERSION_HEX >= TINYMYSQL_VERSION_CHECK(0, 37, 3) */
#define TINYMYSQL_VERSION_CHECK(major, minor, bugfix) \
    ((major << 16) | (minor << 8) | (bugfix))

/*! HEX representation of the current TinyMySql version (for comparison).
    TINYMYSQL_VERSION_HEX is (major << 16) | (minor << 8) | bugfix. */
#define TINYMYSQL_VERSION_HEX TINYMYSQL_VERSION_CHECK(TINYMYSQL_VERSION_MAJOR,    \
                                                      TINYMYSQL_VERSION_MINOR,    \
                                                      TINYMYSQL_VERSION_BUGFIX)

/* Library Features */

/*! Fixed QDateTime handling introduced in Qt v6.8.
    https://github.com/qt/qtbase/commit/2781c3b6248fe4410a7afffd41bad72d8567fc95
    https://codereview.qt-project.org/c/qt/qtbase/+/546954 */
#define tinymysql_lib_utc_qdatetime 20240618L

#endif // ORM_DRIVERS_MYSQL_VERSION_HPP
