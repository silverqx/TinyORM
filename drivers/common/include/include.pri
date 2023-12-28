INCLUDEPATH *= $$PWD

headersList = \
    $$PWD/orm/drivers/driverstypes.hpp \
    $$PWD/orm/drivers/macros/export.hpp \
    $$PWD/orm/drivers/sqldatabase.hpp \
    $$PWD/orm/drivers/sqldatabasemanager.hpp \
    $$PWD/orm/drivers/sqldriver.hpp \
    $$PWD/orm/drivers/sqlerror.hpp \
    $$PWD/orm/drivers/sqlfield.hpp \
    $$PWD/orm/drivers/sqlquery.hpp \
    $$PWD/orm/drivers/sqlrecord.hpp \
    $$PWD/orm/drivers/sqlresult.hpp \
    $$PWD/orm/drivers/utils/notnull.hpp \
    $$PWD/orm/drivers/version.hpp \

HEADERS += $$sorted(headersList)

unset(headersList)
