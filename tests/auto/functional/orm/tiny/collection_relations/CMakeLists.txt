add_executable(collection_relations
    tst_collection_relations.cpp
)

add_test(NAME collection_relations COMMAND collection_relations)

tiny_configure_test(collection_relations DEPENDS_ON_UNITTESTS INCLUDE_MODELS)
