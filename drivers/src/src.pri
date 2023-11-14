sourcesList += \
    $$PWD/orm/drivers/mysql/mysqldriver.cpp \
    $$PWD/orm/drivers/mysql/mysqldriver_p.cpp \
    $$PWD/orm/drivers/mysql/mysqlresult.cpp \
    $$PWD/orm/drivers/mysql/mysqlresult_p.cpp \
    $$PWD/orm/drivers/mysql/mysqlutils_p.cpp \
    $$PWD/orm/drivers/sqldriver.cpp \
    $$PWD/orm/drivers/sqldrivererror.cpp \
    $$PWD/orm/drivers/sqlfield.cpp \
    $$PWD/orm/drivers/sqlquery1.cpp \
    $$PWD/orm/drivers/sqlrecord.cpp \
    $$PWD/orm/drivers/sqlresult.cpp \
    $$PWD/orm/drivers/sqlresult_p.cpp \

SOURCES += $$sorted(sourcesList)

unset(sourcesList)
