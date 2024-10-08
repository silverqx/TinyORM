# TinyDrivers library header and source files
# Create header and source files lists and return them
function(tinydrivers_sources out_headers_private out_headers out_sources)

    # Private Header files section
    set(headers_private "")

    if(TINY_EXTERN_CONSTANTS)
        list(APPEND headers_private constants_extern_p.hpp)
    else()
        list(APPEND headers_private constants_inline_p.hpp)
    endif()

    if(TINY_BUILD_LOADABLE_DRIVERS)
        list(APPEND headers_private utils/fs_p.hpp)
    endif()

    list(APPEND headers_private
        constants_p.hpp
        macros/declaresqldriverprivate_p.hpp
        sqldatabase_p.hpp
        sqldriver_p.hpp
        sqlresult_p.hpp
        support/connectionshash_p.hpp
        support/sqldriverfactory_p.hpp
        support/sqlrecordcache_p.hpp
        utils/type_p.hpp
    )

    # Header files section
    set(headers "")

    list(APPEND headers
        concerns/selectsallcolumnswithlimit0.hpp
        driverstypes.hpp
        dummysqlerror.hpp
        exceptions/driverserror.hpp
        exceptions/invalidargumenterror.hpp
        exceptions/logicerror.hpp
        exceptions/outofrangeerror.hpp
        exceptions/queryerror.hpp
        exceptions/runtimeerror.hpp
        exceptions/sqlerror.hpp
        exceptions/sqltransactionerror.hpp
        libraryinfo.hpp
        macros/export.hpp
        sqldatabase.hpp
        sqldatabasemanager.hpp
        sqldriver.hpp
        sqlfield.hpp
        sqlquery.hpp
        sqlrecord.hpp
        sqlresult.hpp
        utils/notnull.hpp
        version.hpp
    )

    # Source files section
    set(sources "")

    if(TINY_EXTERN_CONSTANTS)
        list(APPEND sources constants_extern_p.cpp)
    endif()

    if(TINY_BUILD_LOADABLE_DRIVERS)
        list(APPEND sources utils/fs_p.cpp)
    endif()

    list(APPEND sources
        concerns/selectsallcolumnswithlimit0.cpp
        dummysqlerror.cpp
        exceptions/logicerror.cpp
        exceptions/queryerror.cpp
        exceptions/runtimeerror.cpp
        exceptions/sqlerror.cpp
        libraryinfo.cpp
        sqldatabase.cpp
        sqldatabase_p.cpp
        sqldatabasemanager.cpp
        sqldriver.cpp
        sqlfield.cpp
        sqlquery.cpp
        sqlrecord.cpp
        sqlresult.cpp
        sqlresult_p.cpp
        support/sqldriverfactory_p.cpp
        utils/type_p.cpp
    )

    list(SORT headers_private)
    list(SORT headers)
    list(SORT sources)

    set(sourceDir "${${TinyDrivers_ns}_SOURCE_DIR}")
    set(suffixDir "orm/drivers/")

    list(TRANSFORM headers_private PREPEND "${sourceDir}/include_private/${suffixDir}")
    list(TRANSFORM headers         PREPEND "${sourceDir}/include/${suffixDir}")
    list(TRANSFORM sources         PREPEND "${sourceDir}/src/${suffixDir}")

    set(${out_headers_private} ${headers_private} PARENT_SCOPE)
    set(${out_headers} ${headers} PARENT_SCOPE)
    set(${out_sources} ${sources} PARENT_SCOPE)

endfunction()

