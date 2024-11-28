project(postgresql_schemabuilder
    LANGUAGES CXX
)

add_executable(postgresql_schemabuilder
    tst_postgresql_schemabuilder.cpp
)

add_test(NAME postgresql_schemabuilder COMMAND postgresql_schemabuilder)

if(ORM)
    tiny_configure_test(postgresql_schemabuilder INCLUDE_MODELS)
else()
    tiny_configure_test(postgresql_schemabuilder)
endif()
