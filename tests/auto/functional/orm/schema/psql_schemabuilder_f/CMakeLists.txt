add_executable(psql_schemabuilder_f
    tst_postgresql_schemabuilder_f.cpp
)

add_test(NAME psql_schemabuilder_f COMMAND psql_schemabuilder_f)

tiny_configure_test(psql_schemabuilder_f DEPENDS_ON_UNITTESTS)