# TinyMySql library header and source files
# Create header and source files lists and return them
function(tinymysqldriver_sources out_headers_private out_headers out_sources)

    # Private Header files section
    set(headers_private "")

    if(TINY_EXTERN_CONSTANTS)
        list(APPEND headers_private mysqlconstants_extern_p.hpp)
    else()
        list(APPEND headers_private mysqlconstants_inline_p.hpp)
    endif()

    list(APPEND headers_private
        concerns/populatesfielddefaultvalues_p.hpp
        macros/includemysqlh_p.hpp
        mysqlconstants_p.hpp
        mysqldriver_p.hpp
        mysqlresult_p.hpp
        mysqltypes_p.hpp
        mysqlutils_p.hpp
    )

    # Header files section
    set(headers "")

    list(APPEND headers
        mysqldriver.hpp
        mysqlresult.hpp
        version.hpp
    )

    # Source files section
    set(sources "")

    if(TINY_EXTERN_CONSTANTS)
        list(APPEND sources mysqlconstants_extern_p.cpp)
    endif()

    if(TINY_BUILD_LOADABLE_DRIVERS)
        list(APPEND sources main.cpp)
    endif()

    list(APPEND sources
        concerns/populatesfielddefaultvalues_p.cpp
        mysqldriver.cpp
        mysqldriver_p.cpp
        mysqlresult.cpp
        mysqlresult_p.cpp
        mysqlutils_p.cpp
    )

    list(SORT headers_private)
    list(SORT headers)
    list(SORT sources)

    set(sourceDir "${${TinyOrm_ns}_SOURCE_DIR}/drivers/mysql")
    set(suffixDir "orm/drivers/mysql/")

    list(TRANSFORM headers_private PREPEND "${sourceDir}/include_private/${suffixDir}")
    list(TRANSFORM headers         PREPEND "${sourceDir}/include/${suffixDir}")
    list(TRANSFORM sources         PREPEND "${sourceDir}/src/${suffixDir}")

    set(${out_headers_private} ${headers_private} PARENT_SCOPE)
    set(${out_headers} ${headers} PARENT_SCOPE)
    set(${out_sources} ${sources} PARENT_SCOPE)

endfunction()

