# Path to the TinyORM source tree
TINYORM_SOURCE_TREE = $$clean_path($$quote($$PWD/..))
# Path to the Tom source tree
TINYTOM_SOURCE_TREE = $$quote($$TINYORM_SOURCE_TREE/tom)

# Also Provides:
# - TINY_BUILD_SUBFOLDER - folder by release type (/debug, /release, or empty)

# Qt Common Configuration
# ---

QT *= core sql

CONFIG *= link_prl

include($$TINYORM_SOURCE_TREE/qmake/common.pri)

# Configure TinyORM library
# ---
# everything else is defined in the qmake/common.pri

# Link against the shared library
CONFIG(shared, dll|shared|static|staticlib) | \
CONFIG(dll, dll|shared|static|staticlib): \
    DEFINES *= TINYORM_LINKING_SHARED

# Disable the ORM-related source code
disable_orm: DEFINES *= TINYORM_DISABLE_ORM
# Disable the tom-related source code (also header files)
disable_tom: DEFINES *= TINYORM_DISABLE_TOM

# Link against TinyORM library
# ---

!isEmpty(TINYORM_SOURCE_TREE): \
exists($$TINYORM_SOURCE_TREE) {
    win32-msvc|win32-clang-msvc {
        INCLUDEPATH *= $$quote($$TINYORM_SOURCE_TREE/include/)
        !disable_tom: \
            INCLUDEPATH *= $$quote($$TINYTOM_SOURCE_TREE/include/)
    }
    else {
        QMAKE_CXXFLAGS += -isystem $$shell_quote($$TINYORM_SOURCE_TREE/include/)
        !disable_tom: \
            QMAKE_CXXFLAGS += -isystem $$shell_quote($$TINYTOM_SOURCE_TREE/include/)
    }
}

!isEmpty(TINYORM_BUILD_TREE): \
exists($$TINYORM_BUILD_TREE) {
    LIBS += $$quote(-L$$clean_path($$TINYORM_BUILD_TREE)/src$${TINY_BUILD_SUBFOLDER}/)
    LIBS += -lTinyOrm
}
