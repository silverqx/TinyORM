sourcesList =

extern_constants: \
    sourcesList += $$PWD/orm/drivers/mysql/mysqlconstants_extern_p.cpp

build_loadable_drivers: \
    sourcesList += $$PWD/orm/drivers/mysql/main.cpp

sourcesList += \
    $$PWD/orm/drivers/mysql/mysqldriver.cpp \
    $$PWD/orm/drivers/mysql/mysqldriver_p.cpp \
    $$PWD/orm/drivers/mysql/mysqlresult.cpp \
    $$PWD/orm/drivers/mysql/mysqlresult_p.cpp \
    $$PWD/orm/drivers/mysql/mysqlutils_p.cpp \

SOURCES += $$sorted(sourcesList)

unset(sourcesList)
