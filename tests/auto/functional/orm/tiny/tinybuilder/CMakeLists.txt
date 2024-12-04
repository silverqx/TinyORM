add_executable(tinybuilder)

target_sources(tinybuilder PRIVATE tst_tinybuilder.cpp)

add_test(NAME tinybuilder COMMAND tinybuilder)

tiny_configure_test(tinybuilder DEPENDS_ON_UNITTESTS INCLUDE_MODELS RUN_SERIAL)
