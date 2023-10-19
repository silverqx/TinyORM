vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO silverqx/TinyORM
    REF v0.36.0
    SHA512 89483c1e78aaaff029d356204c638ee3ef5f24c1400fe2a486c5f77c45e3c16a3dbd5cf7e7221982efb5cac79f191d0a18b9e9fc2073838d3f56f27a0fd1e238
    HEAD_REF main
)

vcpkg_check_features(
    OUT_FEATURE_OPTIONS FEATURE_OPTIONS
    PREFIX TINYORM
    FEATURES
        disable-thread-local DISABLE_THREAD_LOCAL
        inline-constants     INLINE_CONSTANTS
        mysql-ping           MYSQL_PING
        orm                  ORM
        strict-mode          STRICT_MODE
        tom                  TOM
        tom-example          TOM_EXAMPLE
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        -DCMAKE_EXPORT_PACKAGE_REGISTRY:BOOL=OFF
        -DBUILD_TESTS:BOOL=OFF
        -DTINY_PORT:STRING=${PORT}
        -DTINY_VCPKG:BOOL=ON
        -DVERBOSE_CONFIGURE:BOOL=ON
        ${FEATURE_OPTIONS}
    OPTIONS_RELEASE
        -DTINY_TARGET_TRIPLET:STRING=${TARGET_TRIPLET}
)

vcpkg_cmake_install()

vcpkg_cmake_config_fixup()

if(TINYORM_TOM_EXAMPLE)
    vcpkg_copy_tools(TOOL_NAMES tom AUTO_CLEAN)
endif()
