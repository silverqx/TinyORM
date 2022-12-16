#include "tom/application.hpp"

#include <QCoreApplication>
#include <QDebug>

#include <range/v3/algorithm/contains.hpp>
#include <range/v3/view/zip_with.hpp>

#include <orm/databasemanager.hpp>
#include <orm/macros/likely.hpp>
#include <orm/utils/type.hpp>
#include <orm/version.hpp>

#include "tom/commands/completecommand.hpp"
#include "tom/commands/database/seedcommand.hpp"
#include "tom/commands/database/wipecommand.hpp"
#include "tom/commands/environmentcommand.hpp"
#include "tom/commands/helpcommand.hpp"
#include "tom/commands/inspirecommand.hpp"
#include "tom/commands/integratecommand.hpp"
#include "tom/commands/listcommand.hpp"
#include "tom/commands/make/migrationcommand.hpp"
#include "tom/commands/make/modelcommand.hpp"
//#include "tom/commands/make/projectcommand.hpp"
#include "tom/commands/make/seedercommand.hpp"
#include "tom/commands/migrations/freshcommand.hpp"
#include "tom/commands/migrations/installcommand.hpp"
#include "tom/commands/migrations/migratecommand.hpp"
#include "tom/commands/migrations/refreshcommand.hpp"
#include "tom/commands/migrations/resetcommand.hpp"
#include "tom/commands/migrations/rollbackcommand.hpp"
#include "tom/commands/migrations/statuscommand.hpp"
#include "tom/commands/migrations/uninstallcommand.hpp"
#include "tom/exceptions/runtimeerror.hpp"
#include "tom/migrationrepository.hpp"
#include "tom/migrator.hpp"
#include "tom/terminal.hpp"
#include "tom/tomutils.hpp"
#ifndef TINYTOM_TESTS_CODE
#  include "tom/version.hpp"
#endif

/*! Invoke Qt's global post routines. */
extern void Q_DECL_IMPORT qt_call_post_routines();

TINYORM_BEGIN_COMMON_NAMESPACE

using fspath = std::filesystem::path;

using Orm::ConnectionResolverInterface;
using Orm::Constants::EMPTY;
using Orm::Constants::NEWLINE;
using Orm::Constants::TMPL_ONE;
using Orm::Constants::Version;

using TypeUtils = Orm::Utils::Type;

using Tom::Commands::Command;
using Tom::Commands::CompleteCommand;
using Tom::Commands::Database::SeedCommand;
using Tom::Commands::Database::WipeCommand;
using Tom::Commands::EnvironmentCommand;
using Tom::Commands::HelpCommand;
using Tom::Commands::InspireCommand;
using Tom::Commands::IntegrateCommand;
using Tom::Commands::ListCommand;
using Tom::Commands::Make::MigrationCommand;
using Tom::Commands::Make::ModelCommand;
//using Tom::Commands::Make::ProjectCommand;
using Tom::Commands::Make::SeederCommand;
using Tom::Commands::Migrations::FreshCommand;
using Tom::Commands::Migrations::InstallCommand;
using Tom::Commands::Migrations::MigrateCommand;
using Tom::Commands::Migrations::RefreshCommand;
using Tom::Commands::Migrations::ResetCommand;
using Tom::Commands::Migrations::RollbackCommand;
using Tom::Commands::Migrations::StatusCommand;
using Tom::Commands::Migrations::UninstallCommand;

