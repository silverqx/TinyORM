project(relations_inserting_updating
    LANGUAGES CXX
)

add_executable(relations_insrt_updt
    tst_relations_inserting_updating.cpp
)

add_test(NAME relations_insrt_updt COMMAND relations_insrt_updt)

include(TinyTestCommon)
tiny_configure_test(relations_insrt_updt DEPENDS_ON_UNITTESTS INCLUDE_MODELS RUN_SERIAL)
