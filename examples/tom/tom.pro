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

DEFINES += PROJECT_TOM_EXAMPLE

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
