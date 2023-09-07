QT -= gui

TEMPLATE = app
TARGET = tom

# Link against TinyORM library for Tom application (also adds defines and include headers)
# ---

include($$TINYORM_SOURCE_TREE/qmake/tom.pri)

# TinyTom example application specific configuration
# ---

CONFIG *= cmdline

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

# User Configuration
# ---

exists(conf.pri): \
    include(conf.pri)

else:disable_autoconf: \
    error( "'conf.pri' for '$${TARGET}' example project does not exist.\
            See an example configuration in 'examples/tom/conf.pri.example'." )
