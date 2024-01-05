sourcesList =

extern_constants: \
    sourcesList += $$PWD/orm/drivers/constants_extern_p.cpp

sourcesList += \
    $$PWD/orm/drivers/exceptions/logicerror.cpp \
    $$PWD/orm/drivers/exceptions/runtimeerror.cpp \
    $$PWD/orm/drivers/sqldatabase.cpp \
    $$PWD/orm/drivers/sqldatabase_p.cpp \
    $$PWD/orm/drivers/sqldatabasemanager.cpp \
    $$PWD/orm/drivers/sqldriver.cpp \
    $$PWD/orm/drivers/sqlerror.cpp \
    $$PWD/orm/drivers/sqlfield.cpp \
    $$PWD/orm/drivers/sqlquery.cpp \
    $$PWD/orm/drivers/sqlrecord.cpp \
    $$PWD/orm/drivers/sqlresult.cpp \
    $$PWD/orm/drivers/sqlresult_p.cpp \
    $$PWD/orm/drivers/utils/type_p.cpp \

SOURCES += $$sorted(sourcesList)

unset(sourcesList)
