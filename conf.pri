# Dependencies include and library paths
# ---

win32-g++* {
}
else:win32-msvc* {
    # includes
    # MySQL C library is used by ORM and it uses mysql_ping()
    INCLUDEPATH += $$quote(C:/Program Files/MySQL/MySQL Server 8.0/include)
    # range-v3
    INCLUDEPATH += $$quote(E:/c/qMedia/vcpkg/installed/x64-windows/include)
    # boost
#    INCLUDEPATH += $$quote(E:/c_libs/boost/boost_latest)

    # libs
    # MySQL C library
    LIBS += $$quote(-LC:/Program Files/MySQL/MySQL Server 8.0/lib)
    LIBS += -llibmysql
}
