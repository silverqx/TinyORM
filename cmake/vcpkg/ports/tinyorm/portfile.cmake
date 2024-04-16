vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO silverqx/TinyORM
    REF v0.36.5
    SHA512 ba3bf73972a6265663122e2c260354cf213dcdcf7bfd1f7a6a7eb43eb11e06fbed581b3f6ce28898eb60a85d0c9bfe45bfaa9596d92b62ca40702ede9856b183
    HEAD_REF main
)

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

# Validate input
if(TINYORM_BUILD_MYSQL_DRIVER AND
    ("sql-mysql" IN_LIST FEATURES OR "sql-psql" IN_LIST FEATURES OR "sql-sqlite" IN_LIST FEATURES)
)
    message(FATAL_ERROR "The build-mysql-driver and sql-mysql, sql-psql, sql-sqlite vcpkg features \
are mutually exclusive, linking against QtSql and TinyDrivers libraries simultaneously is not \
allowed.")
endif()

if(TINYORM_BUILD_MYSQL_DRIVER)
    list(APPEND FEATURE_OPTIONS -DBUILD_DRIVERS:BOOL=ON)
endif()

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        -DCMAKE_EXPORT_PACKAGE_REGISTRY:BOOL=OFF
        # TODO vcpkg ask about this during PR to vcpkg repository silverqx
        -DCMAKE_CXX_SCAN_FOR_MODULES:BOOL=OFF
        -DBUILD_TESTS:BOOL=OFF
        -DBUILD_TREE_DEPLOY:BOOL=OFF
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
