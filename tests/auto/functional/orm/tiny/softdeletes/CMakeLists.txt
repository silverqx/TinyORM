project(softdeletes
    LANGUAGES CXX
)

add_executable(softdeletes
    tst_softdeletes.cpp
)

add_test(NAME softdeletes COMMAND softdeletes)

include(TinyTestCommon)
tiny_configure_test(softdeletes INCLUDE_MODELS)
