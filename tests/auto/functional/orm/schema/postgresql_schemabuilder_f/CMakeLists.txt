project(postgresql_schemabuilder_f
    LANGUAGES CXX
)

add_executable(postgresql_schemabuilder_f
    tst_postgresql_schemabuilder_f.cpp
)

add_test(NAME postgresql_schemabuilder_f COMMAND postgresql_schemabuilder_f)

include(TinyTestCommon)
tiny_configure_test(postgresql_schemabuilder_f)
