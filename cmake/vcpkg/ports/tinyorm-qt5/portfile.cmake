vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO silverqx/TinyORM
    REF ca8909896247b21bf08d62a5109b23e9f65c89e1
    SHA512 ac0e3e91354a5c12471924a34fa24e4c040125614e1117da1ae50916c70630d217a7df470c1b929aa15f3a6acc6b511ddea808a9f2d6e675f05afa8a2a764e2c
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
        -DMATCH_EQUAL_EXPORTED_BUILDTREE:BOOL=OFF
        -DTINY_PORT:STRING=${PORT}
        -DTINY_VCPKG:BOOL=ON
        -DVERBOSE_CONFIGURE:BOOL=ON
        ${FEATURE_OPTIONS}
    OPTIONS_RELEASE
        -DTINY_TARGET_TRIPLET:STRING=${TARGET_TRIPLET}
)

vcpkg_cmake_install()

vcpkg_cmake_config_fixup()

# https://github.com/microsoft/vcpkg/issues/33551
if(WIN32 AND TINYORM_TOM_EXAMPLE)
    vcpkg_copy_tools(TOOL_NAMES tom AUTO_CLEAN)
endif()
