INCLUDEPATH *= $$PWD

headersList =

extern_constants: \
    headersList += $$PWD/orm/drivers/mysql/mysqlconstants_extern_p.hpp
else: \
    headersList += $$PWD/orm/drivers/mysql/mysqlconstants_inline_p.hpp

headersList += \
    $$PWD/orm/drivers/mysql/macros/includemysqlh_p.hpp \
    $$PWD/orm/drivers/mysql/mysqlconstants_p.hpp \
    $$PWD/orm/drivers/mysql/mysqldriver_p.hpp \
    $$PWD/orm/drivers/mysql/mysqlresult_p.hpp \
    $$PWD/orm/drivers/mysql/mysqltypes_p.hpp \
    $$PWD/orm/drivers/mysql/mysqlutils_p.hpp \

HEADERS += $$sorted(headersList)

unset(headersList)
