project(schemabuilder
    LANGUAGES CXX
)

add_executable(schemabuilder
    tst_schemabuilder.cpp
)

add_test(NAME schemabuilder COMMAND schemabuilder)

include(TinyTestCommon)
tiny_configure_test(schemabuilder DEPENDS_ON_UNITTESTS RUN_SERIAL)
