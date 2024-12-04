add_executable(sqldatabase)

target_sources(sqldatabase PRIVATE tst_sqldatabase.cpp)

add_test(NAME sqldatabase COMMAND sqldatabase)

tiny_configure_test(sqldatabase)
