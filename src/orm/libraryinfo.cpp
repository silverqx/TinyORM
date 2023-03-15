#include "orm/libraryinfo.hpp"

#include "orm/config.hpp" // IWYU pragma: keep

#include "orm/macros/archdetect.hpp"
#include "orm/macros/compilerdetect.hpp"
#include "orm/version.hpp"

// Build type release/debug
#ifdef TINYORM_NO_DEBUG
#  define DEBUG_STRING " release"
#else
#  define DEBUG_STRING " debug"
#endif

// static/shared
#ifdef TINYORM_BUILDING_SHARED
#  define SHARED_STRING " shared"
#else
#  define SHARED_STRING " static"
#endif

// Full build type string
#define TINYORM_BUILD_STR "TinyORM " TINYORM_VERSION_STR " (" ARCH_FULL SHARED_STRING \
                          DEBUG_STRING " build; by " TINYORM_COMPILER_STRING ")"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{

const char *LibraryInfo::build() noexcept
{
    return TINYORM_BUILD_STR;
}

bool LibraryInfo::isDebugBuild()
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

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE
