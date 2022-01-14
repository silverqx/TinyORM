# TinyORM library header and source files
# Create header and source files lists and return them
function(tiny_sources out_headers out_sources)

    set(headers)

    if(TINY_EXTERN_CONSTANTS)
        list(APPEND headers constants_extern.hpp)
    else()
        list(APPEND headers constants_inline.hpp)
    endif()

    list(APPEND headers
        basegrammar.hpp
        concerns/countsqueries.hpp
        concerns/detectslostconnections.hpp
        concerns/hasconnectionresolver.hpp
        concerns/logsqueries.hpp
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
        schema/grammars/mysqlschemagrammar.hpp
        schema/grammars/postgresschemagrammar.hpp
        schema/grammars/schemagrammar.hpp
        schema/grammars/sqliteschemagrammar.hpp
        schema/mysqlschemabuilder.hpp
        schema/postgresschemabuilder.hpp
        schema/schemabuilder.hpp
        schema/sqliteschemabuilder.hpp
        sqliteconnection.hpp
        support/configurationoptionsparser.hpp
        support/databaseconfiguration.hpp
        support/databaseconnectionsmap.hpp
        types/log.hpp
        types/statementscounter.hpp
        utils/fs.hpp
        utils/query.hpp
        utils/thread.hpp
        utils/type.hpp
        version.hpp
    )

    if(ORM)
        list(APPEND headers
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
            tiny/tinybuilder.hpp
            tiny/tinybuilderproxies.hpp
            tiny/tinyconcepts.hpp
            tiny/tinytypes.hpp
            tiny/types/connectionoverride.hpp
            tiny/types/syncchanges.hpp
            tiny/utils/attribute.hpp
            tiny/utils/string.hpp
        )
    endif()

    set(sources)

    if(TINY_EXTERN_CONSTANTS)
        list(APPEND sources constants_extern.cpp)
    endif()

    list(APPEND sources
        basegrammar.cpp
        concerns/countsqueries.cpp
        concerns/detectslostconnections.cpp
        concerns/hasconnectionresolver.cpp
        concerns/logsqueries.cpp
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
        schema/grammars/mysqlschemagrammar.cpp
        schema/grammars/postgresschemagrammar.cpp
        schema/grammars/sqliteschemagrammar.cpp
        schema/mysqlschemabuilder.cpp
        schema/postgresschemabuilder.cpp
        schema/schemabuilder.cpp
        schema/sqliteschemabuilder.cpp
        sqliteconnection.cpp
        support/configurationoptionsparser.cpp
        utils/fs.cpp
        utils/query.cpp
        utils/thread.cpp
        utils/type.cpp
    )

    if(ORM)
        list(APPEND sources
            tiny/exceptions/modelnotfounderror.cpp
            tiny/exceptions/relationnotfounderror.cpp
            tiny/exceptions/relationnotloadederror.cpp
            tiny/model.cpp
            tiny/relations/relation.cpp
            tiny/tinytypes.cpp
            tiny/types/syncchanges.cpp
            tiny/utils/attribute.cpp
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
