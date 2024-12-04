add_executable(schemabuilder)

target_sources(schemabuilder PRIVATE tst_schemabuilder.cpp)

add_test(NAME schemabuilder COMMAND schemabuilder)

tiny_configure_test(schemabuilder DEPENDS_ON_UNITTESTS RUN_SERIAL)
