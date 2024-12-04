add_executable(querybuilder)

target_sources(querybuilder PRIVATE tst_querybuilder.cpp)

add_test(NAME querybuilder COMMAND querybuilder)

tiny_configure_test(querybuilder DEPENDS_ON_UNITTESTS RUN_SERIAL)
