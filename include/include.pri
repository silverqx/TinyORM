INCLUDEPATH *= $$PWD

headersList =

extern_constants: \
    headersList += \
        $$PWD/orm/constants_extern.hpp \
        $$PWD/orm/schema/schemaconstants_extern.hpp
else: \
    headersList += \
        $$PWD/orm/constants_inline.hpp \
        $$PWD/orm/schema/schemaconstants_inline.hpp

# TinyOrm library compiled against the TinyDrivers doesn't use these exception classes
load(private/tiny_drivers)
!tiny_is_building_drivers(): \
    headersList += \
        $$PWD/orm/exceptions/queryerror.hpp \
        $$PWD/orm/exceptions/sqlerror.hpp \
        $$PWD/orm/exceptions/sqltransactionerror.hpp \

headersList += \
    $$PWD/orm/basegrammar.hpp \
    $$PWD/orm/concerns/countsqueries.hpp \
    $$PWD/orm/concerns/detectslostconnections.hpp \
    $$PWD/orm/concerns/hasconnectionresolver.hpp \
    $$PWD/orm/concerns/logsqueries.hpp \
    $$PWD/orm/concerns/managestransactions.hpp \
    $$PWD/orm/concerns/parsessearchpath.hpp \
    $$PWD/orm/config.hpp \
    $$PWD/orm/configurations/configurationoptionsparser.hpp \
    $$PWD/orm/configurations/configurationparser.hpp \
    $$PWD/orm/configurations/configurationparserfactory.hpp \
    $$PWD/orm/configurations/configurationparserinterface.hpp \
    $$PWD/orm/configurations/mysqlconfigurationparser.hpp \
    $$PWD/orm/configurations/postgresconfigurationparser.hpp \
    $$PWD/orm/configurations/sqliteconfigurationparser.hpp \
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
    $$PWD/orm/exceptions/lostconnectionerror.hpp \
    $$PWD/orm/exceptions/multiplerecordsfounderror.hpp \
    $$PWD/orm/exceptions/ormerror.hpp \
    $$PWD/orm/exceptions/outofrangeerror.hpp \
    $$PWD/orm/exceptions/recordsnotfounderror.hpp \
    $$PWD/orm/exceptions/runtimeerror.hpp \
    $$PWD/orm/exceptions/searchpathemptyerror.hpp \
    $$PWD/orm/exceptions/sqlitedatabasedoesnotexisterror.hpp \
    $$PWD/orm/libraryinfo.hpp \
    $$PWD/orm/macros/archdetect.hpp \
    $$PWD/orm/macros/commonnamespace.hpp \
    $$PWD/orm/macros/compilerdetect.hpp \
    $$PWD/orm/macros/export.hpp \
    $$PWD/orm/macros/export_common.hpp \
    $$PWD/orm/macros/likely.hpp \
    $$PWD/orm/macros/logexecutedquery.hpp \
    $$PWD/orm/macros/sqldrivermappings.hpp \
    $$PWD/orm/macros/stringify.hpp \
    $$PWD/orm/macros/systemheader.hpp \
    $$PWD/orm/macros/threadlocal.hpp \
    $$PWD/orm/mysqlconnection.hpp \
    $$PWD/orm/ormconcepts.hpp \
    $$PWD/orm/ormtypes.hpp \
    $$PWD/orm/postgresconnection.hpp \
    $$PWD/orm/query/concerns/buildsqueries.hpp \
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
    $$PWD/orm/schema.hpp \
    $$PWD/orm/schema/blueprint.hpp \
    $$PWD/orm/schema/columndefinition.hpp \
    $$PWD/orm/schema/columndefinitionreference.hpp \
    $$PWD/orm/schema/foreignidcolumndefinitionreference.hpp \
    $$PWD/orm/schema/foreignkeydefinitionreference.hpp \
    $$PWD/orm/schema/grammars/mysqlschemagrammar.hpp \
    $$PWD/orm/schema/grammars/postgresschemagrammar.hpp \
    $$PWD/orm/schema/grammars/schemagrammar.hpp \
    $$PWD/orm/schema/grammars/sqliteschemagrammar.hpp \
    $$PWD/orm/schema/indexdefinitionreference.hpp \
    $$PWD/orm/schema/mysqlschemabuilder.hpp \
    $$PWD/orm/schema/postgresschemabuilder.hpp \
    $$PWD/orm/schema/schemabuilder.hpp \
    $$PWD/orm/schema/schemaconstants.hpp \
    $$PWD/orm/schema/schematypes.hpp \
    $$PWD/orm/schema/sqliteschemabuilder.hpp \
    $$PWD/orm/sqliteconnection.hpp \
    $$PWD/orm/support/databaseconfiguration.hpp \
    $$PWD/orm/support/databaseconnectionsmap.hpp \
    $$PWD/orm/support/replacebindings.hpp \
    $$PWD/orm/types/aboutvalue.hpp \
    $$PWD/orm/types/log.hpp \
    $$PWD/orm/types/sqlquery.hpp \
    $$PWD/orm/types/statementscounter.hpp \
    $$PWD/orm/utils/configuration.hpp \
    $$PWD/orm/utils/container.hpp \
    $$PWD/orm/utils/fs.hpp \
    $$PWD/orm/utils/helpers.hpp \
    $$PWD/orm/utils/integralcast.hpp \
    $$PWD/orm/utils/notnull.hpp \
    $$PWD/orm/utils/nullvariant.hpp \
    $$PWD/orm/utils/query.hpp \
    $$PWD/orm/utils/string.hpp \
    $$PWD/orm/utils/thread.hpp \
    $$PWD/orm/utils/type.hpp \
    $$PWD/orm/version.hpp \