# TinyORM library header and source files
# Create header and source files lists and return them
function(tinyorm_sources out_headers out_sources)

    # Header files section
    set(headers "")

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

    # TinyOrm library compiled against the TinyDrivers doesn't use these exception classes
    if(NOT BUILD_DRIVERS)
        list(APPEND headers
            exceptions/queryerror.hpp
            exceptions/sqlerror.hpp
            exceptions/sqltransactionerror.hpp
        )
    endif()

    list(APPEND headers
        basegrammar.hpp
        concerns/countsqueries.hpp
        concerns/detectslostconnections.hpp
        concerns/hasconnectionresolver.hpp
        concerns/logsqueries.hpp
        concerns/managestransactions.hpp
        concerns/parsessearchpath.hpp
        connectionresolverinterface.hpp
        connectors/connectionfactory.hpp
        connectors/connector.hpp
        config.hpp
        configurations/configurationoptionsparser.hpp
        configurations/configurationparser.hpp
        configurations/configurationparserfactory.hpp
        configurations/configurationparserinterface.hpp
        configurations/mysqlconfigurationparser.hpp
        configurations/postgresconfigurationparser.hpp
        configurations/sqliteconfigurationparser.hpp
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
        exceptions/lostconnectionerror.hpp
        exceptions/multiplerecordsfounderror.hpp
        exceptions/ormerror.hpp
        exceptions/outofrangeerror.hpp
        exceptions/recordsnotfounderror.hpp
        exceptions/runtimeerror.hpp
        exceptions/searchpathemptyerror.hpp
        exceptions/sqlitedatabasedoesnotexisterror.hpp
        libraryinfo.hpp
        macros/archdetect.hpp
        macros/commonnamespace.hpp
        macros/compilerdetect.hpp
        macros/export.hpp
        macros/export_common.hpp
        macros/likely.hpp
        macros/logexecutedquery.hpp
        macros/sqldrivermappings.hpp
        macros/stringify.hpp
        macros/systemheader.hpp
        macros/threadlocal.hpp
        mysqlconnection.hpp
        ormconcepts.hpp
        ormtypes.hpp
        postgresconnection.hpp
        query/concerns/buildsqueries.hpp
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
        schema/indexdefinitionreference.hpp
        schema/mysqlschemabuilder.hpp
        schema/postgresschemabuilder.hpp
        schema/schemabuilder.hpp
        schema/schemaconstants.hpp
        schema/schematypes.hpp
        schema/sqliteschemabuilder.hpp
        sqliteconnection.hpp
        support/databaseconfiguration.hpp
        support/databaseconnectionsmap.hpp
        support/replacebindings.hpp
        types/aboutvalue.hpp
        types/log.hpp
        types/sqlquery.hpp
        types/statementscounter.hpp
        utils/configuration.hpp
        utils/container.hpp
        utils/fs.hpp
        utils/helpers.hpp
        utils/integralcast.hpp
        utils/notnull.hpp
        utils/nullvariant.hpp
        utils/query.hpp
        utils/string.hpp
        utils/thread.hpp
        utils/type.hpp
        version.hpp
    )

    if(ORM)
        list(APPEND headers
            tiny/casts/attribute.hpp
            tiny/concerns/buildsqueries.hpp
            tiny/concerns/buildssoftdeletes.hpp
            tiny/concerns/guardedmodel.hpp
            tiny/concerns/guardsattributes.hpp
            tiny/concerns/hasattributes.hpp
            tiny/concerns/hasrelationships.hpp
            tiny/concerns/hasrelationstore.hpp
            tiny/concerns/hastimestamps.hpp
            tiny/concerns/hidesattributes.hpp
            tiny/concerns/queriesrelationships.hpp
            tiny/exceptions/massassignmenterror.hpp
            tiny/exceptions/modelnotfounderror.hpp
            tiny/exceptions/mutatormappingnotfounderror.hpp
            tiny/exceptions/relationmappingnotfounderror.hpp
            tiny/exceptions/relationnotloadederror.hpp
            tiny/macros/crtpmodel.hpp
            tiny/macros/crtpmodelwithbase.hpp
            tiny/macros/relationstoresaliases.hpp
            tiny/model.hpp
            tiny/modelproxies.hpp
            tiny/relations/basepivot.hpp
            tiny/relations/belongsto.hpp
            tiny/relations/belongstomany.hpp
            tiny/relations/concerns/comparesrelatedmodels.hpp
            tiny/relations/concerns/interactswithpivottable.hpp
            tiny/relations/concerns/supportsdefaultmodels.hpp
            tiny/relations/hasmany.hpp
            tiny/relations/hasone.hpp
            tiny/relations/hasoneormany.hpp
            tiny/relations/pivot.hpp
            tiny/relations/relation.hpp
            tiny/relations/relationproxies.hpp
            tiny/relations/relationtypes.hpp
            tiny/softdeletes.hpp
            tiny/support/stores/baserelationstore.hpp
            tiny/support/stores/belongstomanyrelatedtablestore.hpp
            tiny/support/stores/eagerrelationstore.hpp
            tiny/support/stores/lazyrelationstore.hpp
            tiny/support/stores/pushrelationstore.hpp
            tiny/support/stores/queriesrelationshipsstore.hpp
            tiny/support/stores/serializerelationstore.hpp
            tiny/support/stores/touchownersrelationstore.hpp
            tiny/tinybuilder.hpp
            tiny/tinybuilderproxies.hpp
            tiny/tinyconcepts.hpp
            tiny/tinytypes.hpp
            tiny/types/connectionoverride.hpp
            tiny/types/modelattributes.hpp
            tiny/types/modelscollection.hpp
            tiny/types/syncchanges.hpp
            tiny/utils/attribute.hpp
        )
    endif()

    # Source files section
    set(sources "")

    if(TINY_EXTERN_CONSTANTS)
        list(APPEND sources
            constants_extern.cpp
            schema/schemaconstants_extern.cpp
        )
    endif()

    # TinyOrm library compiled against the TinyDrivers doesn't use these exception classes
    if(NOT BUILD_DRIVERS)
        list(APPEND sources
            exceptions/queryerror.cpp
            exceptions/sqlerror.cpp
        )
    endif()

    list(APPEND sources
        basegrammar.cpp
        concerns/countsqueries.cpp
        concerns/detectslostconnections.cpp
        concerns/hasconnectionresolver.cpp
        concerns/logsqueries.cpp
        concerns/managestransactions.cpp
        concerns/parsessearchpath.cpp
        configurations/configurationoptionsparser.cpp
        configurations/configurationparser.cpp
        configurations/configurationparserfactory.cpp
        configurations/mysqlconfigurationparser.cpp
        configurations/postgresconfigurationparser.cpp
        configurations/sqliteconfigurationparser.cpp
        connectors/connectionfactory.cpp
        connectors/connector.cpp
        connectors/mysqlconnector.cpp
        connectors/postgresconnector.cpp
        connectors/sqliteconnector.cpp
        databaseconnection.cpp
        databasemanager.cpp
        db.cpp
        exceptions/logicerror.cpp
        exceptions/runtimeerror.cpp
        libraryinfo.cpp
        mysqlconnection.cpp
        postgresconnection.cpp
        query/concerns/buildsqueries.cpp
        query/grammars/grammar.cpp
        query/grammars/mysqlgrammar.cpp
        query/grammars/postgresgrammar.cpp
        query/grammars/sqlitegrammar.cpp
        query/joinclause.cpp
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
        schema/indexdefinitionreference.cpp
        schema/mysqlschemabuilder.cpp
        schema/postgresschemabuilder.cpp
        schema/schemabuilder.cpp
        schema/sqliteschemabuilder.cpp
        sqliteconnection.cpp
        types/sqlquery.cpp
        utils/configuration.cpp
        utils/fs.cpp
        utils/helpers.cpp
        utils/nullvariant.cpp
        utils/query.cpp
        utils/string.cpp
        utils/thread.cpp
        utils/type.cpp
    )

    if(ORM)
        list(APPEND sources
            tiny/concerns/guardedmodel.cpp
            tiny/exceptions/modelnotfounderror.cpp
            tiny/exceptions/relationmappingnotfounderror.cpp
            tiny/exceptions/relationnotloadederror.cpp
            tiny/tinytypes.cpp
            tiny/utils/attribute.cpp
        )
    endif()

    list(SORT headers)
    list(SORT sources)

    list(TRANSFORM headers PREPEND "include/orm/")
    list(TRANSFORM sources PREPEND "src/orm/")

    set(${out_headers} ${headers} PARENT_SCOPE)
    set(${out_sources} ${sources} PARENT_SCOPE)

endfunction()

# Tom console application header and source files
# Create header and source files lists and return them
function(tinytom_sources out_headers out_sources)

    # Tom header files section
    set(headers "")

    if(TINY_EXTERN_CONSTANTS)
        list(APPEND headers tomconstants_extern.hpp)
    else()
        list(APPEND headers tomconstants_inline.hpp)
    endif()

    # There is no need to compile the bash completion for other platforms.
    # The pwsh can run on all our supported platforms so always compile it.
    if(MINGW OR CMAKE_SYSTEM_NAME STREQUAL "Linux")
        list(APPEND headers commands/complete/bashcommand.hpp)
    endif()

    list(APPEND headers
        application.hpp
        commands/aboutcommand.hpp
        commands/command.hpp
        commands/complete/basecompletecommand.hpp
        commands/complete/completetypes.hpp
        commands/complete/pwshcommand.hpp
        commands/database/seedcommand.hpp
        commands/database/wipecommand.hpp
        commands/environmentcommand.hpp
        commands/helpcommand.hpp
        commands/inspirecommand.hpp
        commands/integratecommand.hpp
        commands/listcommand.hpp
        commands/make/concerns/prepareoptionvalues.hpp
        commands/make/makecommand.hpp
        commands/make/migrationcommand.hpp
        commands/make/modelcommand.hpp
        commands/make/modelcommandconcepts.hpp
        commands/make/modelcommandtypes.hpp
#        commands/make/projectcommand.hpp
        commands/make/seedercommand.hpp
        commands/make/stubs/migrationstubs.hpp
        commands/make/stubs/modelstubs.hpp
#        commands/make/stubs/projectstubs.hpp
        commands/make/stubs/seederstubs.hpp
        commands/make/support/migrationcreator.hpp
        commands/make/support/modelcreator.hpp
        commands/make/support/preparebtmoptionvalues.hpp
        commands/make/support/prepareforeignkeyvalues.hpp
        commands/make/support/seedercreator.hpp
        commands/make/support/tableguesser.hpp
        commands/migrations/freshcommand.hpp
        commands/migrations/installcommand.hpp
        commands/migrations/migratecommand.hpp
        commands/migrations/refreshcommand.hpp
        commands/migrations/resetcommand.hpp
        commands/migrations/rollbackcommand.hpp
        commands/migrations/statuscommand.hpp
        commands/migrations/uninstallcommand.hpp
        commands/stubs/integratestubs.hpp
        concerns/callscommands.hpp
        concerns/confirmable.hpp
        concerns/guesscommandname.hpp
        concerns/interactswithio.hpp
        concerns/pretendable.hpp
        concerns/printsoptions.hpp
        concerns/usingconnection.hpp
        config.hpp
        exceptions/invalidargumenterror.hpp
        exceptions/invalidtemplateargumenterror.hpp
        exceptions/logicerror.hpp
        exceptions/runtimeerror.hpp
        exceptions/tomerror.hpp
        migration.hpp
        migrationrepository.hpp
        migrator.hpp
        seeder.hpp
        terminal.hpp
        tomconstants.hpp
        tomtypes.hpp
        tomutils.hpp
        types/commandlineoption.hpp
        types/guessedcommandname.hpp
        version.hpp
    )

    # Tom source files section
    set(sources "")

    if(TINY_EXTERN_CONSTANTS)
        list(APPEND sources tomconstants_extern.cpp)
    endif()

    # There is no need to compile the bash completion for other platforms.
    # The pwsh can run on all our supported platforms so always compile it.
    if(MINGW OR CMAKE_SYSTEM_NAME STREQUAL "Linux")
        list(APPEND sources commands/complete/bashcommand.cpp)
    endif()

    list(APPEND sources
        application.cpp
        commands/aboutcommand.cpp
        commands/command.cpp
        commands/complete/basecompletecommand.cpp
        commands/complete/pwshcommand.cpp
        commands/database/seedcommand.cpp
        commands/database/wipecommand.cpp
        commands/environmentcommand.cpp
        commands/helpcommand.cpp
        commands/inspirecommand.cpp
        commands/integratecommand.cpp
        commands/listcommand.cpp
        commands/make/concerns/prepareoptionvalues.cpp
        commands/make/makecommand.cpp
        commands/make/migrationcommand.cpp
        commands/make/modelcommand.cpp
#        commands/make/projectcommand.cpp
        commands/make/seedercommand.cpp
        commands/make/support/migrationcreator.cpp
        commands/make/support/modelcreator.cpp
        commands/make/support/prepareforeignkeyvalues.cpp
        commands/make/support/seedercreator.cpp
        commands/make/support/tableguesser.cpp
        commands/migrations/freshcommand.cpp
        commands/migrations/installcommand.cpp
        commands/migrations/migratecommand.cpp
        commands/migrations/refreshcommand.cpp
        commands/migrations/resetcommand.cpp
        commands/migrations/rollbackcommand.cpp
        commands/migrations/statuscommand.cpp
        commands/migrations/uninstallcommand.cpp
        concerns/callscommands.cpp
        concerns/confirmable.cpp
        concerns/guesscommandname.cpp
        concerns/interactswithio.cpp
        concerns/pretendable.cpp
        concerns/printsoptions.cpp
        concerns/usingconnection.cpp
        exceptions/tomlogicerror.cpp
        exceptions/tomruntimeerror.cpp
        migrationrepository.cpp
        migrator.cpp
        seeder.cpp
        terminal.cpp
        tomutils.cpp
    )

    list(SORT headers)
    list(SORT sources)

    list(TRANSFORM headers PREPEND "${PROJECT_SOURCE_DIR}/tom/include/tom/")
    list(TRANSFORM sources PREPEND "${PROJECT_SOURCE_DIR}/tom/src/tom/")

    set(${out_headers} ${headers} PARENT_SCOPE)
    set(${out_sources} ${sources} PARENT_SCOPE)

