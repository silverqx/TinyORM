INCLUDEPATH *= $$PWD

headersList =

extern_constants: \
    headersList += \
        $$PWD/tom/tomconstants_extern.hpp
else: \
    headersList += \
        $$PWD/tom/tomconstants_inline.hpp

# There is no need to compile the bash completion for other platforms.
# The pwsh can run on all our supported platforms so always compile it.
linux|mingw: \
    headersList += \
        $$PWD/tom/commands/complete/bashcommand.hpp

headersList += \
    $$PWD/tom/application.hpp \
    $$PWD/tom/commands/aboutcommand.hpp \
    $$PWD/tom/commands/command.hpp \
    $$PWD/tom/commands/complete/basecompletecommand.hpp \
    $$PWD/tom/commands/complete/completetypes.hpp \
    $$PWD/tom/commands/complete/pwshcommand.hpp \
    $$PWD/tom/commands/database/seedcommand.hpp \
    $$PWD/tom/commands/database/wipecommand.hpp \
    $$PWD/tom/commands/environmentcommand.hpp \
    $$PWD/tom/commands/helpcommand.hpp \
    $$PWD/tom/commands/inspirecommand.hpp \
    $$PWD/tom/commands/integratecommand.hpp \
    $$PWD/tom/commands/listcommand.hpp \
    $$PWD/tom/commands/make/concerns/prepareoptionvalues.hpp \
    $$PWD/tom/commands/make/makecommand.hpp \
    $$PWD/tom/commands/make/migrationcommand.hpp \
    $$PWD/tom/commands/make/modelcommand.hpp \
    $$PWD/tom/commands/make/modelcommandconcepts.hpp \
    $$PWD/tom/commands/make/modelcommandtypes.hpp \
#    $$PWD/tom/commands/make/projectcommand.hpp \
    $$PWD/tom/commands/make/seedercommand.hpp \
    $$PWD/tom/commands/make/stubs/migrationstubs.hpp \
    $$PWD/tom/commands/make/stubs/modelstubs.hpp \
#    $$PWD/tom/commands/make/stubs/projectstubs.hpp \
    $$PWD/tom/commands/make/stubs/seederstubs.hpp \
    $$PWD/tom/commands/make/support/migrationcreator.hpp \
    $$PWD/tom/commands/make/support/modelcreator.hpp \
    $$PWD/tom/commands/make/support/preparebtmoptionvalues.hpp \
    $$PWD/tom/commands/make/support/prepareforeignkeyvalues.hpp \
    $$PWD/tom/commands/make/support/seedercreator.hpp \
    $$PWD/tom/commands/make/support/tableguesser.hpp \
    $$PWD/tom/commands/migrations/freshcommand.hpp \
    $$PWD/tom/commands/migrations/installcommand.hpp \
    $$PWD/tom/commands/migrations/migratecommand.hpp \
    $$PWD/tom/commands/migrations/refreshcommand.hpp \
    $$PWD/tom/commands/migrations/resetcommand.hpp \
    $$PWD/tom/commands/migrations/rollbackcommand.hpp \
    $$PWD/tom/commands/migrations/statuscommand.hpp \
    $$PWD/tom/commands/migrations/uninstallcommand.hpp \
    $$PWD/tom/commands/stubs/integratestubs.hpp \
    $$PWD/tom/concerns/callscommands.hpp \
    $$PWD/tom/concerns/confirmable.hpp \
    $$PWD/tom/concerns/guesscommandname.hpp \
    $$PWD/tom/concerns/interactswithio.hpp \
    $$PWD/tom/concerns/pretendable.hpp \
    $$PWD/tom/concerns/printsoptions.hpp \
    $$PWD/tom/concerns/usingconnection.hpp \
    $$PWD/tom/config.hpp \
    $$PWD/tom/exceptions/invalidargumenterror.hpp \
    $$PWD/tom/exceptions/invalidtemplateargumenterror.hpp \
    $$PWD/tom/exceptions/logicerror.hpp \
    $$PWD/tom/exceptions/runtimeerror.hpp \
    $$PWD/tom/exceptions/tomerror.hpp \
    $$PWD/tom/migration.hpp \
    $$PWD/tom/migrationrepository.hpp \
    $$PWD/tom/migrator.hpp \
    $$PWD/tom/seeder.hpp \
    $$PWD/tom/terminal.hpp \
    $$PWD/tom/tomconstants.hpp \
    $$PWD/tom/tomtypes.hpp \
    $$PWD/tom/tomutils.hpp \
    $$PWD/tom/types/commandlineoption.hpp \
    $$PWD/tom/types/guesscommandnametype.hpp \
    $$PWD/tom/version.hpp \

HEADERS += $$sorted(headersList)

unset(headersList)
