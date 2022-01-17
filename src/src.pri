extern_constants: \
    sourcesList += $$PWD/orm/constants_extern.cpp

sourcesList += \
    $$PWD/orm/basegrammar.cpp \
    $$PWD/orm/concerns/countsqueries.cpp \
    $$PWD/orm/concerns/detectslostconnections.cpp \
    $$PWD/orm/concerns/hasconnectionresolver.cpp \
    $$PWD/orm/concerns/logsqueries.cpp \
    $$PWD/orm/concerns/managestransactions.cpp \
    $$PWD/orm/connectors/connectionfactory.cpp \
    $$PWD/orm/connectors/connector.cpp \
    $$PWD/orm/connectors/mysqlconnector.cpp \
    $$PWD/orm/connectors/postgresconnector.cpp \
    $$PWD/orm/connectors/sqliteconnector.cpp \
    $$PWD/orm/databaseconnection.cpp \
    $$PWD/orm/databasemanager.cpp \
    $$PWD/orm/db.cpp \
    $$PWD/orm/exceptions/logicerror.cpp \
    $$PWD/orm/exceptions/queryerror.cpp \
    $$PWD/orm/exceptions/runtimeerror.cpp \
    $$PWD/orm/exceptions/sqlerror.cpp \
    $$PWD/orm/libraryinfo.cpp \
    $$PWD/orm/mysqlconnection.cpp \
    $$PWD/orm/postgresconnection.cpp \
    $$PWD/orm/query/grammars/grammar.cpp \
    $$PWD/orm/query/grammars/mysqlgrammar.cpp \
    $$PWD/orm/query/grammars/postgresgrammar.cpp \
    $$PWD/orm/query/grammars/sqlitegrammar.cpp \
    $$PWD/orm/query/joinclause.cpp \
    $$PWD/orm/query/processors/mysqlprocessor.cpp \
    $$PWD/orm/query/processors/postgresprocessor.cpp \
    $$PWD/orm/query/processors/processor.cpp \
    $$PWD/orm/query/processors/sqliteprocessor.cpp \
    $$PWD/orm/query/querybuilder.cpp \
    $$PWD/orm/schema/grammars/mysqlschemagrammar.cpp \
    $$PWD/orm/schema/grammars/postgresschemagrammar.cpp \
    $$PWD/orm/schema/grammars/sqliteschemagrammar.cpp \
    $$PWD/orm/schema/mysqlschemabuilder.cpp \
    $$PWD/orm/schema/postgresschemabuilder.cpp \
    $$PWD/orm/schema/schemabuilder.cpp \
    $$PWD/orm/sqliteconnection.cpp \
    $$PWD/orm/support/configurationoptionsparser.cpp \
    $$PWD/orm/utils/fs.cpp \
    $$PWD/orm/utils/query.cpp \
    $$PWD/orm/utils/thread.cpp \
    $$PWD/orm/utils/type.cpp \

!disable_orm: \
    sourcesList += \
        $$PWD/orm/tiny/exceptions/modelnotfounderror.cpp \
        $$PWD/orm/tiny/exceptions/relationnotfounderror.cpp \
        $$PWD/orm/tiny/exceptions/relationnotloadederror.cpp \
        $$PWD/orm/tiny/tinytypes.cpp \
        $$PWD/orm/tiny/types/syncchanges.cpp \
        $$PWD/orm/tiny/utils/attribute.cpp \
        $$PWD/orm/tiny/utils/string.cpp \

SOURCES += $$sorted(sourcesList)

unset(sourcesList)
