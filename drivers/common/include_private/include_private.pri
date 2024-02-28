INCLUDEPATH *= $$PWD

headersList =

extern_constants: \
    headersList += $$PWD/orm/drivers/constants_extern_p.hpp
else: \
    headersList += $$PWD/orm/drivers/constants_inline_p.hpp

headersList += \
    $$PWD/orm/drivers/constants_p.hpp \
    $$PWD/orm/drivers/macros/declaresqldriverprivate_p.hpp \
    $$PWD/orm/drivers/sqldatabase_p.hpp \
    $$PWD/orm/drivers/sqldriver_p.hpp \
    $$PWD/orm/drivers/sqlresult_p.hpp \
    $$PWD/orm/drivers/support/connectionshash_p.hpp \
    $$PWD/orm/drivers/utils/type_p.hpp \

HEADERS += $$sorted(headersList)

unset(headersList)
