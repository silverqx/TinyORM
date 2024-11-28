project(sqldatabase
    LANGUAGES CXX
)

add_executable(sqldatabase
    tst_sqldatabase.cpp
)

add_test(NAME sqldatabase COMMAND sqldatabase)

tiny_configure_test(sqldatabase)
