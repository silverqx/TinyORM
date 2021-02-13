QT -= gui

TEMPLATE = lib

# Utils library specific configuration
# ---

CONFIG *= qt link_prl tinyorm_utils

# Common configuration
# ---

include(../../../config.pri)

# Utils library defines
# ---

# Build as shared library
DEFINES += UTILS_BUILDING_SHARED

# File version and windows manifest
# ---

win32:VERSION = 0.1.0.0
else:VERSION = 0.1.0

win32-msvc* {
    QMAKE_TARGET_PRODUCT = Utils
    QMAKE_TARGET_DESCRIPTION = Utils library for TinyORM tests
    QMAKE_TARGET_COMPANY = Crystal Studio
    QMAKE_TARGET_COPYRIGHT = Copyright (©) 2020 Crystal Studio
#    RC_ICONS = images/utils.ico
    RC_LANG = 1033
}

# Use Precompiled headers (PCH)
# ---

PRECOMPILED_HEADER = $$quote($$PWD/src/pch.h)

precompile_header:!isEmpty(PRECOMPILED_HEADER) {
    DEFINES += USING_PCH
}

HEADERS += $$quote($$PWD/src/pch.h)

# TinyORM library headers include path
# ---

include(../../../include/include.pri)

# Link against TinyORM library
# ---

win32:CONFIG(release, debug|release) {
    LIBS += -L$$TINYORM_BUILD_TREE/src/release/ -lTinyOrm0
}
else:win32:CONFIG(debug, debug|release) {
    LIBS += -L$$TINYORM_BUILD_TREE/src/debug/ -lTinyOrm0
}
else:unix {
    LIBS += -L$$TINYORM_BUILD_TREE/src/ -lTinyOrm0
}

# Utils library header and source files
# ---

include(src/src.pri)

# Default rules for deployment
# ---

release {
    target.CONFIG += no_default_install
}
