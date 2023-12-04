QT *= testlib
QT -= gui

TEMPLATE = app

# Link against TinyORM library (also adds defines and include headers)
# ---

include($$TINYORM_SOURCE_TREE/qmake/TinyOrm.pri)

# Tests specific configuration
# ---

CONFIG *= cmdline testcase

# Compiler and Linker options
# ---

win32-g++ {
    QMAKE_CXXFLAGS_DEBUG += -Wa,-mbig-obj
    # Avoid string table overflow
    QMAKE_CXXFLAGS_DEBUG += -O1
}

# BUG qmake, in Makefile.Debug during debug\moc_predefs.h in all tests silverqx
win32-clang-msvc: \
    QMAKE_CXXFLAGS_WARN_ON -= -WX

# Tests defines
# ---

DEFINES *= PROJECT_TINYORM_TEST

# Windows resource and manifest files
# ---

# Find Windows manifest
mingw: tinyRcIncludepath += $$quote($$TINYORM_SOURCE_TREE/tests/resources/)
else:  tinyRcIncludepath =

load(tiny_resource_and_manifest)
tiny_resource_and_manifest($$tinyRcIncludepath,     \
    $$quote($$TINYORM_SOURCE_TREE/tests/resources), \
    TinyTest                                        \
)

unset(tinyRcIncludepath)

# Use TinyORM's library precompiled headers (PCH)
# ---

include($$TINYORM_SOURCE_TREE/include/pch.pri)

# Default rules for deployment
# ---

target.CONFIG += no_default_install

# User Configuration
# ---

exists($$TINYORM_SOURCE_TREE/tests/conf.pri): \
    include($$TINYORM_SOURCE_TREE/tests/conf.pri)

else:disable_autoconf: \
    error( "'tests/conf.pri' for 'tests' project does not exist.\
            See an example configuration in 'tests/conf.pri.example'." )
