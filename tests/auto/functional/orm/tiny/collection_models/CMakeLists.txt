project(collection_models
    LANGUAGES CXX
)

add_executable(collection_models
    tst_collection_models.cpp
)

add_test(NAME collection_models COMMAND collection_models)

include(TinyTestCommon)
tiny_configure_test(collection_models DEPENDS_ON_UNITTESTS INCLUDE_MODELS)
