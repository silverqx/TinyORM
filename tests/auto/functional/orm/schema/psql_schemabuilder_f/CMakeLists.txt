project(postgresql_schemabuilder_f
    LANGUAGES CXX
)

add_executable(psql_schemabuilder_f
    tst_postgresql_schemabuilder_f.cpp
)

add_test(NAME psql_schemabuilder_f COMMAND psql_schemabuilder_f)

include(TinyTestCommon)
tiny_configure_test(psql_schemabuilder_f DEPENDS_ON_UNITTESTS)
