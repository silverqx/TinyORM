project(postgresql_connection
    LANGUAGES CXX
)

add_executable(postgresql_connection
    tst_postgresql_connection.cpp
)

add_test(NAME postgresql_connection COMMAND postgresql_connection)

include(TinyTestCommon)
tiny_configure_test(postgresql_connection DEPENDS_ON_UNITTESTS)
