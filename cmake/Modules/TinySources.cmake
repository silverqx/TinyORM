# TinyORM library header and source files
# Create header and source files lists and return them
function(tinyorm_sources out_headers out_sources)

    # ORM headers section
    set(headers)

    if(TINY_EXTERN_CONSTANTS)
        list(APPEND headers
            constants_extern.hpp
            schema/schemaconstants_extern.hpp
        )
    else()
        list(APPEND headers
            constants_inline.hpp
            schema/schemaconstants_inline.hpp
        )
    endif()

    list(APPEND headers
        basegrammar.hpp
        concerns/countsqueries.hpp
        concerns/detectslostconnections.hpp
        concerns/hasconnectionresolver.hpp
        concerns/logsqueries.hpp
        concerns/managestransactions.hpp
        connectionresolverinterface.hpp
        connectors/connectionfactory.hpp
        connectors/connector.hpp
        connectors/connectorinterface.hpp
        connectors/mysqlconnector.hpp
        connectors/postgresconnector.hpp
        connectors/sqliteconnector.hpp
        constants.hpp
        databaseconnection.hpp
        databasemanager.hpp
        db.hpp
        exceptions/domainerror.hpp
        exceptions/invalidargumenterror.hpp
        exceptions/invalidformaterror.hpp
        exceptions/invalidtemplateargumenterror.hpp
        exceptions/logicerror.hpp
        exceptions/queryerror.hpp
        exceptions/runtimeerror.hpp
        exceptions/sqlerror.hpp
        exceptions/sqltransactionerror.hpp
        libraryinfo.hpp
        macros/archdetect.hpp
        macros/compilerdetect.hpp
        macros/export.hpp
        macros/export_common.hpp
        macros/likely.hpp
        macros/logexecutedquery.hpp
        macros/systemheader.hpp
        macros/threadlocal.hpp
        mysqlconnection.hpp
        ormconcepts.hpp
        ormtypes.hpp
        postgresconnection.hpp
        query/expression.hpp
        query/grammars/grammar.hpp
        query/grammars/mysqlgrammar.hpp
        query/grammars/postgresgrammar.hpp
        query/grammars/sqlitegrammar.hpp
        query/joinclause.hpp
        query/processors/mysqlprocessor.hpp
        query/processors/postgresprocessor.hpp
        query/processors/processor.hpp
        query/processors/sqliteprocessor.hpp
        query/querybuilder.hpp
        schema.hpp
        schema/blueprint.hpp
        schema/columndefinition.hpp
        schema/columndefinitionreference.hpp
        schema/foreignidcolumndefinitionreference.hpp
        schema/foreignkeydefinitionreference.hpp
        schema/grammars/mysqlschemagrammar.hpp
        schema/grammars/postgresschemagrammar.hpp
        schema/grammars/schemagrammar.hpp
        schema/grammars/sqliteschemagrammar.hpp
        schema/mysqlschemabuilder.hpp
        schema/postgresschemabuilder.hpp
        schema/schemabuilder.hpp
        schema/schemaconstants.hpp
        schema/schematypes.hpp
        schema/sqliteschemabuilder.hpp
        sqliteconnection.hpp
        support/configurationoptionsparser.hpp
        support/databaseconfiguration.hpp
        support/databaseconnectionsmap.hpp
        types/log.hpp
        types/statementscounter.hpp
        utils/container.hpp
        utils/fs.hpp
        utils/query.hpp
        utils/thread.hpp
        utils/type.hpp
        version.hpp
    )

    if(ORM)
        list(APPEND headers
            tiny/concerns/guardedmodel.hpp
            tiny/concerns/guardsattributes.hpp
            tiny/concerns/hasattributes.hpp
            tiny/concerns/hasrelationships.hpp
            tiny/concerns/hasrelationstore.hpp
            tiny/concerns/hastimestamps.hpp
            tiny/concerns/queriesrelationships.hpp
            tiny/exceptions/massassignmenterror.hpp
            tiny/exceptions/modelnotfounderror.hpp
            tiny/exceptions/relationnotfounderror.hpp
            tiny/exceptions/relationnotloadederror.hpp
            tiny/macros/crtpmodel.hpp
            tiny/macros/crtpmodelwithbase.hpp
            tiny/model.hpp
            tiny/modelproxies.hpp
            tiny/relations/basepivot.hpp
            tiny/relations/belongsto.hpp
            tiny/relations/belongstomany.hpp
            tiny/relations/concerns/supportsdefaultmodels.hpp
            tiny/relations/hasmany.hpp
            tiny/relations/hasone.hpp
            tiny/relations/hasoneormany.hpp
            tiny/relations/pivot.hpp
            tiny/relations/relation.hpp
            tiny/relations/relationproxies.hpp
            tiny/relations/relationtypes.hpp
            tiny/tinybuilder.hpp
            tiny/tinybuilderproxies.hpp
            tiny/tinyconcepts.hpp
            tiny/tinytypes.hpp
            tiny/types/connectionoverride.hpp
            tiny/types/syncchanges.hpp
            tiny/utils/attribute.hpp
        )
    endif()

    # Headers used in both ORM and TOM
    if(ORM OR TOM)
        list(APPEND headers
            tiny/utils/string.hpp
        )
    endif()

    # ORM sources section
    set(sources)

    if(TINY_EXTERN_CONSTANTS)
        list(APPEND sources
            constants_extern.cpp
            schema/schemaconstants_extern.cpp
        )
    endif()

    list(APPEND sources
        basegrammar.cpp
        concerns/countsqueries.cpp
        concerns/detectslostconnections.cpp
        concerns/hasconnectionresolver.cpp
        concerns/logsqueries.cpp
        concerns/managestransactions.cpp
        connectors/connectionfactory.cpp
        connectors/connector.cpp
        connectors/mysqlconnector.cpp
        connectors/postgresconnector.cpp
        connectors/sqliteconnector.cpp
        databaseconnection.cpp
        databasemanager.cpp
        db.cpp
        exceptions/logicerror.cpp
        exceptions/queryerror.cpp
        exceptions/runtimeerror.cpp
        exceptions/sqlerror.cpp
        libraryinfo.cpp
        mysqlconnection.cpp
        postgresconnection.cpp
        query/grammars/grammar.cpp
        query/grammars/mysqlgrammar.cpp
        query/grammars/postgresgrammar.cpp
        query/grammars/sqlitegrammar.cpp
        query/joinclause.cpp
        query/processors/mysqlprocessor.cpp
        query/processors/postgresprocessor.cpp
        query/processors/processor.cpp
        query/processors/sqliteprocessor.cpp
        query/querybuilder.cpp
        schema.cpp
        schema/blueprint.cpp
        schema/foreignidcolumndefinitionreference.cpp
        schema/foreignkeydefinitionreference.cpp
        schema/grammars/mysqlschemagrammar.cpp
        schema/grammars/postgresschemagrammar.cpp
        schema/grammars/schemagrammar.cpp
        schema/grammars/sqliteschemagrammar.cpp
        schema/mysqlschemabuilder.cpp
        schema/postgresschemabuilder.cpp
        schema/schemabuilder.cpp
        sqliteconnection.cpp
        support/configurationoptionsparser.cpp
        utils/fs.cpp
        utils/query.cpp
        utils/thread.cpp
        utils/type.cpp
    )

    if(ORM)
        list(APPEND sources
            tiny/concerns/guardedmodel.cpp
            tiny/exceptions/modelnotfounderror.cpp
            tiny/exceptions/relationnotfounderror.cpp
            tiny/exceptions/relationnotloadederror.cpp
            tiny/tinytypes.cpp
            tiny/types/syncchanges.cpp
            tiny/utils/attribute.cpp
        )
    endif()

    # Sources needed in both ORM and TOM
    if(ORM OR TOM)
        list(APPEND sources
            tiny/utils/string.cpp
        )
    endif()

    list(SORT headers)
    list(SORT sources)

    list(TRANSFORM headers PREPEND "include/orm/")
    list(TRANSFORM sources PREPEND "src/orm/")

    set(${out_headers} ${headers} PARENT_SCOPE)
    set(${out_sources} ${sources} PARENT_SCOPE)
endfunction()

# TinyTom application header and source files
# Create header and source files lists and return them
function(tinytom_sources out_headers out_sources)

    # Tom headers section
    set(headers)

    if(TINY_EXTERN_CONSTANTS)
        list(APPEND headers
            tomconstants_extern.hpp
        )
    else()
        list(APPEND headers
            tomconstants_inline.hpp
        )
    endif()

    list(APPEND headers
        application.hpp
        commands/command.hpp
        commands/database/seedcommand.hpp
        commands/database/wipecommand.hpp
        commands/environmentcommand.hpp
        commands/helpcommand.hpp
        commands/inspirecommand.hpp
        commands/listcommand.hpp
        commands/make/migrationcommand.hpp
#        commands/make/projectcommand.hpp
        commands/make/stubs/migrationstubs.hpp
        commands/make/stubs/projectstubs.hpp
        commands/migrations/freshcommand.hpp
        commands/migrations/installcommand.hpp
        commands/migrations/migratecommand.hpp
        commands/migrations/refreshcommand.hpp
        commands/migrations/resetcommand.hpp
        commands/migrations/rollbackcommand.hpp
        commands/migrations/statuscommand.hpp
        concerns/callscommands.hpp
        concerns/confirmable.hpp
        concerns/guesscommandname.hpp
        concerns/interactswithio.hpp
        concerns/printsoptions.hpp
        concerns/usingconnection.hpp
        config.hpp
        exceptions/invalidargumenterror.hpp
        exceptions/invalidtemplateargumenterror.hpp
        exceptions/logicerror.hpp
        exceptions/runtimeerror.hpp
        exceptions/tomerror.hpp
        migration.hpp
        migrationcreator.hpp
        migrationrepository.hpp
        migrator.hpp
        seeder.hpp
        tableguesser.hpp
        terminal.hpp
        tomconstants.hpp
        tomtypes.hpp
        tomutils.hpp
        version.hpp
    )

    # Tom sources section
    set(sources)

    if(TINY_EXTERN_CONSTANTS)
        list(APPEND sources
            tomconstants_extern.cpp
        )
    endif()

    list(APPEND sources
        application.cpp
        commands/command.cpp
        commands/database/seedcommand.cpp
        commands/database/wipecommand.cpp
        commands/environmentcommand.cpp
        commands/helpcommand.cpp
        commands/inspirecommand.cpp
        commands/listcommand.cpp
        commands/make/migrationcommand.cpp
#        commands/make/projectcommand.cpp
        commands/migrations/freshcommand.cpp
        commands/migrations/installcommand.cpp
        commands/migrations/migratecommand.cpp
        commands/migrations/refreshcommand.cpp
        commands/migrations/resetcommand.cpp
        commands/migrations/rollbackcommand.cpp
        commands/migrations/statuscommand.cpp
        concerns/callscommands.cpp
        concerns/confirmable.cpp
        concerns/guesscommandname.cpp
        concerns/interactswithio.cpp
        concerns/printsoptions.cpp
        concerns/usingconnection.cpp
        exceptions/tomlogicerror.cpp
        exceptions/tomruntimeerror.cpp
        migrationcreator.cpp
        migrationrepository.cpp
        migrator.cpp
        seeder.cpp
        tableguesser.cpp
        terminal.cpp
        tomutils.cpp
    )

    list(SORT headers)
    list(SORT sources)

    list(TRANSFORM headers PREPEND "${CMAKE_SOURCE_DIR}/tom/include/tom/")
    list(TRANSFORM sources PREPEND "${CMAKE_SOURCE_DIR}/tom/src/tom/")

    set(${out_headers} ${headers} PARENT_SCOPE)
    set(${out_sources} ${sources} PARENT_SCOPE)
endfunction()
