QT *= core sql
QT -= gui

TEMPLATE = app
TARGET = tom_testdata

# TinyTom migrations for unit tests specific configuration
# ---

CONFIG *= cmdline

include($$TINYORM_SOURCE_TREE/qmake/tom.pri)

# TinyTom migrations for unit tests defines
# ---

DEFINES += PROJECT_TOM_TESTDATA

# TinyTom migrations for unit tests header and source files
# ---

SOURCES += $$PWD/main.cpp

# Migrations header files
include($$PWD/database/migrations.pri)
# Seeders header files
include($$PWD/database/seeders.pri)

# Deployment
# ---

win32-msvc:CONFIG(debug, debug|release) {
    win32-msvc: target.path = C:/optx64/$${TARGET}
#    else: unix:!android: target.path = /opt/$${TARGET}/bin
    !isEmpty(target.path): INSTALLS += target
}

# CONFIG tiny_autoconf
# ---

tiny_autoconf {
    load(tiny_find_packages)

    # Find the vcpkg and add the vcpkg/<triplet>/include/ on the system include path
    tiny_find_vcpkg()
}

# User Configuration
# ---

exists(conf.pri): \
    include(conf.pri)

else:!tiny_autoconf: \
    error( "'conf.pri' for '$${TARGET}' example project does not exist. See an example\
            configuration in 'examples/tom/conf.pri.example' or call 'vcpkg install'\
            in the project's root." )
