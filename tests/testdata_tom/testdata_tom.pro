QT *= core sql
QT -= gui

TEMPLATE = app
TARGET = tom_testdata

# TinyTom migrations for unit tests specific configuration
# ---

CONFIG *= console

include($$TINYORM_SOURCE_TREE/qmake/tom.pri)

# TinyTom migrations for unit tests defines
# ---

DEFINES += PROJECT_TOM_TESTDATA

# TinyTom defines
# ---
# this define is not provided in the qmake/tom.pri

# Another thing is that this project is build only if the build_tests qmake config option
# is defined, but I leave this condition below because this logic is ok and there can be
# edge cases like building the testdata_tom project stand-alone or whatever

# Enable code needed by tests (modify the migrate:status command for tests need)
build_tests: \
    DEFINES *= TINYTOM_TESTS_CODE

# TinyTom migrations for unit tests header and source files
# ---

SOURCES += $$PWD/main.cpp

# Migrations header files
include($$PWD/database/migrations.pri)

# Deployment
# ---

win32-msvc:CONFIG(debug, debug|release) {
    win32-msvc: target.path = C:/optx64/$${TARGET}
#    else: unix:!android: target.path = /opt/$${TARGET}/bin
    !isEmpty(target.path): INSTALLS += target
}

# User Configuration
# ---

exists(conf.pri): \
    include(conf.pri)

#else:is_vcpkg_build: \
#    include(../qmake/vcpkgconf.pri)

else: \
    error( "'conf.pri' for 'tom' example project does not exist. See an example\
            configuration in 'examples/tom/conf.pri.example' or call 'vcpkg install'\
            in the project's root." )
