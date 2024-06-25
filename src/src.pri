sourcesList =

extern_constants: \
    sourcesList += \
        $$PWD/orm/constants_extern.cpp \
        $$PWD/orm/schema/schemaconstants_extern.cpp

# TinyOrm library compiled against the TinyDrivers doesn't use these exception classes
load(private/tiny_drivers)
!tiny_is_building_drivers(): \
    sourcesList += \
        $$PWD/orm/exceptions/queryerror.cpp \
        $$PWD/orm/exceptions/sqlerror.cpp \

sourcesList += \
    $$PWD/orm/basegrammar.cpp \
    $$PWD/orm/concerns/countsqueries.cpp \
    $$PWD/orm/concerns/detectslostconnections.cpp \
    $$PWD/orm/concerns/hasconnectionresolver.cpp \
    $$PWD/orm/concerns/logsqueries.cpp \
    $$PWD/orm/concerns/managestransactions.cpp \
    $$PWD/orm/concerns/parsessearchpath.cpp \
    $$PWD/orm/configurations/configurationoptionsparser.cpp \
    $$PWD/orm/configurations/configurationparser.cpp \
    $$PWD/orm/configurations/configurationparserfactory.cpp \
    $$PWD/orm/configurations/mysqlconfigurationparser.cpp \
    $$PWD/orm/configurations/postgresconfigurationparser.cpp \
    $$PWD/orm/configurations/sqliteconfigurationparser.cpp \
    $$PWD/orm/connectors/connectionfactory.cpp \
    $$PWD/orm/connectors/connector.cpp \
    $$PWD/orm/connectors/mysqlconnector.cpp \
    $$PWD/orm/connectors/postgresconnector.cpp \
    $$PWD/orm/connectors/sqliteconnector.cpp \
    $$PWD/orm/databaseconnection.cpp \
    $$PWD/orm/databasemanager.cpp \
    $$PWD/orm/db.cpp \
    $$PWD/orm/exceptions/logicerror.cpp \
    $$PWD/orm/exceptions/runtimeerror.cpp \
    $$PWD/orm/libraryinfo.cpp \
    $$PWD/orm/mysqlconnection.cpp \
    $$PWD/orm/postgresconnection.cpp \
    $$PWD/orm/query/concerns/buildsqueries.cpp \
    $$PWD/orm/query/grammars/grammar.cpp \
    $$PWD/orm/query/grammars/mysqlgrammar.cpp \
    $$PWD/orm/query/grammars/postgresgrammar.cpp \
    $$PWD/orm/query/grammars/sqlitegrammar.cpp \
    $$PWD/orm/query/joinclause.cpp \
    $$PWD/orm/query/processors/processor.cpp \
    $$PWD/orm/query/processors/sqliteprocessor.cpp \
    $$PWD/orm/query/querybuilder.cpp \
    $$PWD/orm/schema.cpp \
    $$PWD/orm/schema/blueprint.cpp \
    $$PWD/orm/schema/foreignidcolumndefinitionreference.cpp \
    $$PWD/orm/schema/foreignkeydefinitionreference.cpp \
    $$PWD/orm/schema/grammars/mysqlschemagrammar.cpp \
    $$PWD/orm/schema/grammars/postgresschemagrammar.cpp \
    $$PWD/orm/schema/grammars/schemagrammar.cpp \
    $$PWD/orm/schema/grammars/sqliteschemagrammar.cpp \
    $$PWD/orm/schema/indexdefinitionreference.cpp \
    $$PWD/orm/schema/mysqlschemabuilder.cpp \
    $$PWD/orm/schema/postgresschemabuilder.cpp \
    $$PWD/orm/schema/schemabuilder.cpp \
    $$PWD/orm/schema/sqliteschemabuilder.cpp \
    $$PWD/orm/sqliteconnection.cpp \
    $$PWD/orm/types/sqlquery.cpp \
    $$PWD/orm/utils/configuration.cpp \
    $$PWD/orm/utils/fs.cpp \
    $$PWD/orm/utils/helpers.cpp \
    $$PWD/orm/utils/nullvariant.cpp \
    $$PWD/orm/utils/query.cpp \
    $$PWD/orm/utils/string.cpp \
    $$PWD/orm/utils/thread.cpp \
    $$PWD/orm/utils/type.cpp \

!disable_orm: \
    sourcesList += \
        $$PWD/orm/tiny/concerns/guardedmodel.cpp \
        $$PWD/orm/tiny/exceptions/modelnotfounderror.cpp \
        $$PWD/orm/tiny/exceptions/relationmappingnotfounderror.cpp \
        $$PWD/orm/tiny/exceptions/relationnotloadederror.cpp \
        $$PWD/orm/tiny/tinytypes.cpp \
        $$PWD/orm/tiny/utils/attribute.cpp \

SOURCES += $$sorted(sourcesList)

unset(sourcesList)
