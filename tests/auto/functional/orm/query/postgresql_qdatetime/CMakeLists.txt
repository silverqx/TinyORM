project(postgresql_qdatetime
    LANGUAGES CXX
)

add_executable(postgresql_qdatetime
    tst_postgresql_qdatetime.cpp
)

add_test(NAME postgresql_qdatetime COMMAND postgresql_qdatetime)

include(TinyTestCommon)
tiny_configure_test(postgresql_qdatetime DEPENDS_ON_UNITTESTS RUN_SERIAL)
