project(postgresql_schemabuilder_f
    LANGUAGES CXX
)

add_executable(postgresql_schemabuilder_f
    tst_schemabuilder.cpp
)

add_test(NAME postgresql_schemabuilder_f COMMAND postgresql_schemabuilder_f)

include(TinyTestCommon)
tiny_configure_test(postgresql_schemabuilder_f)
