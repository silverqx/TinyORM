project(TinyUtils
    DESCRIPTION "Utils library for TinyORM tests"
    LANGUAGES CXX
    VERSION 0.1.0.0
)

add_library(TinyUtils
    src/databases.hpp
    src/utils_global.hpp
    src/databases.cpp
)
add_library(TinyOrm::TinyUtils ALIAS TinyUtils)

target_precompile_headers(TinyUtils PRIVATE $<$<COMPILE_LANGUAGE:CXX>:"pch.h">)

set_target_properties(TinyUtils
    PROPERTIES
        C_VISIBILITY_PRESET "hidden"
        CXX_VISIBILITY_PRESET "hidden"
        VISIBILITY_INLINES_HIDDEN YES
        VERSION ${PROJECT_VERSION}
        SOVERSION 0
)

# Create an empty SQLite database file when does not exist
set(tiny_sqlite_db "${CMAKE_BINARY_DIR}/tests/q_tinyorm_test_1.sqlite3")
create_sqlite_db("${tiny_sqlite_db}")

target_compile_definitions(TinyUtils
    PRIVATE TINYORM_SQLITE_DATABASE="${tiny_sqlite_db}"
)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(TinyUtils
        PRIVATE UTILS_BUILDING_SHARED
    )
endif()

target_include_directories(TinyUtils PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(TinyUtils
    PUBLIC
        TinyOrm::TinyOrm
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        TinyOrm::CommonConfig
)
