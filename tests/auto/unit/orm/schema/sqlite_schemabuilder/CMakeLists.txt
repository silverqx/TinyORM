add_executable(sqlite_schemabuilder)

target_sources(sqlite_schemabuilder PRIVATE tst_sqlite_schemabuilder.cpp)

add_test(NAME sqlite_schemabuilder COMMAND sqlite_schemabuilder)

if(ORM)
    tiny_configure_test(sqlite_schemabuilder INCLUDE_MODELS)
else()
    tiny_configure_test(sqlite_schemabuilder)
endif()
