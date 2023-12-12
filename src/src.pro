QT *= core sql
QT -= gui

TEMPLATE = lib
TARGET = TinyOrm

# Version requirements
# ---
# Older versions may work, but you are on your own
# Req - requirement, throws error
# Rec - recommended, shows info message

# 16.10/16.11 (1929) - to support #pragma system_header
tinyMinReqMsvc    = 19.29
tinyMinReqClangCl = 14.0.3
tinyMinRecClang   = 12
tinyMinRecGCC     = 10.2
tinyMinRecQt      = 5.15.2

# Make minimum toolchain version a requirement
load(tiny_toolchain_requirement)

# TinyORM library specific configuration
# ---

CONFIG *= create_prl create_pc create_libtool

# Common Configuration
# ---

include(../qmake/common.pri)

# TinyORM defines
# ---

DEFINES += PROJECT_TINYORM

# Build as the shared library
CONFIG(shared, dll|shared|static|staticlib) | \
CONFIG(dll, dll|shared|static|staticlib): \
    DEFINES *= TINYORM_BUILDING_SHARED

# TinyORM library header and source files
# ---

# tiny_version_numbers() depends on HEADERS (version.hpp)
include(../include/include.pri)
include(src.pri)

# TinyTom header and source files
# ---

!disable_tom {
    include(../tom/include/include.pri)
    include(../tom/src/src.pri)
}

# File version
# ---

# Find version numbers in the version header file and assign them to the
# <TARGET>_VERSION_<MAJOR,MINOR,PATCH,TWEAK> and also to the VERSION variable.
load(tiny_version_numbers)
tiny_version_numbers()

# Windows resource and manifest files
# ---

load(tiny_resource_and_manifest)
tiny_resource_and_manifest(                                                            \
    # RC_INCLUDEPATH - find icons, Windows manifest on MinGW, and orm/version.hpp
    # These two paths are not divided as in other tiny_resource_and_manifest() calls
    # because of finding icons on win32 (revisited).
    $$quote($$TINYORM_SOURCE_TREE/include/) $$quote($$TINYORM_SOURCE_TREE/resources/), \
    ../resources                                                                       \
)

# Use Precompiled headers (PCH)
# ---

include(../include/pch.pri)

# Deployment
# ---

win32-msvc:CONFIG(debug, debug|release) {
    win32-msvc: target.path = C:/optx64/$${TARGET}
#    else: unix:!android: target.path = /opt/$${TARGET}/bin
    !isEmpty(target.path): INSTALLS += target
}

# Some info output
# ---

load(tiny_info_messages)
tiny_log_info_messages()

!build_pass {
    !disable_orm: \
        message( "Build ORM-related source code." )
    else: \
        message( "Disabled ORM-related source code (build the query builder only)." )

    mysql_ping: \
        message( "Enabled MySQL ping on Orm::MySqlConnection." )
}

# Auto-configuration
# ---

!disable_autoconf {
    load(tiny_find_packages)

    # Find the vcpkg and add the vcpkg/<triplet>/include/ on the system include path
    tiny_find_vcpkg()

    # Find the MySQL and add it on the system include path and library path
    mysql_ping: tiny_find_mysql()
}

# User Configuration
# ---

exists(../conf.pri): \
    include(../conf.pri)

else:disable_autoconf: \
    error( "'conf.pri' for '$${TARGET}' project does not exist.\
            See an example configuration in 'conf.pri.example'." )