using Tom::Constants::Complete;
using Tom::Constants::DbSeed;
using Tom::Constants::DbWipe;
using Tom::Constants::Env;
using Tom::Constants::Help;
using Tom::Constants::Inspire;
using Tom::Constants::Integrate;
using Tom::Constants::List;
using Tom::Constants::MakeMigration;
using Tom::Constants::MakeModel;
//using Tom::Constants::MakeProject;
using Tom::Constants::MakeSeeder;
using Tom::Constants::Migrate;
using Tom::Constants::MigrateFresh;
using Tom::Constants::MigrateInstall;
using Tom::Constants::MigrateRefresh;
using Tom::Constants::MigrateReset;
using Tom::Constants::MigrateRollback;
using Tom::Constants::MigrateStatus;
using Tom::Constants::MigrateUninstall;
using Tom::Constants::NsAll;
using Tom::Constants::NsDb;
using Tom::Constants::NsGlobal;
using Tom::Constants::NsMake;
using Tom::Constants::NsMigrate;
using Tom::Constants::NsNamespaced;
using Tom::Constants::ansi;
using Tom::Constants::env;
using Tom::Constants::env_up;
using Tom::Constants::help;
using Tom::Constants::noansi;
using Tom::Constants::nointeraction;
using Tom::Constants::quiet;
using Tom::Constants::verbose;

using TomUtils = Tom::Utils;

