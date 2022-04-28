#include "tom/application.hpp"

#include <QCoreApplication>
#include <QDebug>

#include <range/v3/algorithm/contains.hpp>
#include <range/v3/view/zip_with.hpp>

#include <orm/databasemanager.hpp>
#include <orm/macros/likely.hpp>
#include <orm/utils/type.hpp>
#include <orm/version.hpp>

#include "tom/commands/database/wipecommand.hpp"
#include "tom/commands/environmentcommand.hpp"
#include "tom/commands/helpcommand.hpp"
#include "tom/commands/inspirecommand.hpp"
#include "tom/commands/listcommand.hpp"
#include "tom/commands/make/migrationcommand.hpp"
//#include "tom/commands/make/projectcommand.hpp"
#include "tom/commands/migrations/freshcommand.hpp"
#include "tom/commands/migrations/installcommand.hpp"
#include "tom/commands/migrations/migratecommand.hpp"
#include "tom/commands/migrations/refreshcommand.hpp"
#include "tom/commands/migrations/resetcommand.hpp"
#include "tom/commands/migrations/rollbackcommand.hpp"
#include "tom/commands/migrations/statuscommand.hpp"
#include "tom/migrationrepository.hpp"
#include "tom/migrator.hpp"
#include "tom/terminal.hpp"
#ifndef TINYTOM_TESTS_CODE
#  include "tom/version.hpp"
#endif

using fspath = std::filesystem::path;

using Orm::ConnectionResolverInterface;
using Orm::Constants::EMPTY;
using Orm::Constants::NEWLINE;

using TypeUtils = Orm::Utils::Type;

using Tom::Commands::Command;
using Tom::Commands::Database::WipeCommand;
using Tom::Commands::EnvironmentCommand;
using Tom::Commands::HelpCommand;
using Tom::Commands::InspireCommand;
using Tom::Commands::ListCommand;
using Tom::Commands::Make::MigrationCommand;
//using Tom::Commands::Make::ProjectCommand;
using Tom::Commands::Migrations::FreshCommand;
using Tom::Commands::Migrations::InstallCommand;
using Tom::Commands::Migrations::MigrateCommand;
using Tom::Commands::Migrations::RefreshCommand;
using Tom::Commands::Migrations::ResetCommand;
using Tom::Commands::Migrations::RollbackCommand;
using Tom::Commands::Migrations::StatusCommand;

using namespace Tom::Constants;