endfunction()

# Models header and source files (primarily used in AutoTests)
# Create header and source files lists and return them
function(tiny_model_sources out_headers out_sources)

    # Header files section
    set(headers "")

    # Model Include lists (used to avoid duplicate #include-s) 😮
    list(APPEND headers
        album_includeslist.hpp
        torrent_includeslist.hpp
        torrent_returnrelation_includeslist.hpp
        torrenteager_includeslist.hpp
        torrentpeereager_includeslist.hpp
        torrentpreviewablefileeager_withdefault_includeslist.hpp
    )

    # Individual Model classes
    list(APPEND headers
        album.hpp
        albumimage.hpp
        datetime.hpp
        datetime_serializeoverride.hpp
        filepropertyproperty.hpp
        massassignmentmodels.hpp
        phone.hpp
        role.hpp
        roleuser.hpp
        roleuser_appends.hpp
        setting.hpp
        tag.hpp
        tag_basicpivot_norelations.hpp
        tag_custompivot_norelations.hpp
        tagged.hpp
        tagproperty.hpp
        torrent.hpp
        torrent_norelations.hpp
        torrent_returnrelation.hpp
        torrent_with_equality_operator.hpp
        torrent_with_qdatetime.hpp
        torrent_without_qdatetime.hpp
        torrenteager.hpp
        torrenteager_failed.hpp
        torrentpeer.hpp
        torrentpeer_norelations.hpp
        torrentpeereager.hpp
        torrentpreviewablefile.hpp
        torrentpreviewablefile_norelations.hpp
        torrentpreviewablefileeager.hpp
        torrentpreviewablefileeager_withdefault.hpp
        torrentpreviewablefileproperty.hpp
        torrentpreviewablefileproperty_norelations.hpp
        torrentstate.hpp
        type.hpp
        user.hpp
        user_norelations.hpp
    )

    # Source files section (empty)
    set(sources "")

    list(SORT headers)

    list(TRANSFORM headers PREPEND "${${TinyOrm_ns}_SOURCE_DIR}/tests/models/models/")

    set(${out_headers} ${headers} PARENT_SCOPE)
    set(${out_sources} ${sources} PARENT_SCOPE)