namespace Tom {

/* Adding/removing/disabling/enabling a command, add #include, using, factory in the
   Application::createCommand(), add a command name to the Application::commandNames(),
   update indexes in the Application::commandsIndexes() and if the command introduces
   a new namespace add it to the Application::namespaceNames().
   I have everything extracted and placed it to the bottom of application.cpp so it is
   nicely in one place. */

/* The m_options list is used by the help, list, and complete commands. For help and
   complete commands it's clear where they use the m_options list. The list command
   uses it to show common options, before it "lists commands".
   The m_options lists saves a copy of all options or only common options, as is needed
   by the current operation or command that uses this m_options data member.
   It's initialized in the Application::initializeParser() by the common options and
   the remaining options will be prepended by the help command
   in the HelpCommand::printOptionsSection().
   The complete command uses another approach to obtain all options
   in the CompleteCommand::getCommandOptionsSignature(). */

/* public */

Application::Application(int &argc, char **argv, std::shared_ptr<DatabaseManager> db,
                         const char *const environmentEnvName, QString migrationTable,
                         std::vector<std::shared_ptr<Migration>> migrations,
                         std::vector<std::shared_ptr<Seeder>> seeders)
    : m_argc(argc)
    , m_argv(argv)
    , m_db(std::move(db))
#ifndef TINYTOM_TESTS_CODE
    , m_qtApplication(argc, argv)
#endif
    , m_environmentEnvName(environmentEnvName)
    , m_migrationTable(std::move(migrationTable))
    , m_migrationsPath(initializePath(TINYORM_STRINGIFY(TINYTOM_MIGRATIONS_DIR)))
    , m_modelsPath(initializePath(TINYORM_STRINGIFY(TINYTOM_MODELS_DIR)))
    , m_seedersPath(initializePath(TINYORM_STRINGIFY(TINYTOM_SEEDERS_DIR)))
    , m_migrations(std::move(migrations))
    , m_seeders(std::move(seeders))
{
    // Enable UTF-8 encoding and vt100 support
    Terminal::initialize();

    // Following is not relevant in the auto test executables
#ifndef TINYTOM_TESTS_CODE
    QCoreApplication::setOrganizationName(QStringLiteral("TinyORM"));
    QCoreApplication::setOrganizationDomain(QStringLiteral("tinyorm.org"));
    QCoreApplication::setApplicationName(QStringLiteral("tom"));
    QCoreApplication::setApplicationVersion(TINYTOM_VERSION_STR);
#endif

    // Print a newline at application's normal exit
//    initializeAtExit();

    // Fix m_argc/m_argv data members if the argv is empty
    fixEmptyArgv();

    // Initialize the command-line parser
    initializeParser(m_parser);
}

int Application::run()
{
    // Throw if no database connection configuration is registered
    throwIfNoConnectionConfig();
    // Default database connection is required if exactly one connection is registered
    throwIfEmptyDefaultConnection();

    // Process the actual command-line arguments given by the user
    parseCommandLine();

    // Ownership of a unique_ptr()
    return createCommand(getCommandName())->run();
}

void Application::logException(const std::exception &e, const bool noAnsi)
{
    // TODO future decide how qCritical()/qFatal() really works, also regarding to the Qt Creator's settings 'Ignore first chance access violations' and similar silverqx
    // TODO future alse how to correctly setup this in prod/dev envs. silverqx

    /* Want to have this method static, downside is that the InteractsWithIO has to be
       instantiated again. */
    const Concerns::InteractsWithIO io(noAnsi);

    static const auto tmpl = QStringLiteral("%1%2%1").arg(NEWLINE, TMPL_ONE);

    const auto message = QStringLiteral("Caught '%1' Exception:\n%2")
                         .arg(TypeUtils::classPureBasename(e, true), e.what());

    // No-ansi output
    if (noAnsi || !io.isAnsiOutput(std::cerr)) {
        qCritical().nospace().noquote() << tmpl.arg(message);
        return;
    }

    /* Print error wall (red box with a white text) */
    qCritical().nospace().noquote() << tmpl.arg(io.errorWallInternal(message));
}

QStringList Application::arguments() const
{
    // Never obtain arguments from the QCoreApplication instance in unit tests
    return hasQtApplication ? QCoreApplication::arguments()
                            : prepareArguments();
}

Application &Application::migrationsPath(fspath path)
{
    m_migrationsPath = initializePath(std::move(path));

    return *this;
}

Application &Application::seedersPath(fspath path)
{
    m_seedersPath = initializePath(std::move(path));

    return *this;
}

#ifdef TINYTOM_TESTS_CODE
std::vector<Application::StatusRow> Application::status()
{
    return StatusCommand::status();
}

void Application::enableInUnitTests() noexcept
{
    StatusCommand::setInUnitTests();
}
#endif

/* protected */

void Application::fixEmptyArgv()
{
    constexpr static const auto *const empty = "";

    if (m_argc == 0 || m_argv == nullptr) {
        m_argc = 0;
        m_argv = const_cast<char **>(&empty);
    }
}

// CUR tom, remove? silverqx
//void Application::initializeAtExit()
//{
//    std::atexit([]
//    {
//        std::cout << std::endl;
//    });
//}

void Application::initializeParser(QCommandLineParser &parser)
{
    parser.setApplicationDescription(
                QStringLiteral("TinyORM %1").arg(TINYORM_VERSION_STR));

    // Common options used in all commands
    parser.addOptions(saveOptions({
        {      ansi,           QStringLiteral("Force ANSI output")},
        {      noansi,         QStringLiteral("Disable ANSI output")},
        {      env,            QStringLiteral("The environment the command should run "
                                              "under"), env_up}, // Value
        {{QChar('h'),
          help},               QStringLiteral("Display help for the given command. When "
                                              "no command is given display help for the "
                                              "<info>list</info> command")},
        {{QChar('n'),
          nointeraction},      QStringLiteral("Do not ask any interactive question")},
        {{QChar('q'),
          quiet},              QStringLiteral("Do not output any message")},
        {{QChar('V'),
          Version},            QStringLiteral("Display this application version")},
        {{QChar('v'),
          verbose},            QStringLiteral("Increase the verbosity of messages: "
                                              "1 for normal output, "
                                              "2 for more verbose output and "
                                              "3 for debug")},
    }));
}

QList<QCommandLineOption>
Application::saveOptions(QList<CommandLineOption> &&options)
{
    m_options = std::move(options);

    // Result is passed to the QCommandLineParser::addOptions() so conversion is needed
    return TomUtils::convertToQCommandLineOptionList(m_options);
}

void Application::prependOptions(QList<CommandLineOption> &&options)
{
    // It contains only common options by default (initialized in the initializeParser())
    auto commonOptions = m_options;

    m_options.reserve(options.size() + commonOptions.size());

    // Command options first
    m_options = std::move(options);

    // Common options after Command options
    m_options << std::move(commonOptions);
}

/* Run command */

void Application::parseCommandLine()
{
    /* Can not use QCommandLineParser::ParseAsPositionalArguments because I still need
       to check eg. --version and --no-interaction below, it doesn't matter as everything
       works just fine. */

    /* Arguments have to be prepared manually in unit tests because is not possible
       to use the QCoreApplication::arguments() method. Also no error handling needed
       here as it is only some kind of pre-parse, whole command is parsed inside
       the command itself. */
    m_parser.parse(arguments());

    initializeEnvironment();

    /* Command-line arguments are parsed now, so the InteractsWithIO() base class can be
       initialized. Nothing bad to divide it into two steps as output to the console
       is not needed until here. */
    Concerns::InteractsWithIO::initialize(m_parser);

    if (m_parser.isSet(nointeraction))
        m_interactive = false;

    if (m_parser.isSet(Version))
        showVersion();
}

void Application::initializeEnvironment()
{
    /*! Order is as follow, the default value is development, can be overridden by
        a env. variable which name is in the m_environmentEnvName data member, highest
        priority has --env command-line argument. */
    if (auto environmentOpt = m_parser.value(env);
        !environmentOpt.isEmpty()
    )
        m_environment = std::move(environmentOpt);

    else if (auto environmentEnv = QString::fromUtf8(m_environmentEnvName).isEmpty()
                                   ? EMPTY
                                   : qEnvironmentVariable(m_environmentEnvName);
             !environmentEnv.isEmpty()
    )
        m_environment = std::move(environmentEnv);
}

QString Application::getCommandName()
{
    const auto arguments = m_parser.positionalArguments();

    if (arguments.isEmpty())
        return {};

    return getCommandName(arguments.constFirst(), ShowCommandsList);
}

QString Application::getCommandName(const QString &name, CommandNotFound notFound)
{
    // Try to find a full command name to avoid the guess logic
    if (auto name_ = name.toLower();
        ranges::contains(commandNames(), name_)
    )
        return name_;

    // Invoke the guess logic 👌
    auto commandName = guessCommandName(name);

    // If passed a non-existent command then show all commands list or error wall
    if (commandName.isEmpty())
        switch (notFound) {
        T_LIKELY
        case ShowCommandsList:
            showCommandsList(EXIT_FAILURE);

        T_UNLIKELY
        case ShowErrorWall: {
            errorWall(QStringLiteral("Command '%1' is not defined.").arg(name));

            exitApplication(EXIT_FAILURE);
        }
        default:
            Q_UNREACHABLE();
        }

    return commandName;
}

/* Early exit during parse command-line */

Q_NORETURN void Application::showVersion() const
{
    printVersion();

    exitApplication(EXIT_SUCCESS);
}

/* Has to be divided into two methods because the printVersion() is called from
   the list command. */
void Application::printVersion() const
{
    note(QStringLiteral("TinyORM "), false);

    info(TINYORM_VERSION_STR);
}

Q_NORETURN void Application::showCommandsList(const int exitCode)
{
    // FUTURE tom, allocate on heap to make func const, I would have to pass the const Application & to the Command base class and instead of to use one Application::m_parser I would have to create own parser for Command, problem is with the saveOptions(), prependOptions(), and initializeParser(), they are used in commands and they modify Application instance silverqx
    ListCommand(*this, m_parser).run();

    exitApplication(exitCode);
}

Q_NORETURN void Application::exitApplication(const int exitCode)
{
    /* Invoke post routines manually, it's safe as they will not be called twice even if
       the QCoreApplication's desctructor calls also this function. */
    qt_call_post_routines();

    ::exit(exitCode); // NOLINT(concurrency-mt-unsafe)
}

/* Commands factory */

std::unique_ptr<Command>
Application::createCommand(const QString &command, const OptionalParserRef parser,
                           const bool showHelp)
{
    // Use a custom parser if passed as the argument, needed by CallsCommands::call()
    auto parserRef = parser ? *parser : std::ref(m_parser);

    if (command == Complete)
        return std::make_unique<CompleteCommand>(*this, parserRef);

    if (command == DbSeed)
        return std::make_unique<SeedCommand>(*this, parserRef);

    if (command == DbWipe)
        return std::make_unique<WipeCommand>(*this, parserRef);

    if (command == Env)
        return std::make_unique<EnvironmentCommand>(*this, parserRef);

    if (command == Help)
        return std::make_unique<HelpCommand>(*this, parserRef);

    if (command == Inspire)
        return std::make_unique<InspireCommand>(*this, parserRef);

    if (command == List)
        return std::make_unique<ListCommand>(*this, parserRef);

    if (command == MakeMigration)
        return std::make_unique<MigrationCommand>(*this, parserRef);

    if (command == MakeModel)
        return std::make_unique<ModelCommand>(*this, parserRef);

//    if (command == MakeProject)
//        return std::make_unique<ProjectCommand>(*this, parserRef);

    if (command == MakeSeeder)
        return std::make_unique<SeederCommand>(*this, parserRef);

    if (command == Migrate)
        return std::make_unique<MigrateCommand>(*this, parserRef, createMigrator());

    if (command == MigrateFresh)
        return std::make_unique<FreshCommand>(*this, parserRef, createMigrator());

    if (command == MigrateInstall)
        return std::make_unique<InstallCommand>(*this, parserRef,
                                                createMigrationRepository());

    if (command == MigrateRollback)
        return std::make_unique<RollbackCommand>(*this, parserRef, createMigrator());

    if (command == MigrateRefresh)
        return std::make_unique<RefreshCommand>(*this, parserRef, createMigrator());

    if (command == MigrateReset)
        return std::make_unique<ResetCommand>(*this, parserRef, createMigrator());

    if (command == MigrateStatus)
        return std::make_unique<StatusCommand>(*this, parserRef, createMigrator());

    if (command == MigrateUninstall)
        return std::make_unique<UninstallCommand>(*this, parserRef,
                                                  createMigrationRepository());

    if (command == Integrate)
        return std::make_unique<IntegrateCommand>(*this, parserRef);

    // Used by the help command
    if (!showHelp)
        return nullptr;

    // If passed non-existent command then show all commands list
    this->showCommandsList(EXIT_FAILURE);
}

std::shared_ptr<MigrationRepository> Application::createMigrationRepository() const
{
    if (m_repository)
        return m_repository;

    return m_repository =
            std::make_shared<MigrationRepository>(
                std::dynamic_pointer_cast<ConnectionResolverInterface>(m_db),
                m_migrationTable);
}

std::shared_ptr<Migrator> Application::createMigrator() const
{
    if (m_migrator)
        return m_migrator;

    return m_migrator =
            std::make_shared<Migrator>(
                createMigrationRepository(),
                std::dynamic_pointer_cast<ConnectionResolverInterface>(m_db),
                m_migrations, m_migrationsProperties, m_parser);
}

/* Others */

QStringList Application::prepareArguments() const
{
    QStringList arguments;
    arguments.reserve(m_argc);

    for (QStringList::size_type i = 0; i < m_argc; ++i)
        arguments << QString::fromUtf8(m_argv[i]);

    return arguments;
}

const std::vector<std::shared_ptr<Application::Command>> &
Application::createCommandsVector()
{
    static const std::vector<std::shared_ptr<Command>> cached = [this]
    {
        return commandNames()
                | ranges::views::transform([this](const QString &commandName)
                                           -> std::shared_ptr<Command>
        {
            return createCommand(commandName);
        })
                | ranges::to<std::vector<std::shared_ptr<Command>>>();
    }();

    return cached;
}

const std::unordered_map<QString, std::tuple<int, int>> &
Application::commandsByNamespaceHash()
{
    /* First number is index where it starts (0-based), second the number where it ends
       (it's like iterator's end so should point after).
       Look to the Application::commandNames() to understand this indexes.
       tuple is forwarded as args to the ranges::views::slice(). */
    static const std::unordered_map<QString, std::tuple<int, int>> cached = []
    {
        Q_ASSERT(namespaceNames().size() == commandsIndexes().size());

        return ranges::views::zip_with(
                    [](const QString &namespaceName, const auto &indexes)
                    -> std::pair<QString, std::tuple<int, int>>
        {
            return {namespaceName, indexes};
        },
            namespaceNames(), commandsIndexes()
        )
            | ranges::to<std::unordered_map<QString, std::tuple<int, int>>>();
    }();

    return cached;
}

const std::vector<std::reference_wrapper<const QString>> &
Application::commandNames()
{
    // Order is important here (shown by defined order by the list command)
    static const std::vector<std::reference_wrapper<const QString>> cached {
        // global namespace
        Complete, Env, Help, Inspire, Integrate, List, Migrate,
        // db
        DbSeed, DbWipe,
        // make
        MakeMigration, MakeModel, /*MakeProject,*/ MakeSeeder,
        // migrate
        MigrateFresh,  MigrateInstall,  MigrateRefresh, MigrateReset, MigrateRollback,
        MigrateStatus, MigrateUninstall
    };

    return cached;
}

const std::vector<std::reference_wrapper<const QString>> &
Application::namespaceNames()
{
    // Order is important here - ziped with the commandsIndexes()
    static const std::vector<std::reference_wrapper<const QString>> cached {
        // global namespace
        EMPTY, NsGlobal,
        // all other namespaces
        NsDb, NsMake, NsMigrate,
        /* The special index used by the command name guesser, it doesn't name
           the namespace but rather returns all namespaced commands. I leave it
           accessible also by the list command so a user can also display all namespaced
           commands. */
        NsNamespaced,
        /* The special index used by the command name guesser for the complete command,
           it doesn't name the namespace but rather returns all commands. I leave it
           accessible also by the list command so a user can also display all namespaced
           commands. */
        NsAll,
    };

    return cached;
}

const std::vector<std::tuple<int, int>> &Application::commandsIndexes()
{
    /* First number is index where it starts (0-based), second the number where it ends
       (it's like iterator's end so should point after).
       Look to the Application::commandNames() to understand this indexes.
       tuple is forwarded as args to the ranges::views::slice().

       Order is important here - ziped with the namespaceNames(). */
    static const std::vector<std::tuple<int, int>> cached {
        {0,   7}, // "" - also global
        {0,   7}, // global
        {7,   9}, // db
        {9,  12}, // make
        {12, 19}, // migrate
        {7,  19}, // namespaced
        {0,  19}, // all
    };

    return cached;
}

QList<CommandLineOption> Application::getCommandOptionsSignature(const QString &command)
{
    // Ownership of a unique_ptr()
    return createCommand(command, std::nullopt, false)->optionsSignature();
}

fspath Application::initializePath(fspath &&path)
{
    path = path.lexically_normal();

    // Remove a trailing separator
    if (!path.empty() && path.filename().empty())
        path = path.parent_path();

    return std::move(path);
}

std::shared_ptr<ConnectionResolverInterface>
Application::connectionResolver() const noexcept
{
    return std::dynamic_pointer_cast<ConnectionResolverInterface>(m_db);
}

void Application::throwIfNoConnectionConfig() const
{
    // Nothing to do, some database connection configuration/s are already registered
    if (m_db->originalConfigsSize() > 0)
        return;

    throw Exceptions::RuntimeError(
                "No connection configuration is registered, please add at least one "
                "database configuration to the DatabaseManager (eg. using "
                "DB::create()).");
}

void Application::throwIfEmptyDefaultConnection() const
{
    // Nothing to do, default connection is defined or more connections is registered
    if (!m_db->getDefaultConnection().isEmpty() || m_db->originalConfigsSize() != 1)
        return;

    // Throw if exactly one database connection is registered
    throw Exceptions::RuntimeError("Default database connection is not configured.");
}

/* Auto tests helpers */

#ifdef TINYTOM_TESTS_CODE
/* public */

int Application::runWithArguments(QStringList &&arguments)
{
    // Process the actual command-line arguments given by the user
    parseCommandLine();

    // Ownership of a unique_ptr()
    return createCommand(getCommandName())->runWithArguments(std::move(arguments));
}
#endif

} // namespace Tom

TINYORM_END_COMMON_NAMESPACE

// CUR tom, commands I want to implement; test, schema:dump, model:show silverqx