/*! Invoke Qt's global post routines. */
extern void Q_DECL_IMPORT qt_call_post_routines();

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom {

/* Adding/removing/disabling/enabling a command, add #include, using, factory in the
   Application::createCommand(), add a command name to the Application::commandNames(),
   update indexes in the Application::commandsIndexes() and if the command introduces
   a new namespace add it to the Application::namespaceNames().
   I have everything extracted and placed it to the bottom of application.cpp so it is
   nicely on one place. */

/* public */

Application::Application(int &argc, char **argv, std::shared_ptr<DatabaseManager> db,
                         const char *const environmentEnvName, QString migrationTable,
                         std::vector<std::shared_ptr<Migration>> migrations)
    : m_argc(argc)
    , m_argv(argv)
    , m_db(std::move(db))
#ifndef TINYTOM_TESTS_CODE
    , m_qtApplication(argc, argv)
#endif
    , m_environmentEnvName(environmentEnvName)
    , m_migrationTable(std::move(migrationTable))
    , m_migrationsPath(initializeMigrationsPath(
                           TINYORM_STRINGIFY(TINYTOM_MIGRATIONS_PATH)))
    , m_migrations(std::move(migrations))
{
    // Enable UTF-8 encoding and vt100 support
    Terminal::initialize();

    // Following is not relevant in the auto test executables
#ifndef TINYTOM_TESTS_CODE
    QCoreApplication::setOrganizationName(QLatin1String("TinyORM"));
    QCoreApplication::setOrganizationDomain(QLatin1String("tinyorm.org"));
    QCoreApplication::setApplicationName(QLatin1String("tom"));
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
    // Process the actual command-line arguments given by the user
    parseCommandLine();

    // Ownership of a unique_ptr()
    return createCommand(getCommandName())->run();
}

void Application::logException(const std::exception &e, const bool noAnsi)
{
    // TODO future decide how qCritical()/qFatal() really works, also regarding to the Qt Creator's settings 'Ignore first chance access violations' and similar silverqx
    // TODO future alse how to correctly setup this in prod/dev envs. silverqx
    auto message = QLatin1String("Caught '%1' Exception:\n%2")
                   .arg(TypeUtils::classPureBasename(e, true), e.what());

    /* Want to have this method static, downside is that the InteractsWithIO has to be
       instantiated again. */
    Concerns::InteractsWithIO io(noAnsi);

    static const auto tmpl = QLatin1String("%1%2%1").arg(NEWLINE, "%1");

    // No-ansi output
    if (noAnsi || !io.isAnsiOutput(std::cerr)) {
        qCritical().nospace().noquote() << tmpl.arg(std::move(message));
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
    m_migrationsPath = initializeMigrationsPath(std::move(path));

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
    constexpr const auto *const empty = "";

    if (m_argc == 0 || m_argv == nullptr) {
        m_argc = 0;
        m_argv = const_cast<char **>(&empty);
    }
}

// CUR tom, remove? silverqx
void Application::initializeAtExit() const
{
    std::atexit([]
    {
        std::cout << std::endl;
    });
}

void Application::initializeParser(QCommandLineParser &parser)
{
    parser.setApplicationDescription(
                QStringLiteral("TinyORM %1").arg(TINYORM_VERSION_STR));

    // Common options used in all commands
    parser.addOptions(saveOptions({
        {      ansi,           QLatin1String("Force ANSI output")},
        {      noansi,         QLatin1String("Disable ANSI output")},
        {      env,            QLatin1String("The environment the command should run "
                                             "under"), env}, // Value
        {{"h", help},          QLatin1String("Display help for the given command. When "
                                             "no command is given display help for the "
                                             "<info>list</info> command")},
        {{"n", nointeraction}, QLatin1String("Do not ask any interactive question")},
        {{"q", quiet},         QLatin1String("Do not output any message")},
        {{"V", version},       QLatin1String("Display this application version")},
        {{"v", verbose},       QLatin1String("Increase the verbosity of messages: "
                                             "1 for normal output, "
                                             "2 for more verbose output and "
                                             "3 for debug")},
    }));
}

const QList<QCommandLineOption> &
Application::saveOptions(QList<QCommandLineOption> &&options)
{
    return m_options = std::move(options);
}

QList<QCommandLineOption>
Application::prependOptions(QList<QCommandLineOption> &&options)
{
    auto commonOptions = m_options;

    m_options = options;

    m_options << std::move(commonOptions);

    // Return only a new options because they are passed to the addOptions() method
    return std::move(options);
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

    /* Command line arguments are parsed now, the InteractsWithIO() base class can be
       initialized. Nothing bad to divide it into two steps as output to the console
       is not needed until here. */
    Concerns::InteractsWithIO::initialize(m_parser);

    if (m_parser.isSet(nointeraction))
        m_interactive = false;

    if (m_parser.isSet(version))
        showVersion();
}

void Application::initializeEnvironment()
{
    /*! Order is as follow, the default value is development, can be overriden by
        a env. variable which name is in the m_environmentEnvName data member, highest
        priority has --env command-line argument. */
    if (auto environmentOpt = m_parser.value(env);
        !environmentOpt.isEmpty()
    )
        m_environment = std::move(environmentOpt);

    else if (auto environmentEnv = QString::fromUtf8(m_environmentEnvName).isEmpty()
                                   ? ""
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

    return getCommandName(arguments[0], ShowCommandsList);
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
            errorWall(QLatin1String("Command '%1' is not defined.").arg(name));

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

Q_NORETURN void Application::exitApplication(const int exitCode) const
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

//    if (command == MakeProject)
//        return std::make_unique<ProjectCommand>(*this, parserRef);

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
                m_migrations, m_parser);
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
Application::commandsByNamespaceHash() const
{
    /* First number is index where it starts (0-based), second the number where it ends
       (it's like iterator's end so should point after).
       Look to the Application::commandNames() to understand this indexes.
       tuple is forwarded as args to the ranges::views::slice(). */
    static const std::unordered_map<QString, std::tuple<int, int>> cached = [this]
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
Application::commandNames() const
{
    // Order is important here (shown by defined order by the list command)
    static const std::vector<std::reference_wrapper<const QString>> cached {
        // global namespace
        Env, Help, Inspire, List, Migrate,
        // db
        DbWipe,
        // make
        MakeMigration, /*MakeProject,*/
        // migrate
        MigrateFresh, MigrateInstall, MigrateRefresh, MigrateReset, MigrateRollback,
        MigrateStatus,
    };

    return cached;
}

const std::vector<std::reference_wrapper<const QString>> &
Application::namespaceNames() const
{
    // Order is important here - ziped with the commandsIndexes()
    static const std::vector<std::reference_wrapper<const QString>> cached {
        // global namespace
        EMPTY, NsGlobal,
        // all other namespaces
        NsDb, NsMake, NsMigrate,
        /* The special index used by the command name guesser doesn't name the namespace
           but rather returns all namespaced commands. I leave it accessible also by
           the list command so a user can also display all namespaced commands. */
        NsNamespaced,
    };

    return cached;
}

const std::vector<std::tuple<int, int>> &Application::commandsIndexes() const
{
    /* First number is index where it starts (0-based), second the number where it ends
       (it's like iterator's end so should point after).
       Look to the Application::commandNames() to understand this indexes.
       tuple is forwarded as args to the ranges::views::slice().

       Order is important here - ziped with the namespaceNames(). */
    static const std::vector<std::tuple<int, int>> cached {
        {0, 5},  // "" - also global
        {0, 5},  // global
        {5, 6},  // db
        {6, 7},  // make
        {7, 13}, // migrate
        {5, 13}, // namespaced
    };

    return cached;
}

fspath Application::initializeMigrationsPath(fspath &&path)
{
    if (path.is_relative())
        path = std::filesystem::current_path() / std::move(path);

    return path.make_preferred();
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

// CUR tom, commands I want to implement; completion, test, db:seed, schema:dump silverqx
