vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO silverqx/TinyORM
    REF 01a834ef3dbb0a9a6b484049a214eda6fb5e9cdb
    SHA512 a8b363d24609671be7addd9ad862be04640efeb8c01b10e49cf68b7e3e0d46c9c34a02e7d8cf2e980136008e1f43b714464f515f77918d4876c7610fff3c189c
    HEAD_REF main
)

vcpkg_check_features(
    OUT_FEATURE_OPTIONS FEATURE_OPTIONS
    PREFIX TINYORM
    FEATURES
        mysqlping MYSQL_PING
)

vcpkg_cmake_configure(
    SOURCE_PATH ${SOURCE_PATH}
    OPTIONS
        -DBUILD_TESTS:BOOL=OFF
        -DMATCH_EQUAL_EXPORTED_BUILDTREE:BOOL=ON
        -DVERBOSE_CONFIGURE:BOOL=ON
        -DTINY_VCPKG:BOOL=ON
        ${FEATURE_OPTIONS}
)

vcpkg_cmake_install()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(tiny_cmake_config_fixup)
tiny_cmake_config_fixup()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/share")

# Install license and usage
file(INSTALL "${SOURCE_PATH}/LICENSE"
    DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}"
    RENAME copyright
)

configure_file("${CURRENT_PORT_DIR}/usage"
    "${CURRENT_PACKAGES_DIR}/share/${PORT}/usage"
    @ONLY
)
