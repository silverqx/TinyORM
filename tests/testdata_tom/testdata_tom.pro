QT -= gui

TEMPLATE = app
TARGET = tom_testdata

# Common for all executables
# ---

include($$TINYORM_SOURCE_TREE/qmake/common/executables.pri)

# Link against TinyORM library for Tom application (also adds defines and include headers)
# ---

include($$TINYORM_SOURCE_TREE/qmake/tom.pri)

# TinyTom migrations for unit tests specific configuration
# ---

CONFIG *= cmdline

# TinyTom migrations for unit tests defines
# ---

DEFINES *= PROJECT_TOM_TESTDATA

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

# User Configuration
# ---

exists($$PWD/conf.pri): \
    include($$PWD/conf.pri)

else:disable_autoconf: \
    error( "'conf.pri' for 'tests/$${TARGET}' example project does not exist.\
             See an example configuration in 'tests/testdata_tom/conf.pri.example'." )
