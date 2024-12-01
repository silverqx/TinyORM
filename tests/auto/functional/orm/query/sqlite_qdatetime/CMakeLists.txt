add_executable(sqlite_qdatetime
    tst_sqlite_qdatetime.cpp
)

add_test(NAME sqlite_qdatetime COMMAND sqlite_qdatetime)

tiny_configure_test(sqlite_qdatetime DEPENDS_ON_UNITTESTS RUN_SERIAL)
