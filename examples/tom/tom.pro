QT *= core sql
QT -= gui

TEMPLATE = app
TARGET = tom

# TinyTom example application specific configuration
# ---

CONFIG *= cmdline

include($$TINYORM_SOURCE_TREE/qmake/tom.pri)

# TinyTom example application defines
# ---

DEFINES += PROJECT_TOMEXAMPLE

# TinyTom defines
# ---
# this define is not provided in the qmake/tom.pri

# Enable code needed by tests (modify the migrate:status command for tests need)
build_tests: \
    DEFINES *= TINYTOM_TESTS_CODE

# TinyTom example application header and source files
# ---

SOURCES += $$PWD/main.cpp

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
