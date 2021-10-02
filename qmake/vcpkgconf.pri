# Provide include and lib paths to the qmake during a vcpkg install
# <vcpkg> tag is replaced by vcpkg_replace_string() during vcpkg install
INCLUDEPATH += $$quote(<vcpkg>/include)
LIBS += $$quote(-L<vcpkg>/lib)

# Link against the libmysql if the tinyorm[mysql-ping] feature was used
contains(DEFINES, USE_MYSQL_PING): \
    LIBS += -llibmysql
