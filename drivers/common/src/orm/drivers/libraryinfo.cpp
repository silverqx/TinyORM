#include "orm/drivers/libraryinfo.hpp"

#include <orm/macros/archdetect.hpp>
#include <orm/macros/compilerdetect.hpp>

#include "orm/drivers/config_p.hpp" // IWYU pragma: keep
#include "orm/drivers/version.hpp"

#ifdef TINYDRIVERS_MYSQL_DRIVER
#  include "orm/drivers/mysql/version.hpp"
#endif

#ifndef sl
/*! Alias for the QStringLiteral(). */
#  define sl(str) QStringLiteral(str)
#endif

// Build type release/debug
#ifdef TINYDRIVERS_NO_DEBUG
#  define DEBUG_STRING " release"
#else
#  define DEBUG_STRING " debug"
#endif

// Linking type for TinyDrivers (static/shared)
#ifdef TINYDRIVERS_BUILDING_SHARED
#  define TINYDRIVERS_LINKING_STRING " shared"
#else
#  define TINYDRIVERS_LINKING_STRING " static"
#endif

// Linking type for TinyMySql (static/shared/loadable)
#ifdef TINYDRIVERS_MYSQL_DRIVER
#  ifdef TINYDRIVERS_BUILDING_SHARED
#    ifdef TINYDRIVERS_MYSQL_LOADABLE_LIBRARY
#      define TINYMYSQL_LINKING_STRING " loadable"
#    else
#      define TINYMYSQL_LINKING_STRING " shared"
#    endif // TINYDRIVERS_MYSQL_LOADABLE_LIBRARY
#  else
#    define TINYMYSQL_LINKING_STRING " static"
#  endif // TINYDRIVERS_BUILDING_SHARED
#endif // TINYDRIVERS_MYSQL_DRIVER

// Full build type string for TinyDrivers
#define TINYDRIVERS_BUILD_STR                                                           \
    "TinyDrivers " TINYDRIVERS_VERSION_STR " (" ARCH_FULL TINYDRIVERS_LINKING_STRING    \
    DEBUG_STRING " build; by " TINYORM_COMPILER_STRING ")"

#ifdef TINYDRIVERS_MYSQL_DRIVER
// Full build type string for TinyMySql
#  define TINYMYSQL_BUILD_STR                                                     \
    "TinyMySql " TINYMYSQL_VERSION_STR " (" ARCH_FULL TINYMYSQL_LINKING_STRING    \
    DEBUG_STRING " build; by " TINYORM_COMPILER_STRING ")"
#endif

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers
{

/* public */

const char *LibraryInfo::build() noexcept
{
    return TINYDRIVERS_BUILD_STR;
}

#ifdef TINYDRIVERS_MYSQL_DRIVER
const char *LibraryInfo::mysqlBuild() noexcept
{
    return TINYMYSQL_BUILD_STR;
}
#endif

bool LibraryInfo::isDebugBuild() noexcept
{
#ifdef TINYDRIVERS_NO_DEBUG
    return false;
#else
    return true;
#endif
}

QVersionNumber LibraryInfo::version() noexcept
{
    return QVersionNumber(TINYDRIVERS_VERSION_MAJOR, TINYDRIVERS_VERSION_MINOR,
                          TINYDRIVERS_VERSION_BUGFIX);
}

std::map<QString, QString> LibraryInfo::allCMacrosMap()
{
    // To avoid include-ing the orm::constants
    static const auto ON = sl("ON");
    static const auto OFF = sl("OFF");

    return {
#ifdef TINYDRIVERS_BUILDING_SHARED
        {sl("TINYDRIVERS_BUILDING_SHARED"), ON},
#else
        {sl("TINYDRIVERS_BUILDING_SHARED"), OFF},
#endif
#ifdef TINYDRIVERS_DEBUG
        {sl("TINYDRIVERS_DEBUG"), ON},
#else
        {sl("TINYDRIVERS_DEBUG"), OFF},
#endif
#ifdef TINYDRIVERS_EXTERN_CONSTANTS
        {sl("TINYDRIVERS_EXTERN_CONSTANTS"), ON},
#else
        {sl("TINYDRIVERS_EXTERN_CONSTANTS"), OFF},
#endif
#ifdef TINYDRIVERS_INLINE_CONSTANTS
        {sl("TINYDRIVERS_INLINE_CONSTANTS"), ON},
#else
        {sl("TINYDRIVERS_INLINE_CONSTANTS"), OFF},
#endif
#ifdef TINYDRIVERS_MYSQL_DRIVER
        {sl("TINYDRIVERS_MYSQL_DRIVER"), ON},
#else
        {sl("TINYDRIVERS_MYSQL_DRIVER"), OFF},
#endif
#ifdef TINYDRIVERS_MYSQL_LOADABLE_LIBRARY
        {sl("TINYDRIVERS_MYSQL_LOADABLE_LIBRARY"), ON},
#else
        {sl("TINYDRIVERS_MYSQL_LOADABLE_LIBRARY"), OFF},
#endif
#ifdef TINYDRIVERS_MYSQL_PATH
        {sl("TINYDRIVERS_MYSQL_PATH"), QString::fromUtf8(
                                           TINY_STRINGIFY(TINYDRIVERS_MYSQL_PATH))},
#endif
#ifdef TINYDRIVERS_NO_DEBUG
        {sl("TINYDRIVERS_NO_DEBUG"), ON},
#else
        {sl("TINYDRIVERS_NO_DEBUG"), OFF},
#endif
#ifdef TINYDRIVERS_TESTS_CODE
        {sl("TINYDRIVERS_TESTS_CODE"), ON},
#else
        {sl("TINYDRIVERS_TESTS_CODE"), OFF},
#endif
#ifdef TINYDRIVERS_USING_PCH
        {sl("TINYDRIVERS_USING_PCH"), ON},
#else
        {sl("TINYDRIVERS_USING_PCH"), OFF},
#endif
    };
}

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE
