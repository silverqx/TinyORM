INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/export.hpp \
    $$PWD/orm/basegrammar.hpp \
    $$PWD/orm/concerns/detectslostconnections.hpp \
    $$PWD/orm/concerns/hasconnectionresolver.hpp \
    $$PWD/orm/configuration.hpp \
    $$PWD/orm/connectioninterface.hpp \
    $$PWD/orm/connectionresolverinterface.hpp \
    $$PWD/orm/connectors/connectionfactory.hpp \
    $$PWD/orm/connectors/connector.hpp \
    $$PWD/orm/connectors/connectorinterface.hpp \
    $$PWD/orm/connectors/mysqlconnector.hpp \
    $$PWD/orm/connectors/sqliteconnector.hpp \
    $$PWD/orm/databaseconnection.hpp \
    $$PWD/orm/databasemanager.hpp \
    $$PWD/orm/db.hpp \
    $$PWD/orm/domainerror.hpp \
    $$PWD/orm/invalidargumenterror.hpp \
    $$PWD/orm/invalidformaterror.hpp \
    $$PWD/orm/logicerror.hpp \
    $$PWD/orm/logquery.hpp \
    $$PWD/orm/mysqlconnection.hpp \
    $$PWD/orm/ormtypes.hpp \
    $$PWD/orm/query/expression.hpp \
    $$PWD/orm/query/grammars/grammar.hpp \
    $$PWD/orm/query/grammars/mysqlgrammar.hpp \
    $$PWD/orm/query/grammars/sqlitegrammar.hpp \
    $$PWD/orm/query/joinclause.hpp \
    $$PWD/orm/query/processors/mysqlprocessor.hpp \
    $$PWD/orm/query/processors/processor.hpp \
    $$PWD/orm/query/processors/sqliteprocessor.hpp \
    $$PWD/orm/query/querybuilder.hpp \
    $$PWD/orm/queryerror.hpp \
    $$PWD/orm/runtimeerror.hpp \
    $$PWD/orm/schema/grammars/mysqlschemagrammar.hpp \
    $$PWD/orm/schema/grammars/schemagrammar.hpp \
    $$PWD/orm/schema/grammars/sqliteschemagrammar.hpp \
    $$PWD/orm/schema/mysqlschemabuilder.hpp \
    $$PWD/orm/schema/schemabuilder.hpp \
    $$PWD/orm/schema/sqliteschemabuilder.hpp \
    $$PWD/orm/sqlerror.hpp \
    $$PWD/orm/sqliteconnection.hpp \
    $$PWD/orm/sqltransactionerror.hpp \
    $$PWD/orm/support/configurationoptionsparser.hpp \
    $$PWD/orm/tiny/concerns/guardsattributes.hpp \
    $$PWD/orm/tiny/concerns/hasattributes.hpp \
    $$PWD/orm/tiny/concerns/hasrelationstore.hpp \
    $$PWD/orm/tiny/massassignmenterror.hpp \
    $$PWD/orm/tiny/model.hpp \
    $$PWD/orm/tiny/modelnotfounderror.hpp \
    $$PWD/orm/tiny/relationnotfounderror.hpp \
    $$PWD/orm/tiny/relationnotloadederror.hpp \
    $$PWD/orm/tiny/relations/basepivot.hpp \
    $$PWD/orm/tiny/relations/belongsto.hpp \
    $$PWD/orm/tiny/relations/belongstomany.hpp \
    $$PWD/orm/tiny/relations/concerns/supportsdefaultmodels.hpp \
    $$PWD/orm/tiny/relations/hasmany.hpp \
    $$PWD/orm/tiny/relations/hasone.hpp \
    $$PWD/orm/tiny/relations/hasoneormany.hpp \
    $$PWD/orm/tiny/relations/pivot.hpp \
    $$PWD/orm/tiny/relations/relation.hpp \
    $$PWD/orm/tiny/tinybuilder.hpp \
    $$PWD/orm/utils/attribute.hpp \
    $$PWD/orm/utils/string.hpp \
    $$PWD/orm/utils/type.hpp \
