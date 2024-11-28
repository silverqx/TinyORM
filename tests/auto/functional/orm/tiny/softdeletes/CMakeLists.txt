project(softdeletes
    LANGUAGES CXX
)

add_executable(softdeletes
    tst_softdeletes.cpp
)

add_test(NAME softdeletes COMMAND softdeletes)

tiny_configure_test(softdeletes DEPENDS_ON_UNITTESTS INCLUDE_MODELS RUN_SERIAL)