endfunction()

# Database migrations header files (used in AutoTests and Tom example console application)
# Create header and source files lists and return them
function(tiny_tests_migration_sources out_headers)

    # Header files section
    set(headers "")

    list(APPEND headers
        2014_10_12_000000_create_posts_table.hpp
        2014_10_12_100000_add_factor_column_to_posts_table.hpp
        2014_10_12_200000_create_properties_table.hpp
        2014_10_12_300000_create_phones_table.hpp
    )

    list(SORT headers)

    list(TRANSFORM headers
        PREPEND "${${TinyOrm_ns}_SOURCE_DIR}/tests/database/migrations/"
    )

    set(${out_headers} ${headers} PARENT_SCOPE)

endfunction()

# Database seeders header files (used in the Tom example console application)
# Create header and source files lists and return them
function(tiny_tests_seeder_sources out_headers)

    # Header files section
    set(headers "")

    list(APPEND headers
        databaseseeder.hpp
        phoneseeder.hpp
        propertyseeder.hpp
    )

    list(SORT headers)

    list(TRANSFORM headers PREPEND "${${TinyOrm_ns}_SOURCE_DIR}/tests/database/seeders/")

    set(${out_headers} ${headers} PARENT_SCOPE)

endfunction()

# Tom example console application - database migrations and seeders header files
# Create header and source files lists and return them
function(tiny_tom_example_database_sources out_headers)

    # Header files section
    set(migration_headers "")
    set(seeder_headers "")

    # Migrations
    tiny_tests_migration_sources(migration_headers)
    # Seeders
    tiny_tests_seeder_sources(seeder_headers)

    set(headers "")
    list(APPEND headers ${migration_headers} ${seeder_headers})

    list(SORT headers)

    set(${out_headers} ${headers} PARENT_SCOPE)

endfunction()

# Tom testdata application - database migrations and seeder header files
# Create header and source files lists and return them
function(tiny_tom_testdata_database_sources out_headers)

    # Header files section
    set(headers "")

    list(APPEND headers
        # Migrations
        migrations/2022_05_11_170000_create_users_table.hpp
        migrations/2022_05_11_170100_create_roles_table.hpp
        migrations/2022_05_11_170200_create_role_user_table.hpp
        migrations/2022_05_11_170300_create_user_phones_table.hpp
        migrations/2022_05_11_170400_create_settings_table.hpp
        migrations/2022_05_11_170500_create_torrents_table.hpp
        migrations/2022_05_11_170600_create_torrent_peers_table.hpp
        migrations/2022_05_11_170700_create_torrent_previewable_files_table.hpp
        migrations/2022_05_11_170800_create_torrent_previewable_file_properties_table.hpp
        migrations/2022_05_11_170900_create_file_property_properties_table.hpp
        migrations/2022_05_11_171000_create_torrent_tags_table.hpp
        migrations/2022_05_11_171100_create_tag_torrent_table.hpp
        migrations/2022_05_11_171200_create_tag_properties_table.hpp
        migrations/2022_05_11_171300_create_types_table.hpp
        migrations/2022_05_11_171400_create_datetimes_table.hpp
        migrations/2022_05_11_171500_create_albums_table.hpp
        migrations/2022_05_11_171600_create_album_images_table.hpp
        migrations/2022_05_11_171700_create_torrent_states_table.hpp
        migrations/2022_05_11_171800_create_state_torrent_table.hpp
        migrations/2022_05_11_171900_create_role_tag_table.hpp
        migrations/2022_05_11_172000_create_empty_with_default_values_table.hpp

        # Seeders
        seeders/databaseseeder.hpp
    )

    list(SORT headers)

    list(TRANSFORM headers
        PREPEND "${${TinyOrm_ns}_SOURCE_DIR}/tests/testdata_tom/database/"
    )

    set(${out_headers} ${headers} PARENT_SCOPE)

endfunction()
