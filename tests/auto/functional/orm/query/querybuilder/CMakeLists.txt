project(querybuilder
    LANGUAGES CXX
)

add_executable(querybuilder
    tst_querybuilder.cpp
)

add_test(NAME querybuilder COMMAND querybuilder)

include(TinyTestCommon)
tiny_configure_test(querybuilder DEPENDS_ON_UNITTESTS RUN_SERIAL)
