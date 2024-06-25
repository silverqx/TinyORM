include($$TINYORM_SOURCE_TREE/tests/qmake/common.pri)
include($$TINYORM_SOURCE_TREE/tests/qmake/TinyUtils.pri)

# No need to link against to QtSql module
QT -= sql

SOURCES = tst_versions.cpp

tom_example:!disable_tom: \
    DEFINES *= TINYTOM_EXAMPLE

# Used by checkFileVersion_*() tests
win32 {
    DEFINES *= TINYTEST_VERSIONS_IS_QMAKE

    CONFIG(shared, dll|shared|static|staticlib) | \
    CONFIG(dll, dll|shared|static|staticlib): \
        DEFINES *= TINYTEST_VERSIONS_IS_SHARED_BUILD

    # File versions
    # ---

    # Find version numbers in the version header files and assign them to the
    # <TARGET>_VERSION_<MAJOR,MINOR,PATCH,TWEAK> (excluding the VERSION variable).
    load(private/tiny_version_numbers)
    build_shared_drivers | \
    build_loadable_drivers: tiny_version_numbers(, TinyDrivers, false)
    build_loadable_drivers: tiny_version_numbers(, TinyMySql, false)
    tiny_version_numbers(, TinyOrm, false)
    tiny_version_numbers(, TinyUtils, false)

    # Filenames
    tinyDriversFilename = TinyDrivers$${TinyDrivers_VERSION_MAJOR}.dll
    tinyMySqlFilename   = TinyMySql$${TinyMySql_VERSION_MAJOR}.dll
    tinyOrmFilename     = TinyOrm$${TinyOrm_VERSION_MAJOR}.dll
    tinyUtilsFilename   = TinyUtils$${TinyUtils_VERSION_MAJOR}.dll

    # Filepaths to dll/exe-es
    TINYTEST_VERSIONS_TINYDRIVERS_PATH = \
        $$quote($${TINYORM_BUILD_TREE}/drivers/common$${TINY_BUILD_SUBFOLDER}/$$tinyDriversFilename)
    TINYTEST_VERSIONS_TINYMYSQL_PATH = \
        $$quote($${TINYORM_BUILD_TREE}/drivers/mysql$${TINY_BUILD_SUBFOLDER}/$$tinyMySqlFilename)
    TINYTEST_VERSIONS_TINYORM_PATH = \
        $$quote($${TINYORM_BUILD_TREE}/src$${TINY_BUILD_SUBFOLDER}/$$tinyOrmFilename)
    TINYTEST_VERSIONS_TINYUTILS_PATH = \
        $$quote($${TINYORM_BUILD_TREE}/tests/TinyUtils$${TINY_BUILD_SUBFOLDER}/$$tinyUtilsFilename)
    TINYTEST_VERSIONS_TOMEXAMPLE_PATH = \
        $$quote($${TINYORM_BUILD_TREE}/examples/tom$${TINY_BUILD_SUBFOLDER}/tom.exe)

    QMAKE_SUBSTITUTES += $$quote(include/versionsdebug_qmake.hpp.in)

    HEADERS += $$quote($$OUT_PWD/include/versionsdebug_qmake.hpp)

    INCLUDEPATH += $$quote($$OUT_PWD/include/)

    # To find tom/include/version.hpp (don't need to include whole qmake/tom.pri).
    # INCLUDEPATH += tom/include/ is added in the TinyOrm.pri

    LIBS += -lVersion
}

# No need to link against the TinyOrm library
LIBS -= -lTinyOrm
