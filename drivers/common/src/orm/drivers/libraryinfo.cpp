#include "orm/drivers/libraryinfo.hpp"

#include <orm/macros/archdetect.hpp>
#include <orm/macros/compilerdetect.hpp>

#include "orm/drivers/config_p.hpp" // IWYU pragma: keep
#include "orm/drivers/version.hpp"

#ifdef TINYDRIVERS_MYSQL_DRIVER
#  include "orm/drivers/mysql/version.hpp"
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

using namespace Qt::StringLiterals; // NOLINT(google-build-using-namespace)

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
    static const auto ON = u"ON"_s;
    static const auto OFF = u"OFF"_s;

    return {
#ifdef TINYDRIVERS_BUILDING_SHARED
        {u"TINYDRIVERS_BUILDING_SHARED"_s, ON},
#else
        {u"TINYDRIVERS_BUILDING_SHARED"_s, OFF},
#endif
#ifdef TINYDRIVERS_DEBUG
        {u"TINYDRIVERS_DEBUG"_s, ON},
#else
        {u"TINYDRIVERS_DEBUG"_s, OFF},
#endif
#ifdef TINYDRIVERS_EXTERN_CONSTANTS
        {u"TINYDRIVERS_EXTERN_CONSTANTS"_s, ON},
#else
        {u"TINYDRIVERS_EXTERN_CONSTANTS"_s, OFF},
#endif
#ifdef TINYDRIVERS_INLINE_CONSTANTS
        {u"TINYDRIVERS_INLINE_CONSTANTS"_s, ON},
#else
        {u"TINYDRIVERS_INLINE_CONSTANTS"_s, OFF},
#endif
#ifdef TINYDRIVERS_MYSQL_DRIVER
        {u"TINYDRIVERS_MYSQL_DRIVER"_s, ON},
#else
        {u"TINYDRIVERS_MYSQL_DRIVER"_s, OFF},
#endif
#ifdef TINYDRIVERS_MYSQL_LOADABLE_LIBRARY
        {u"TINYDRIVERS_MYSQL_LOADABLE_LIBRARY"_s, ON},
#else
        {u"TINYDRIVERS_MYSQL_LOADABLE_LIBRARY"_s, OFF},
#endif
#ifdef TINYDRIVERS_MYSQL_PATH
        {u"TINYDRIVERS_MYSQL_PATH"_s, QString::fromUtf8(
                                          TINY_STRINGIFY(TINYDRIVERS_MYSQL_PATH))},
#endif
#ifdef TINYDRIVERS_NO_DEBUG
        {u"TINYDRIVERS_NO_DEBUG"_s, ON},
#else
        {u"TINYDRIVERS_NO_DEBUG"_s, OFF},
#endif
#ifdef TINYDRIVERS_TESTS_CODE
        {u"TINYDRIVERS_TESTS_CODE"_s, ON},
#else
        {u"TINYDRIVERS_TESTS_CODE"_s, OFF},
#endif
#ifdef TINYDRIVERS_USING_PCH
        {u"TINYDRIVERS_USING_PCH"_s, ON},
#else
        {u"TINYDRIVERS_USING_PCH"_s, OFF},
#endif
    };
}

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE
