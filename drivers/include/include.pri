INCLUDEPATH *= $$PWD

headersList += \
    $$PWD/orm/drivers/mysql/mysqldriver.hpp \
    $$PWD/orm/drivers/sqldatabase.hpp \
    $$PWD/orm/drivers/sqldatabasemanager.hpp \
    $$PWD/orm/drivers/sqldriver.hpp \
    $$PWD/orm/drivers/sqldrivererror.hpp \
    $$PWD/orm/drivers/sqlfield.hpp \
    $$PWD/orm/drivers/sqlquery1.hpp \
    $$PWD/orm/drivers/sqlrecord.hpp \
    $$PWD/orm/drivers/sqlresult.hpp \

HEADERS += $$sorted(headersList)

unset(headersList)
