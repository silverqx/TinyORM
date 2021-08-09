INCLUDEPATH += <vcpkg>/include
LIBS += -L<vcpkg>/lib
contains(DEFINES, USE_MYSQL_PING) {
    LIBS += -llibmysql
}
