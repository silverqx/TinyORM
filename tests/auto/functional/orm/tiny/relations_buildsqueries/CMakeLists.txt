project(relations_buildsqueries
    LANGUAGES CXX
)

add_executable(relations_buildsqueries
    tst_relations_buildsqueries.cpp
)

add_test(NAME relations_buildsqueries COMMAND relations_buildsqueries)

include(TinyTestCommon)
tiny_configure_test(relations_buildsqueries DEPENDS_ON_UNITTESTS INCLUDE_MODELS RUN_SERIAL)
