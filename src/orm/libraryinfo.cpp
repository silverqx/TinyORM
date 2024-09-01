#include "orm/libraryinfo.hpp"

#include "orm/constants.hpp"
#include "orm/macros/archdetect.hpp"
#include "orm/macros/compilerdetect.hpp"
#include "orm/utils/type.hpp"
#include "orm/version.hpp"

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

using namespace Qt::StringLiterals; // NOLINT(google-build-using-namespace)

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
        {u"TINYORM_BUILDING_SHARED"_s, ON},
#else
        {u"TINYORM_BUILDING_SHARED"_s, OFF},
#endif
#ifdef TINYORM_DEBUG
        {u"TINYORM_DEBUG"_s, ON},
#else
        {u"TINYORM_DEBUG"_s, OFF},
#endif
#ifdef TINYORM_DEBUG_SQL
        {u"TINYORM_DEBUG_SQL"_s, ON},
#else
        {u"TINYORM_DEBUG_SQL"_s, OFF},
#endif
// Newline needed - QtCreator syntax highlighting bug
#ifdef TINYORM_DISABLE_ORM
        {u"TINYORM_DISABLE_ORM"_s, ON},
#else
        {u"TINYORM_DISABLE_ORM"_s, OFF},
#endif
#ifdef TINYORM_DISABLE_THREAD_LOCAL
        {u"TINYORM_DISABLE_THREAD_LOCAL"_s, ON},
#else
        {u"TINYORM_DISABLE_THREAD_LOCAL"_s, OFF},
#endif
#ifdef TINYORM_DISABLE_TOM
        {u"TINYORM_DISABLE_TOM"_s, ON},
#else
        {u"TINYORM_DISABLE_TOM"_s, OFF},
#endif
#ifdef TINYORM_EXTERN_CONSTANTS
        {u"TINYORM_EXTERN_CONSTANTS"_s, ON},
#else
        {u"TINYORM_EXTERN_CONSTANTS"_s, OFF},
#endif
#ifdef TINYORM_INLINE_CONSTANTS
        {u"TINYORM_INLINE_CONSTANTS"_s, ON},
#else
        {u"TINYORM_INLINE_CONSTANTS"_s, OFF},
#endif
// CMake ON/OFF
#ifdef TINYORM_LTO
        {u"TINYORM_LTO"_s, TINY_MACRO_BOOL(TINYORM_LTO)},
#endif
#ifdef TINYORM_MYSQL_PING
        {u"TINYORM_MYSQL_PING"_s, ON},
#else
        {u"TINYORM_MYSQL_PING"_s, OFF},
#endif
// Newline needed - QtCreator syntax highlighting bug
#ifdef TINYORM_NO_DEBUG
        {u"TINYORM_NO_DEBUG"_s, ON},
#else
        {u"TINYORM_NO_DEBUG"_s, OFF},
#endif
#ifdef TINYORM_NO_DEBUG_SQL
        {u"TINYORM_NO_DEBUG_SQL"_s, ON},
#else
        {u"TINYORM_NO_DEBUG_SQL"_s, OFF},
#endif
// CMake ON/OFF
#ifdef TINYORM_STRICT_MODE
        {u"TINYORM_STRICT_MODE"_s, TINY_MACRO_BOOL(TINYORM_STRICT_MODE)},
#endif
// Newline needed - QtCreator syntax highlighting bug
#ifdef TINYORM_TESTS_CODE
        {u"TINYORM_TESTS_CODE"_s, ON},
#else
        {u"TINYORM_TESTS_CODE"_s, OFF},
#endif
#ifdef TINYORM_TOM_EXAMPLE
        {u"TINYORM_TOM_EXAMPLE"_s, ON},
#else
        {u"TINYORM_TOM_EXAMPLE"_s, OFF},
#endif
#ifdef TINYORM_USING_PCH
        {u"TINYORM_USING_PCH"_s, ON},
#else
        {u"TINYORM_USING_PCH"_s, OFF},
#endif
// CMake ON/OFF/NOTFOUND (TriState bool)
#ifdef TINYORM_MSVC_RUNTIME_DYNAMIC
        {u"TINYORM_MSVC_RUNTIME_DYNAMIC"_s,
                    TypeUtils::normalizeCMakeTriStateBool(
                        TINY_STRINGIFY(TINYORM_MSVC_RUNTIME_DYNAMIC))},
#endif
    };
}

std::map<QString, QString> LibraryInfo::tomCMacrosMap()
{
    return {
#ifdef TINYTOM_CMAKE_MSVC_RUNTIME_LIBRARY
        {u"TINYTOM_CMAKE_MSVC_RUNTIME_LIBRARY"_s,
                    TINY_STRINGIFY(TINYTOM_CMAKE_MSVC_RUNTIME_LIBRARY)},
#endif
        {u"TINYTOM_MIGRATIONS_DIR"_s, TINY_STRINGIFY(TINYTOM_MIGRATIONS_DIR)},
        {u"TINYTOM_MODELS_DIR"_s,     TINY_STRINGIFY(TINYTOM_MODELS_DIR)},
        {u"TINYTOM_SEEDERS_DIR"_s,    TINY_STRINGIFY(TINYTOM_SEEDERS_DIR)},
    };
}

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE
