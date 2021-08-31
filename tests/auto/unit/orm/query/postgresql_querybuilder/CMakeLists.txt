project(postgresql_querybuilder
    LANGUAGES CXX
)

add_executable(postgresql_querybuilder
    tst_postgresql_querybuilder.cpp
)

add_test(NAME postgresql_querybuilder COMMAND postgresql_querybuilder)

include(TinyTestCommon)
tiny_configure_test(postgresql_querybuilder)
