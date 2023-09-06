# The tom.pri file is available to simplify the integration of the TinyORM library
# into the Tom migrations application.
#
# It sets up and configures the CONFIG and DEFINES qmake variables, adds the TinyORM, Tom,
# and vcpkg header files on the system INCLUDEPATH (cross-platform using the -isystem or
# -imsvc), links against the TinyORM shared or static library using the LIBS.
#
# Requirements:
#
# You must build your tom application with the same CONFIG qmake variables that were used
# when building the TinyORM library.
#
# You must define the following variables before the tom.pri will be included:
#
# - TINYORM_BUILD_TREE - Path to the TinyORM build folder.
# - TINY_VCPKG_ROOT    - Path to the vcpkg installation folder.
#                        If not defined, then it tries to use the VCPKG_ROOT environment
#                        variable.
# - TINY_VCPKG_TRIPLET - The vcpkg triplet to use (vcpkg/installed/$$TINY_VCPKG_TRIPLET/).
#                        If not defined, then it tries to guess the vcpkg triplet based
#                        on the current compiler and OS (based on the QMAKESPEC), and
#                        as the last thing, it tries to use the VCPKG_DEFAULT_TRIPLET
#                        environment variable.
#
# These variables will be set after the configuration is done:
#
# - TINY_BUILD_SUBFOLDER - Folder by release type (/debug, /release, or empty).
# - TINY_VCPKG_INCLUDE   - Path to the vcpkg include folder (vcpkg/installed/<triplet>/include/).

# Path to the TinyORM source tree
TINYORM_SOURCE_TREE = $$clean_path($$quote($$PWD/..))
# Path to the Tom source tree
TINYTOM_SOURCE_TREE = $$quote($$TINYORM_SOURCE_TREE/tom)

# Qt Common Configuration
# ---

QT *= core sql

CONFIG *= link_prl

include($$TINYORM_SOURCE_TREE/qmake/common.pri)

# Configure TinyORM library
# ---
# everything else is defined in the qmake/common.pri

# Link against the shared library
CONFIG(shared, dll|shared|static|staticlib) | \
CONFIG(dll, dll|shared|static|staticlib): \
    DEFINES *= TINYORM_LINKING_SHARED

# Disable the ORM-related source code
disable_orm: DEFINES *= TINYORM_DISABLE_ORM
# Forbid disabling the tom-related source code and header files
disable_tom: \
    error( "Disabling the tom is forbidden in the tom.pri, please remove\
            the 'disable_tom' configuration." )

# File version
# ---

# Find version numbers in the version header file and assign them to the
# <TARGET>_VERSION_<MAJOR,MINOR,PATCH,TWEAK> and also to the VERSION variable.
load(tiny_version_numbers)
tiny_version_numbers($$quote($$TINYTOM_SOURCE_TREE/include/tom/version.hpp))

# Windows resource and manifest files
# ---

# To find version.hpp
tinyRcIncludepath = $$quote($$TINYTOM_SOURCE_TREE/include/)
# To find Windows manifest
mingw: tinyRcIncludepath += $$quote($$TINYTOM_SOURCE_TREE/resources/)

load(tiny_resource_and_manifest)
tiny_resource_and_manifest(                                               \
    $$tinyRcIncludepath, $$TINYTOM_SOURCE_TREE/resources, tom, Tom        \
)

unset(tinyRcIncludepath)

# Link against TinyORM library
# ---

load(tiny_system_includepath)

tiny_add_system_includepath(                   \
    $$quote($$TINYORM_SOURCE_TREE/include/)    \
    $$quote($$TINYTOM_SOURCE_TREE/include/)    \
)

# Don't check if exists() because QtCreator depends on these LIBS paths it adds them
# on the PATH or LD_LIBRARY_PATH during Run Project
!isEmpty(TINYORM_BUILD_TREE) {
    LIBS += $$quote(-L$$clean_path($$TINYORM_BUILD_TREE)/src$${TINY_BUILD_SUBFOLDER}/)
    LIBS += -lTinyOrm
}

# vcpkg - range-v3 and tabulate
# ---
# Affected by (in the following order):
# TINY_VCPKG_ROOT and TINY_VCPKG_TRIPLET qmake variables
# TINY_VCPKG_ROOT and TINY_VCPKG_TRIPLET environment variables
# VCPKG_ROOT and VCPKG_DEFAULT_TRIPLET environment variables

load(tiny_find_packages)

tiny_find_vcpkg()

# Some info output
# ---

# The TINYORM_BUILD_TREE is super important, so print some warning if isn't defined,
# can be disabled using CONFIG*=no_tinyorm_link_warning
!build_pass: \
!no_tinyorm_link_warning: \
if(!defined(TINYORM_BUILD_TREE, var)|isEmpty(TINYORM_BUILD_TREE)): \
    warning( "The 'TINYORM_BUILD_TREE' qmake variable to the TinyORM build tree was not\
              defined, please define it or link against the TinyOrm library manually\
              using the 'LIBS' qmake variable. $$TARGET" )
