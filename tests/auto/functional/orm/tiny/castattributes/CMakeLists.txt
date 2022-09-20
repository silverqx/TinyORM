project(castattributes
    LANGUAGES CXX
)

add_executable(castattributes
    tst_castattributes.cpp
)

add_test(NAME castattributes COMMAND castattributes)

include(TinyTestCommon)
tiny_configure_test(castattributes INCLUDE_MODELS)

