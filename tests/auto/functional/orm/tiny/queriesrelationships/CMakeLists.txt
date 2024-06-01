project(queriesrelationships
    LANGUAGES CXX
)

add_executable(queriesrelationships
    tst_queriesrelationships.cpp
)

add_test(NAME queriesrelationships COMMAND queriesrelationships)

include(TinyTestCommon)
tiny_configure_test(queriesrelationships DEPENDS_ON_UNITTESTS INCLUDE_MODELS RUN_SERIAL)
