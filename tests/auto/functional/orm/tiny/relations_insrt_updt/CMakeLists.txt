project(relations_inserting_updating
    LANGUAGES CXX
)

add_executable(relations_inserting_updating
    tst_relations_inserting_updating.cpp
)

add_test(NAME relations_inserting_updating COMMAND relations_inserting_updating)

include(TinyTestCommon)
tiny_configure_test(relations_inserting_updating INCLUDE_MODELS)
