vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO silverqx/TinyORM
    REF 03bbe98d6740720060506bbf345e4fa9c3d5a433
    SHA512 3e3cd94cd88e1f56ebb0d7cd1454332b7b92cc839a1916161627e83ce2a8fcddf9699e190bfe3d1ee7d97b88c076027f8fd7aa0619214e7098540edc60667ea5
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
    PREFER_NINJA
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

# Install header files and license
file(INSTALL "${SOURCE_PATH}/include/"
    DESTINATION "${CURRENT_PACKAGES_DIR}/include"
    FILES_MATCHING PATTERN *.hpp
)
file(INSTALL "${SOURCE_PATH}/LICENSE"
    DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}"
    RENAME copyright
)

configure_file("${CURRENT_PORT_DIR}/usage"
    "${CURRENT_PACKAGES_DIR}/share/${PORT}/usage"
    @ONLY
)
