project(databaseconnection
    LANGUAGES CXX
)

add_executable(databaseconnection
    tst_databaseconnection.cpp
)

add_test(NAME databaseconnection COMMAND databaseconnection)

include(TinyTestCommon)
tiny_configure_test(databaseconnection)
