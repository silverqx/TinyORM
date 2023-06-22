project(queriesrelationships
    LANGUAGES CXX
)

add_executable(queriesrelationships
    tst_queriesrelationships.cpp
)

add_test(NAME queriesrelationships COMMAND queriesrelationships)

include(TinyTestCommon)
tiny_configure_test(queriesrelationships INCLUDE_MODELS)
