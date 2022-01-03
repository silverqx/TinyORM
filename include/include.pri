INCLUDEPATH += $$PWD

extern_constants: \
    headersList += $$PWD/orm/constants_extern.hpp
else: \
    headersList += $$PWD/orm/constants_inline.hpp

headersList += \
    $$PWD/orm/basegrammar.hpp \
    $$PWD/orm/concepts.hpp \
    $$PWD/orm/concerns/detectslostconnections.hpp \
    $$PWD/orm/concerns/hasconnectionresolver.hpp \
    $$PWD/orm/config.hpp \
    $$PWD/orm/connectioninterface.hpp \
    $$PWD/orm/connectionresolverinterface.hpp \
    $$PWD/orm/connectors/connectionfactory.hpp \
    $$PWD/orm/connectors/connector.hpp \
    $$PWD/orm/connectors/connectorinterface.hpp \
    $$PWD/orm/connectors/mysqlconnector.hpp \
    $$PWD/orm/connectors/postgresconnector.hpp \
    $$PWD/orm/connectors/sqliteconnector.hpp \
    $$PWD/orm/constants.hpp \
    $$PWD/orm/databaseconnection.hpp \
    $$PWD/orm/databasemanager.hpp \
    $$PWD/orm/db.hpp \
    $$PWD/orm/exceptions/domainerror.hpp \
    $$PWD/orm/exceptions/invalidargumenterror.hpp \
    $$PWD/orm/exceptions/invalidformaterror.hpp \
    $$PWD/orm/exceptions/invalidtemplateargumenterror.hpp \
    $$PWD/orm/exceptions/logicerror.hpp \
    $$PWD/orm/exceptions/queryerror.hpp \
    $$PWD/orm/exceptions/runtimeerror.hpp \
    $$PWD/orm/exceptions/sqlerror.hpp \
    $$PWD/orm/exceptions/sqltransactionerror.hpp \
    $$PWD/orm/libraryinfo.hpp \
    $$PWD/orm/macros/archdetect.hpp \
    $$PWD/orm/macros/commonnamespace.hpp \
    $$PWD/orm/macros/compilerdetect.hpp \
    $$PWD/orm/macros/export.hpp \
    $$PWD/orm/macros/export_common.hpp \
    $$PWD/orm/macros/likely.hpp \
    $$PWD/orm/macros/logexecutedquery.hpp \
    $$PWD/orm/macros/systemheader.hpp \
    $$PWD/orm/macros/threadlocal.hpp \
    $$PWD/orm/mysqlconnection.hpp \
    $$PWD/orm/ormtypes.hpp \
    $$PWD/orm/postgresconnection.hpp \
    $$PWD/orm/query/expression.hpp \
    $$PWD/orm/query/grammars/grammar.hpp \
    $$PWD/orm/query/grammars/mysqlgrammar.hpp \
    $$PWD/orm/query/grammars/postgresgrammar.hpp \
    $$PWD/orm/query/grammars/sqlitegrammar.hpp \
    $$PWD/orm/query/joinclause.hpp \
    $$PWD/orm/query/processors/mysqlprocessor.hpp \
    $$PWD/orm/query/processors/postgresprocessor.hpp \
    $$PWD/orm/query/processors/processor.hpp \
    $$PWD/orm/query/processors/sqliteprocessor.hpp \
    $$PWD/orm/query/querybuilder.hpp \
    $$PWD/orm/schema/grammars/mysqlschemagrammar.hpp \
    $$PWD/orm/schema/grammars/postgresschemagrammar.hpp \
    $$PWD/orm/schema/grammars/schemagrammar.hpp \
    $$PWD/orm/schema/grammars/sqliteschemagrammar.hpp \
    $$PWD/orm/schema/mysqlschemabuilder.hpp \
    $$PWD/orm/schema/postgresschemabuilder.hpp \
    $$PWD/orm/schema/schemabuilder.hpp \
    $$PWD/orm/schema/sqliteschemabuilder.hpp \
    $$PWD/orm/sqliteconnection.hpp \
    $$PWD/orm/support/configurationoptionsparser.hpp \
    $$PWD/orm/support/databaseconfiguration.hpp \
    $$PWD/orm/support/databaseconnectionsmap.hpp \
    $$PWD/orm/types/log.hpp \
    $$PWD/orm/types/statementscounter.hpp \
    $$PWD/orm/utils/fs.hpp \
    $$PWD/orm/utils/query.hpp \
    $$PWD/orm/utils/thread.hpp \
    $$PWD/orm/utils/type.hpp \
    $$PWD/orm/version.hpp \

!disable_orm: \
    headersList += \
        $$PWD/orm/tiny/concerns/guardsattributes.hpp \
        $$PWD/orm/tiny/concerns/hasattributes.hpp \
        $$PWD/orm/tiny/concerns/hasrelationstore.hpp \
        $$PWD/orm/tiny/concerns/queriesrelationships.hpp \
        $$PWD/orm/tiny/exceptions/massassignmenterror.hpp \
        $$PWD/orm/tiny/exceptions/modelnotfounderror.hpp \
        $$PWD/orm/tiny/exceptions/relationnotfounderror.hpp \
        $$PWD/orm/tiny/exceptions/relationnotloadederror.hpp \
        $$PWD/orm/tiny/model.hpp \
        $$PWD/orm/tiny/modelproxies.hpp \
        $$PWD/orm/tiny/relations/basepivot.hpp \
        $$PWD/orm/tiny/relations/belongsto.hpp \
        $$PWD/orm/tiny/relations/belongstomany.hpp \
        $$PWD/orm/tiny/relations/concerns/supportsdefaultmodels.hpp \
        $$PWD/orm/tiny/relations/hasmany.hpp \
        $$PWD/orm/tiny/relations/hasone.hpp \
        $$PWD/orm/tiny/relations/hasoneormany.hpp \
        $$PWD/orm/tiny/relations/pivot.hpp \
        $$PWD/orm/tiny/relations/relation.hpp \
        $$PWD/orm/tiny/relations/relationproxies.hpp \
        $$PWD/orm/tiny/tinybuilder.hpp \
        $$PWD/orm/tiny/tinybuilderproxies.hpp \
        $$PWD/orm/tiny/tinytypes.hpp \
        $$PWD/orm/tiny/types/syncchanges.hpp \
        $$PWD/orm/tiny/utils/attribute.hpp \
        $$PWD/orm/tiny/utils/string.hpp \

HEADERS += $$sorted(headersList)

unset(headersList)
