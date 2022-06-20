project(sqlite_schemabuilder
    LANGUAGES CXX
)

add_executable(sqlite_schemabuilder
    tst_sqlite_schemabuilder.cpp
)

add_test(NAME sqlite_schemabuilder COMMAND sqlite_schemabuilder)

include(TinyTestCommon)
if(ORM)
    tiny_configure_test(sqlite_schemabuilder INCLUDE_MODELS)
else()
    tiny_configure_test(sqlite_schemabuilder)
endif()
