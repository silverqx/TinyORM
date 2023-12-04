INCLUDEPATH *= $$PWD

headersList = \
    $$PWD/orm/drivers/mysql/mysqldriver.hpp \
    $$PWD/orm/drivers/mysql/mysqlresult.hpp \
    $$PWD/orm/drivers/mysql/version.hpp \

HEADERS += $$sorted(headersList)

unset(headersList)
