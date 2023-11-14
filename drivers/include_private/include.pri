INCLUDEPATH *= $$PWD

headersList += \
    $$PWD/orm/drivers/macros/includemysqlh.hpp \
    $$PWD/orm/drivers/mysql/mysqldriver_p.hpp \
    $$PWD/orm/drivers/mysql/mysqlresult_p.hpp \
    $$PWD/orm/drivers/mysql/mysqlutils_p.hpp \
    $$PWD/orm/drivers/sqldriver_p.hpp \
    $$PWD/orm/drivers/sqlresult_p.hpp \

HEADERS += $$sorted(headersList)

unset(headersList)
