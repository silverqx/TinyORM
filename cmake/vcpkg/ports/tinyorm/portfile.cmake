vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO silverqx/TinyORM
    REF "v${VERSION}"
    SHA512 231601df0e0b9233e6e206717c8ccbe2431ed545858d7efbbad96c7821177d6103d231941fa1bccae8fd2593b5874969bb4e26089d7502839106488d2cd614b6
    HEAD_REF main
)

# STL4043 _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING already defined, see:
# https://github.com/silverqx/TinyORM/blob/main/cmake/CommonModules/TinyCommon.cmake#L122

vcpkg_check_features(
    OUT_FEATURE_OPTIONS FEATURE_OPTIONS
    PREFIX TINYORM
    FEATURES
        build-mysql-driver   BUILD_MYSQL_DRIVER
        disable-thread-local DISABLE_THREAD_LOCAL
        inline-constants     INLINE_CONSTANTS
        mysql-ping           MYSQL_PING
        orm                  ORM
        strict-mode          STRICT_MODE
        tom                  TOM
        tom-example          TOM_EXAMPLE
)

if(TINYORM_BUILD_MYSQL_DRIVER)
    list(APPEND FEATURE_OPTIONS -DBUILD_DRIVERS:BOOL=ON)
endif()

# To propagate de-vendored requirements because qtbase (QMYSQL) doesn't use unofficial-libmysql
# See: https://github.com/microsoft/vcpkg/issues/38194#issuecomment-2059143576
if(VCPKG_LIBRARY_LINKAGE STREQUAL "static" AND NOT TINYORM_MYSQL_PING AND TINYORM_TOM_EXAMPLE AND
    ("sql-mysql" IN_LIST FEATURES OR TINYORM_BUILD_MYSQL_DRIVER)
)
    list(APPEND FEATURE_OPTIONS -DTINY_VCPKG_QT_WITH_MYSQL:BOOL=ON)
endif()

set(TINY_CMAKE_OPTIONS "")
if("debug" IN_LIST FEATURES)
    list(APPEND TINY_CMAKE_OPTIONS --log-level=DEBUG --log-context)
endif()

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        ${TINY_CMAKE_OPTIONS}
        -DCMAKE_CXX_SCAN_FOR_MODULES:BOOL=OFF
        -DCMAKE_EXPORT_PACKAGE_REGISTRY:BOOL=OFF
        -DBUILD_TESTS:BOOL=OFF
        -DBUILD_TREE_DEPLOY:BOOL=OFF
        -DLEAN_HEADERS:BOOL=ON
        -DTINY_PORT:STRING=${PORT}
        -DTINY_VCPKG:BOOL=ON
        -DVERBOSE_CONFIGURE:BOOL=ON
        ${FEATURE_OPTIONS}
)

vcpkg_cmake_install()

vcpkg_cmake_config_fixup()

if(TINYORM_TOM_EXAMPLE)
    vcpkg_copy_tools(TOOL_NAMES tom AUTO_CLEAN)
endif()

vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")
