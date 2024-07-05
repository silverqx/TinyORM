#include "orm/libraryinfo.hpp"

#include "orm/constants.hpp"
#include "orm/macros/archdetect.hpp"
#include "orm/macros/compilerdetect.hpp"
#include "orm/utils/type.hpp"
#include "orm/version.hpp"

#ifndef sl
/*! Alias for the QStringLiteral(). */
#  define sl(str) QStringLiteral(str)
#endif

// Build type release/debug
#ifdef TINYORM_NO_DEBUG
#  define DEBUG_STRING " release"
#else
#  define DEBUG_STRING " debug"
#endif

// Linking type for TinyOrm (static/shared)
#ifdef TINYORM_BUILDING_SHARED
#  define TINYORM_LINKING_STRING " shared"
#else
#  define TINYORM_LINKING_STRING " static"
#endif

// Full build type string for TinyOrm
#define TINYORM_BUILD_STR                                                   \
    "TinyORM " TINYORM_VERSION_STR " (" ARCH_FULL TINYORM_LINKING_STRING    \
    DEBUG_STRING " build; by " TINYORM_COMPILER_STRING ")"

TINYORM_BEGIN_COMMON_NAMESPACE

using Orm::Constants::OFF; // NOLINT(misc-unused-using-decls)
using Orm::Constants::ON; // NOLINT(misc-unused-using-decls)

using TypeUtils = Orm::Utils::Type;

namespace Orm
{

/* public */

const char *LibraryInfo::build() noexcept
{
    return TINYORM_BUILD_STR;
}

bool LibraryInfo::isDebugBuild() noexcept
{
#ifdef TINYORM_NO_DEBUG
    return false;
#else
    return true;
#endif
}

QVersionNumber LibraryInfo::version() noexcept
{
    return QVersionNumber(TINYORM_VERSION_MAJOR, TINYORM_VERSION_MINOR,
                          TINYORM_VERSION_BUGFIX);
}

#ifndef TINY_MACRO_BOOL
/*! Convert the macro BOOL type value passed by the the C macro to the ON/OFF QString. */
#  define TINY_MACRO_BOOL(value) TypeUtils::normalizeCMakeBool(TINY_STRINGIFY(value))
#endif

std::map<QString, QString> LibraryInfo::ormCMacrosMap()
{
    return {
#ifdef TINYORM_BUILDING_SHARED
        {sl("TINYORM_BUILDING_SHARED"), ON},
#else
        {sl("TINYORM_BUILDING_SHARED"), OFF},
#endif
#ifdef TINYORM_DEBUG
        {sl("TINYORM_DEBUG"), ON},
#else
        {sl("TINYORM_DEBUG"), OFF},
#endif
#ifdef TINYORM_DEBUG_SQL
        {sl("TINYORM_DEBUG_SQL"), ON},
#else
        {sl("TINYORM_DEBUG_SQL"), OFF},
#endif
// Newline needed - QtCreator syntax highlighting bug
#ifdef TINYORM_DISABLE_ORM
        {sl("TINYORM_DISABLE_ORM"), ON},
#else
        {sl("TINYORM_DISABLE_ORM"), OFF},
#endif
#ifdef TINYORM_DISABLE_THREAD_LOCAL
        {sl("TINYORM_DISABLE_THREAD_LOCAL"), ON},
#else
        {sl("TINYORM_DISABLE_THREAD_LOCAL"), OFF},
#endif
#ifdef TINYORM_DISABLE_TOM
        {sl("TINYORM_DISABLE_TOM"), ON},
#else
        {sl("TINYORM_DISABLE_TOM"), OFF},
#endif
#ifdef TINYORM_EXTERN_CONSTANTS
        {sl("TINYORM_EXTERN_CONSTANTS"), ON},
#else
        {sl("TINYORM_EXTERN_CONSTANTS"), OFF},
#endif
#ifdef TINYORM_INLINE_CONSTANTS
        {sl("TINYORM_INLINE_CONSTANTS"), ON},
#else
        {sl("TINYORM_INLINE_CONSTANTS"), OFF},
#endif
// CMake ON/OFF
#ifdef TINYORM_LTO
        {sl("TINYORM_LTO"), TINY_MACRO_BOOL(TINYORM_LTO)},
#endif
#ifdef TINYORM_MYSQL_PING
        {sl("TINYORM_MYSQL_PING"), ON},
#else
        {sl("TINYORM_MYSQL_PING"), OFF},
#endif
// Newline needed - QtCreator syntax highlighting bug
#ifdef TINYORM_NO_DEBUG
        {sl("TINYORM_NO_DEBUG"), ON},
#else
        {sl("TINYORM_NO_DEBUG"), OFF},
#endif
#ifdef TINYORM_NO_DEBUG_SQL
        {sl("TINYORM_NO_DEBUG_SQL"), ON},
#else
        {sl("TINYORM_NO_DEBUG_SQL"), OFF},
#endif
// CMake ON/OFF
#ifdef TINYORM_STRICT_MODE
        {sl("TINYORM_STRICT_MODE"), TINY_MACRO_BOOL(TINYORM_STRICT_MODE)},
#endif
// Newline needed - QtCreator syntax highlighting bug
#ifdef TINYORM_TESTS_CODE
        {sl("TINYORM_TESTS_CODE"), ON},
#else
        {sl("TINYORM_TESTS_CODE"), OFF},
#endif
#ifdef TINYORM_TOM_EXAMPLE
        {sl("TINYORM_TOM_EXAMPLE"), ON},
#else
        {sl("TINYORM_TOM_EXAMPLE"), OFF},
#endif
#ifdef TINYORM_USING_PCH
        {sl("TINYORM_USING_PCH"), ON},
#else
        {sl("TINYORM_USING_PCH"), OFF},
#endif
// CMake ON/OFF/NOTFOUND (TriState bool)
#ifdef TINYORM_MSVC_RUNTIME_DYNAMIC
        {sl("TINYORM_MSVC_RUNTIME_DYNAMIC"),
                    TypeUtils::normalizeCMakeTriStateBool(
                        TINY_STRINGIFY(TINYORM_MSVC_RUNTIME_DYNAMIC))},
#endif
    };
}

std::map<QString, QString> LibraryInfo::tomCMacrosMap()
{
    return {
#ifdef TINYTOM_CMAKE_MSVC_RUNTIME_LIBRARY
        {sl("TINYTOM_CMAKE_MSVC_RUNTIME_LIBRARY"),
                    TINY_STRINGIFY(TINYTOM_CMAKE_MSVC_RUNTIME_LIBRARY)},
#endif
        {sl("TINYTOM_MIGRATIONS_DIR"), TINY_STRINGIFY(TINYTOM_MIGRATIONS_DIR)},
        {sl("TINYTOM_MODELS_DIR"),     TINY_STRINGIFY(TINYTOM_MODELS_DIR)},
        {sl("TINYTOM_SEEDERS_DIR"),    TINY_STRINGIFY(TINYTOM_SEEDERS_DIR)},
    };
}

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE
