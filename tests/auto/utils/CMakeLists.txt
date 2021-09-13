project(TinyUtils
    DESCRIPTION "Utils library for TinyORM tests"
    LANGUAGES CXX
    VERSION 0.1.0.0
)

set(TinyUtils_target TinyUtils)
add_library(${TinyUtils_target}
    src/databases.hpp
    src/utils_global.hpp
    src/databases.cpp
)
add_library(TinyOrm::${TinyUtils_target} ALIAS TinyUtils)

target_precompile_headers(${TinyUtils_target} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:"pch.h">)

if(NOT CMAKE_DISABLE_PRECOMPILE_HEADERS)
    target_compile_definitions(${TinyUtils_target} PRIVATE USING_PCH)
endif()

set_target_properties(${TinyUtils_target}
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

target_compile_definitions(${TinyUtils_target}
    PRIVATE TINYORM_SQLITE_DATABASE="${tiny_sqlite_db}"
)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(${TinyUtils_target}
        PRIVATE UTILS_BUILDING_SHARED
    )
endif()

target_include_directories(${TinyUtils_target} PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
)

target_link_libraries(${TinyUtils_target}
    PUBLIC
        TinyOrm::TinyOrm
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        TinyOrm::CommonConfig
)
