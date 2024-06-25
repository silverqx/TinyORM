QT *= core
QT -= gui

TEMPLATE = lib
TARGET = TinyOrm

# Common for static/shared libraries
# ---

include($$TINYORM_SOURCE_TREE/qmake/common/libraries.pri)

# Common Configuration
# ---

include($$TINYORM_SOURCE_TREE/qmake/common/TinyOrm.pri)

# TinyORM defines
# ---

DEFINES *= PROJECT_TINYORM

# Build as the shared library
CONFIG(shared, dll|shared|static|staticlib) | \
CONFIG(dll, dll|shared|static|staticlib): \
    DEFINES *= TINYORM_BUILDING_SHARED

# TinyORM library header and source files
# ---

# tiny_version_numbers() depends on HEADERS (version.hpp)
include($$TINYORM_SOURCE_TREE/include/include.pri)
include($$PWD/src.pri)

# TinyTom header and source files
# ---

!disable_tom {
    include($$TINYORM_SOURCE_TREE/tom/include/include.pri)
    include($$TINYORM_SOURCE_TREE/tom/src/src.pri)
}

# File version
# ---

# Find version numbers in the version header file and assign them to the
# <TARGET>_VERSION_<MAJOR,MINOR,PATCH,TWEAK> and also to the VERSION variable.
load(private/tiny_version_numbers)
tiny_version_numbers()

# Windows resource and manifest files
# ---

load(private/tiny_resource_and_manifest)
tiny_resource_and_manifest(                                                            \
    # RC_INCLUDEPATH - find icons, Windows manifest on MinGW, and orm/version.hpp and stringify.hpp
    # These two paths are not divided as in other tiny_resource_and_manifest() calls
    # because of finding icons on win32 (revisited).
    $$quote($$TINYORM_SOURCE_TREE/include/) $$quote($$TINYORM_SOURCE_TREE/resources/), \
    ../resources                                                                       \
)

# Use Precompiled headers (PCH)
# ---

include($$TINYORM_SOURCE_TREE/include/pch.pri)

# Deployment
# ---

win32-msvc:CONFIG(debug, debug|release) {
    win32-msvc: target.path = C:/optx64/$${TARGET}
#    else: unix:!android: target.path = /opt/$${TARGET}/bin
    !isEmpty(target.path): INSTALLS += target
}

# Some info output
# ---

load(private/tiny_info_messages)
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
    load(private/tiny_find_packages)

    # Find the vcpkg and add the vcpkg/<triplet>/include/ on the system include path
    tiny_find_vcpkg()

    # Find the MySQL and add it on the system include path and library path
    mysql_ping: tiny_find_mysql()
}

# User Configuration
# ---

exists($$TINYORM_SOURCE_TREE/conf.pri): \
    include($$TINYORM_SOURCE_TREE/conf.pri)

else:disable_autoconf: \
    error( "'conf.pri' for '$${TARGET}' project does not exist.\
            See an example configuration in 'conf.pri.example'." )
