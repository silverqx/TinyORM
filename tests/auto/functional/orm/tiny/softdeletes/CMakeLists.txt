add_executable(softdeletes)

target_sources(softdeletes PRIVATE tst_softdeletes.cpp)

add_test(NAME softdeletes COMMAND softdeletes)

tiny_configure_test(softdeletes DEPENDS_ON_UNITTESTS INCLUDE_MODELS RUN_SERIAL)
