#include "tom/application.hpp"

#include <QCoreApplication>
#include <QDebug>

#ifdef _WIN32
#  include <qt_windows.h>

#  include <fcntl.h>
#  include <io.h>
#endif

#include <orm/databasemanager.hpp>
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
#ifndef TINYTOM_TESTS_CODE
#  include "tom/version.hpp"
#endif

using Orm::ConnectionResolverInterface;
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

/*! Invoke Qt's global post routines. */
extern void Q_DECL_IMPORT qt_call_post_routines();

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Tom {

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
    , m_migrations(std::move(migrations))
{
#ifdef _WIN32
    // Prepare console input/output character encoding
    initializeConsoleEncoding();
#endif

    // Following is not relevant in the auto test executables
#ifndef TINYTOM_TESTS_CODE
    QCoreApplication::setOrganizationName("TinyORM");
    QCoreApplication::setOrganizationDomain("tinyorm.org");
    QCoreApplication::setApplicationName("tom");
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
    auto message = QStringLiteral("Caught '%1' Exception:\n%2")
                   .arg(TypeUtils::classPureBasename(e, true), e.what());

    /* Want to have this method static, downside is that the InteractsWithIO has to be
       instantiated again. */
    Concerns::InteractsWithIO io(noAnsi);

    static const auto tmpl = QStringLiteral("%1%2%1").arg(NEWLINE, "%1");

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

#ifdef TINYTOM_TESTS_CODE
std::vector<Application::StatusRow> Application::status() noexcept
{
    return StatusCommand::status();
}

void Application::enableInUnitTests() noexcept
{
    StatusCommand::setInUnitTests();
}
#endif

/* protected */

/* Application initialization */

#ifdef _WIN32
void Application::initializeConsoleEncoding() const
{
    // Set it here so the user doesn't have to deal with this
    SetConsoleOutputCP(CP_UTF8);

    /* UTF-8 encoding is corrupted for narrow input functions, needed to use wcin/wstring
       for an input, input will be in the unicode encoding then needed to translate
       unicode to utf8, eg. by QString::fromStdWString(), WideCharToMultiByte(), or
       std::codecvt(). It also works with msys2 ucrt64 gcc/clang. */
    SetConsoleCP(CP_UTF8);
    _setmode(_fileno(stdin), _O_WTEXT);
}
#endif

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
        {      "ansi",            "Force ANSI output"},
        {      "no-ansi",         "Disable ANSI output"},
        {      "env",             "The environment the command should run under",
                                  "env"}, // Value
        {{"h",  "help"},          "Display help for the given command. When no command "
                                   "is given display help for the <info>list</info> "
                                   "command"},
        {{"n", "no-interaction"}, "Do not ask any interactive question"},
        {{"q", "quiet"},          "Do not output any message"},
        {{"V", "version"},        "Display this application version"},
        {{"v", "verbose"},        "Increase the verbosity of messages: 1 for normal "
                                   "output, 2 for more verbose output and 3 for debug"},
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

    if (m_parser.isSet("no-interaction"))
        m_interactive = false;

    if (m_parser.isSet(QStringLiteral("version")))
        showVersion();
}

void Application::initializeEnvironment()
{
    /*! Order is as follow, the default value is development, can be overriden by
        a env. variable which name is in the m_environmentEnvName data member, highest
        priority has --env command-line argument. */
    if (auto environmentCmd = m_parser.value("env");
        !environmentCmd.isEmpty()
    )
        m_environment = std::move(environmentCmd);

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

    return arguments.at(0);
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

    if (command == QStringLiteral("db:wipe"))
        return std::make_unique<WipeCommand>(*this, parserRef);

    if (command == QStringLiteral("env"))
        return std::make_unique<EnvironmentCommand>(*this, parserRef);

    if (command == QStringLiteral("help"))
        return std::make_unique<HelpCommand>(*this, parserRef);

    if (command == QStringLiteral("inspire"))
        return std::make_unique<InspireCommand>(*this, parserRef);

    if (command == QStringLiteral("list"))
        return std::make_unique<ListCommand>(*this, parserRef);

    if (command == QStringLiteral("make:migration"))
        return std::make_unique<MigrationCommand>(*this, parserRef);

//    if (command == QStringLiteral("make:project"))
//        return std::make_unique<ProjectCommand>(*this, parserRef);

    if (command == QStringLiteral("migrate"))
        return std::make_unique<MigrateCommand>(*this, parserRef, createMigrator());

    if (command == QStringLiteral("migrate:fresh"))
        return std::make_unique<FreshCommand>(*this, parserRef, createMigrator());

    if (command == QStringLiteral("migrate:install"))
        return std::make_unique<InstallCommand>(*this, parserRef,
                                                createMigrationRepository());

    if (command == QStringLiteral("migrate:rollback"))
        return std::make_unique<RollbackCommand>(*this, parserRef, createMigrator());

    if (command == QStringLiteral("migrate:refresh"))
        return std::make_unique<RefreshCommand>(*this, parserRef, createMigrator());

    if (command == QStringLiteral("migrate:reset"))
        return std::make_unique<ResetCommand>(*this, parserRef, createMigrator());

    if (command == QStringLiteral("migrate:status"))
        return std::make_unique<StatusCommand>(*this, parserRef, createMigrator());

    // Used by the help command
    if (!showHelp)
        return nullptr;

    // If passed non-existent command then show all commands list
    this->showCommandsList(EXIT_FAILURE);
}

std::shared_ptr<MigrationRepository> Application::createMigrationRepository()
{
    if (m_repository)
        return m_repository;

    return m_repository =
            std::make_shared<MigrationRepository>(
                std::dynamic_pointer_cast<ConnectionResolverInterface>(m_db),
                m_migrationTable);
}

std::shared_ptr<Migrator> Application::createMigrator()
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

const std::vector<std::shared_ptr<Application::Command>> &
Application::createCommandsVector()
{
    static const std::vector<std::shared_ptr<Command>> cached = [this]
    {
        return commandNames()
                | ranges::views::transform([this](const char *const commandName)
                                           -> std::shared_ptr<Command>
        {
            return createCommand(commandName);
        })
                | ranges::to<std::vector<std::shared_ptr<Command>>>();
    }();

    return cached;
}

const std::vector<const char *> &Application::commandNames() const
{
    // Order is important here
    static const std::vector<const char *> cached {
        // global namespace
        "env", "help", "inspire", "list", "migrate",
        // db
        "db:wipe",
        // make
        "make:migration", "make:project",
        // migrate
        "migrate:fresh",    "migrate:install", "migrate:refresh", "migrate:reset",
        "migrate:rollback", "migrate:status",
    };

    return cached;
}

QStringList Application::prepareArguments() const
{
    QStringList arguments;
    arguments.reserve(m_argc);

    for (QStringList::size_type i = 0; i < m_argc; ++i)
        arguments << QString::fromUtf8(m_argv[i]);

    return arguments;
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
// CUR tom, implement support for short command names, eg mig:st silverqx