!disable_orm: \
    headersList += \
        $$PWD/orm/tiny/casts/attribute.hpp \
        $$PWD/orm/tiny/concerns/buildsqueries.hpp \
        $$PWD/orm/tiny/concerns/buildssoftdeletes.hpp \
        $$PWD/orm/tiny/concerns/guardedmodel.hpp \
        $$PWD/orm/tiny/concerns/guardsattributes.hpp \
        $$PWD/orm/tiny/concerns/hasattributes.hpp \
        $$PWD/orm/tiny/concerns/hasrelationships.hpp \
        $$PWD/orm/tiny/concerns/hasrelationstore.hpp \
        $$PWD/orm/tiny/concerns/hastimestamps.hpp \
        $$PWD/orm/tiny/concerns/hidesattributes.hpp \
        $$PWD/orm/tiny/concerns/queriesrelationships.hpp \
        $$PWD/orm/tiny/exceptions/massassignmenterror.hpp \
        $$PWD/orm/tiny/exceptions/modelnotfounderror.hpp \
        $$PWD/orm/tiny/exceptions/mutatormappingnotfounderror.hpp \
        $$PWD/orm/tiny/exceptions/relationmappingnotfounderror.hpp \
        $$PWD/orm/tiny/exceptions/relationnotloadederror.hpp \
        $$PWD/orm/tiny/macros/crtpmodel.hpp \
        $$PWD/orm/tiny/macros/crtpmodelwithbase.hpp \
        $$PWD/orm/tiny/macros/relationstoresaliases.hpp \
        $$PWD/orm/tiny/model.hpp \
        $$PWD/orm/tiny/modelproxies.hpp \
        $$PWD/orm/tiny/relations/basepivot.hpp \
        $$PWD/orm/tiny/relations/belongsto.hpp \
        $$PWD/orm/tiny/relations/belongstomany.hpp \
        $$PWD/orm/tiny/relations/concerns/comparesrelatedmodels.hpp \
        $$PWD/orm/tiny/relations/concerns/interactswithpivottable.hpp \
        $$PWD/orm/tiny/relations/concerns/supportsdefaultmodels.hpp \
        $$PWD/orm/tiny/relations/hasmany.hpp \
        $$PWD/orm/tiny/relations/hasone.hpp \
        $$PWD/orm/tiny/relations/hasoneormany.hpp \
        $$PWD/orm/tiny/relations/pivot.hpp \
        $$PWD/orm/tiny/relations/relation.hpp \
        $$PWD/orm/tiny/relations/relationproxies.hpp \
        $$PWD/orm/tiny/relations/relationtypes.hpp \
        $$PWD/orm/tiny/softdeletes.hpp \
        $$PWD/orm/tiny/support/stores/baserelationstore.hpp \
        $$PWD/orm/tiny/support/stores/belongstomanyrelatedtablestore.hpp \
        $$PWD/orm/tiny/support/stores/eagerrelationstore.hpp \
        $$PWD/orm/tiny/support/stores/lazyrelationstore.hpp \
        $$PWD/orm/tiny/support/stores/pushrelationstore.hpp \
        $$PWD/orm/tiny/support/stores/queriesrelationshipsstore.hpp \
        $$PWD/orm/tiny/support/stores/serializerelationstore.hpp \
        $$PWD/orm/tiny/support/stores/touchownersrelationstore.hpp \
        $$PWD/orm/tiny/tinybuilder.hpp \
        $$PWD/orm/tiny/tinybuilderproxies.hpp \
        $$PWD/orm/tiny/tinyconcepts.hpp \
        $$PWD/orm/tiny/tinytypes.hpp \
        $$PWD/orm/tiny/types/connectionoverride.hpp \
        $$PWD/orm/tiny/types/modelattributes.hpp \
        $$PWD/orm/tiny/types/modelscollection.hpp \
        $$PWD/orm/tiny/types/syncchanges.hpp \
        $$PWD/orm/tiny/utils/attribute.hpp \

HEADERS += $$sorted(headersList)

unset(headersList)
