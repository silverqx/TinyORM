project(tinybuilder
    LANGUAGES CXX
)

add_executable(tinybuilder
    tst_tinybuilder.cpp
)

add_test(NAME tinybuilder COMMAND tinybuilder)

include(TinyTestCommon)
tiny_configure_test(tinybuilder DEPENDS_ON_UNITTESTS INCLUDE_MODELS RUN_SERIAL)
