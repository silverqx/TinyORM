project(sqlite_qdatetime
    LANGUAGES CXX
)

add_executable(sqlite_qdatetime
    tst_sqlite_qdatetime.cpp
)

add_test(NAME sqlite_qdatetime COMMAND sqlite_qdatetime)

include(TinyTestCommon)
tiny_configure_test(sqlite_qdatetime DEPENDS_ON_UNITTESTS RUN_SERIAL)
