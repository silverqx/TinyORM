QT -= gui

TEMPLATE = app
TARGET = tom

# Common for all executables
# ---

include($$TINYORM_SOURCE_TREE/qmake/common/executables.pri)

# Link against TinyORM library for Tom application (also adds defines and include headers)
# ---

include($$TINYORM_SOURCE_TREE/qmake/tom.pri)

# TinyTom example application specific configuration
# ---

CONFIG *= cmdline

# TinyTom example application defines
# ---

DEFINES *= PROJECT_TOM_EXAMPLE

# TinyTom example application header and source files
# ---

SOURCES += $$PWD/main.cpp

# Migrations and Seeders header files
# ---

# Tom example migrations
include($$TINYORM_SOURCE_TREE/tests/database/migrations.pri)
# Or include yours migrations
#include(/home/xyz/your_project/database/migrations.pri)

# Tom example seeders
include($$TINYORM_SOURCE_TREE/tests/database/seeders.pri)
# Or include yours seeders
#include(/home/xyz/your_project/database/seeders.pri)

# User Configuration
# ---

exists($$PWD/conf.pri): \
    include($$PWD/conf.pri)

else:disable_autoconf: \
    error( "'conf.pri' for '$${TARGET}' example project does not exist.\
            See an example configuration in 'examples/tom/conf.pri.example'." )
