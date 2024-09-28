#include "tom/application.hpp"

#include <QCoreApplication>
#include <QDebug>
#include <QLibraryInfo>

#include <range/v3/algorithm/contains.hpp>
#include <range/v3/version.hpp>
#include <range/v3/view/zip_with.hpp>

/* This header exists from the tabulate v1.4.0, it has been added in the middle of v1.3.0
   and didn't exist at the v1.3.0 release. */
#if __has_include(<tabulate/tabulate.hpp>)
#  include <tabulate/tabulate.hpp>
#endif

#ifdef TINYORM_USING_TINYDRIVERS
#  include <orm/drivers/libraryinfo.hpp>
#  include <orm/drivers/version.hpp>
#endif
#ifdef TINYDRIVERS_MYSQL_DRIVER
#  include <orm/drivers/mysql/version.hpp>
#endif

#include <orm/databasemanager.hpp>
#include <orm/libraryinfo.hpp>
#include <orm/macros/compilerdetect.hpp>
#include <orm/macros/likely.hpp>
#include <orm/utils/type.hpp>
#include <orm/version.hpp>

#include "tom/commands/aboutcommand.hpp"
#if defined(__linux__) || defined(__MINGW32__)
#  include "tom/commands/complete/bashcommand.hpp"
#endif
#include "tom/commands/complete/pwshcommand.hpp"
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
#include "tom/concerns/interactswithio.hpp"
#include "tom/exceptions/runtimeerror.hpp"
#include "tom/migrationrepository.hpp"
#include "tom/migrator.hpp"
#include "tom/terminal.hpp"
#include "tom/tomutils.hpp"
#include "tom/version.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using fspath = std::filesystem::path;

#ifdef TINYORM_USING_TINYDRIVERS
using DriversLibraryInfo = Orm::Drivers::LibraryInfo;
#endif

using Orm::Constants::COMMA;
#ifdef TINYDRIVERS_MYSQL_DRIVER
using Orm::Constants::MYSQL_;
#endif
using Orm::Constants::NEWLINE_C;
using Orm::Constants::NOSPACE;
using Orm::Constants::PARENTH_ONE;
using Orm::Constants::SPACE;
using Orm::Constants::TMPL_ONE;
using Orm::Constants::Version;

using Orm::ConnectionResolverInterface;
using Orm::LibraryInfo;

using TypeUtils = Orm::Utils::Type;

using Tom::Commands::AboutCommand;
using Tom::Commands::Command;
#if defined(__linux__) || defined(__MINGW32__)
using Tom::Commands::Complete::BashCommand;
#endif
using Tom::Commands::Complete::PwshCommand;
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

using Tom::Constants::About;
#if defined(__linux__) || defined(__MINGW32__)
using Tom::Constants::CompleteBash;
#endif
using Tom::Constants::CompletePwsh;
using Tom::Constants::DbSeed;
using Tom::Constants::DbWipe;
using Tom::Constants::EMPTY;
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
using Tom::Constants::NsComplete;
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

/* Adding/removing/disabling/enabling a command, add constants, #include, using, factory
   in Application::createCommand(), add a command name to the Application::commandNames(),
   update indexes in the Application::commandsIndexes() and if the command introduces
   a new namespace add it to the Application::namespaceNames(). If the command or
   namespace is hidden, add it to the isCommandHidden() or isNamespaceHidden() methods.
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

/* Can't be inline static to avoid:
   error: C2492: 'Tom::Application::m_inUnitTests':
   data with thread storage duration may not have dll interface */
#ifdef TINYTOM_TESTS_CODE
namespace
{
    /*! Is enabled logic for unit testing? */
    T_THREAD_LOCAL
    auto g_inUnitTests = false; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
} // namespace
#endif

/* public */

/* Can't change it to const char *argv[] and const int argc because we are passing them
   down to the QCoreApplication() and it needs non-const values. */
Application::Application(int &argc, char *argv[], std::shared_ptr<DatabaseManager> db, // NOLINT(modernize-avoid-c-arrays)
                         const char *const environmentEnvName, QString migrationTable,
                         std::vector<std::shared_ptr<Migration>> migrations,
                         std::vector<std::shared_ptr<Seeder>> seeders)
    : m_argc(argc)
    , m_argv(const_cast<const char **>(argv),
             static_cast<decltype (m_argv)::size_type>(argc))
    , m_db(std::move(db))
    , m_qtApplication(createQCoreApplication(argc, argv))
    , m_environmentEnvName(environmentEnvName)
    , m_migrationTable(std::move(migrationTable))
    , m_migrationsPath(initializePath(TINY_STRINGIFY(TINYTOM_MIGRATIONS_DIR)))
    , m_modelsPath(initializePath(TINY_STRINGIFY(TINYTOM_MODELS_DIR)))
    , m_seedersPath(initializePath(TINY_STRINGIFY(TINYTOM_SEEDERS_DIR)))
    , m_migrations(std::move(migrations))
    , m_seeders(std::move(seeders))
    , m_io(nullptr) // Instantiated after the command-line is parsed
{
    // Enable UTF-8 encoding and VT100 support
    Terminal::initialize();

    QCoreApplication::setApplicationName(u"tom"_s);
    QCoreApplication::setOrganizationName(u"TinyORM"_s);
    QCoreApplication::setOrganizationDomain(u"tinyorm.org"_s);
    QCoreApplication::setApplicationVersion(TINYTOM_VERSION_STR);

    // Initialize the command-line parser
    initializeParser(m_parser);
}

// Needed by a unique_ptr()
Application::~Application() = default;

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
    // TODO future also how to correctly setup this in prod/dev envs. silverqx

    /* Want to have this method static, downside is that the InteractsWithIO has to be
       instantiated again. */
    const Concerns::InteractsWithIO io(noAnsi);

    static const auto tmpl = u"%1%2%1"_s.arg(NEWLINE_C, TMPL_ONE);

    const auto message = u"Caught '%1' Exception:\n%2"_s
                         .arg(TypeUtils::classPureBasename(e, true), e.what());

    // --no-ansi output
    if (noAnsi || !io.isAnsiOutput(std::cerr)) {
        qCritical().nospace().noquote() << tmpl.arg(message);
        return;
    }

    /* Print error wall (red box with a white text) */
    qCritical().nospace().noquote() << tmpl.arg(io.errorWallInternal(message));
}

QStringList Application::arguments() const // NOLINT(readability-convert-member-functions-to-static)
{
#ifdef TINYTOM_TESTS_CODE
    /* Never obtain arguments from the QCoreApplication instance in unit tests because
       they are passed using the runWithArguments() method. */
    return g_inUnitTests ? prepareArguments()
                         : QCoreApplication::arguments();
#else
    return QCoreApplication::arguments();
#endif
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
    g_inUnitTests = true;
}
#endif

/* protected */

void Application::initializeParser(QCommandLineParser &parser)
{
    parser.setApplicationDescription(
                u"TinyORM %1"_s.arg(TINYORM_VERSION_STR));

    // Common options used in all commands
    parser.addOptions(saveOptions({
        {      ansi,           u"Force ANSI output"_s},
        {      noansi,         u"Disable ANSI output"_s},
        {      env,            u"The environment the command should run under"_s, env_up}, // Value
        {{QChar('h'),
          help},               u"Display help for a given command, when no command is "
                                "given display help for the <info>list</info> command"_s},
        {{QChar('n'),
          nointeraction},      u"Do not ask any interactive question"_s},
        {{QChar('q'),
          quiet},              u"Do not output any message"_s},
        {{QChar('V'),
          Version},            u"Display this application version"_s},
        {{QChar('v'),
          verbose},            u"Increase verbosity of messages: "
                                "1 for normal output, "
                                "2 for more verbose output, "
                                "3 for debug"_s},
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

    /* Command-line arguments are parsed now, so the InteractsWithIO() class can be
       instantiated. There's nothing wrong with being so late as output to the console
       is not needed until now. */
    m_io = std::make_unique<Concerns::InteractsWithIO>(m_parser);

    if (m_parser.isSet(nointeraction))
        m_interactive = false;

    if (m_parser.isSet(Version))
        showVersion();
}

void Application::initializeEnvironment()
{
    /*! The default value is local, can be overridden by an environment variable which
        name is in the m_environmentEnvName data member, the --env command-line argument
        has the highest priority. */

    // A value from the --env command-line argument
    if (auto environmentOpt = m_parser.value(env);
        !environmentOpt.isEmpty()
    )
        m_environment = std::move(environmentOpt);

    // Resolve an environment variable set in the m_environmentEnvName data member
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

QString Application::getCommandName(const QString &name, const CommandNotFound notFound)
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
        handleEmptyCommandName(name, notFound); // [[noreturn]]

    return commandName;
}

void Application::handleEmptyCommandName(const QString &name,
                                         const CommandNotFound notFound)
{
    switch (notFound) {
    T_LIKELY
    case ShowCommandsList:
        showCommandsList(EXIT_FAILURE);

    T_UNLIKELY
    case ShowErrorWall:
        io().errorWall(u"Command '%1' is not defined."_s.arg(name));

        exitApplication(EXIT_FAILURE);

    default:
#ifndef TINYTOM_DEBUG
        throw Exceptions::RuntimeError(
                    u"Unexpected value for enum struct CommandNotFound."_s);
#else
        Q_UNREACHABLE();
#endif
    }
}

/* Early exit during parse command-line */

void Application::showVersion() const
{
    printFullVersions();

    exitApplication(EXIT_SUCCESS);
}

void Application::printVersion() const
{
    io().note(u"TinyORM "_s, false);

    io().info(TINYORM_VERSION_STR);
}

void Application::printFullVersions() const
{
    io().note(u"tom "_s, false);
    io().info(TINYTOM_VERSION_STR);

    for (const auto versionsSubsection = createVersionsSubsection();
         const auto &[subsectionName, abouts] : versionsSubsection
    ) {
        // Subsection name is optional
        if (subsectionName) {
            io().newLine();
            io().comment(*subsectionName);
        }

        /*! Alias for the std::map<QString, AboutValue>. */
        using AboutItemsType = std::map<QString, AboutValue>;
        // Must always hold the std::map with the AboutValue mapped_type
        Q_ASSERT(std::holds_alternative<AboutItemsType>(abouts));

        for (const auto &[name, about] : std::get<AboutItemsType>(abouts)) {
            io().note(NOSPACE.arg(name).arg(SPACE), false);
            io().info(about.value, false);

            // Item components
            if (const auto &components = about.components; components)
               io().muted(SPACE + PARENTH_ONE.arg(components->join(COMMA)), false);

            io().newLine();
        }
    }
}

namespace
{
#ifdef TINYORM_USING_TINYDRIVERS
    /*! Get TinyDrivers components for the tom --version and tom about commands. */
    inline QStringList getDriversComponents()
    {
        return {
#ifdef TINYDRIVERS_MYSQL_DRIVER
            MYSQL_,
#endif
        };
    }
#endif // TINYORM_USING_TINYDRIVERS

    /*! Get Qt components for the tom --version and tom about commands. */
    inline QStringList getQtComponents()
    {
        return {
            u"Core"_s,
#ifdef TINYORM_USING_QTSQLDRIVERS
            u"Sql"_s,
#endif
        };
    }
} // namespace

QList<SubSectionItem> Application::createVersionsSubsection()
{
    static const auto Debug_   = u"Debug"_s;
    static const auto Release_ = u"Release"_s;

    return {
        {u"Dependencies"_s,
            std::map<QString, AboutValue> {
                {u"TinyORM"_s,     TINYORM_VERSION_STR},
#ifdef TINYORM_USING_TINYDRIVERS
                {u"TinyDrivers"_s, {TINYDRIVERS_VERSION_STR, getDriversComponents()}},
#endif
#ifdef TINYDRIVERS_MYSQL_DRIVER
                {u"TinyMySql"_s,   TINYMYSQL_VERSION_STR},
#endif
                {u"Qt"_s,          {QT_VERSION_STR, getQtComponents()}},
                {u"range-v3"_s,    u"%1.%2.%3"_s.arg(RANGE_V3_MAJOR)
                                                .arg(RANGE_V3_MINOR)
                                                .arg(RANGE_V3_PATCHLEVEL)},
#if __has_include(<tabulate/tabulate.hpp>) && defined(TABULATE_VERSION_MAJOR) && \
    defined(TABULATE_VERSION_MINOR) && defined(TABULATE_VERSION_PATCH)
                {u"tabulate"_s,    u"%1.%2.%3"_s.arg(TABULATE_VERSION_MAJOR)
                                                .arg(TABULATE_VERSION_MINOR)
                                                .arg(TABULATE_VERSION_PATCH)},
#else
                {u"tabulate"_s,    u"<=1.3 (doesn't has <tabulate/tabulate.hpp>)"_s}}},
#endif
            }},
        {u"Build types"_s,
            std::map<QString, AboutValue> {
#ifdef TINYTOM_DEBUG
                {u"tom build type"_s, Debug_},
#else
                {u"tom build type"_s, Release_},
#endif
                {u"TinyORM build type"_s,          LibraryInfo::isDebugBuild()
                                                   ? Debug_ : Release_},
                {u"TinyORM full build type"_s,     LibraryInfo::build()},
#ifdef TINYORM_USING_TINYDRIVERS
                {u"TinyDrivers build type"_s,      DriversLibraryInfo::isDebugBuild()
                                                   ? Debug_ : Release_},
                {u"TinyDrivers full build type"_s, DriversLibraryInfo::build()},
#endif
                /* I have dropped the MySqlLibraryInfo because the TinyDrivers library has
                   all information about TinyMySql/... drivers and it would be problematic
                   to call these static methods if the driver is built as the loadable
                   module. I would need to store a handle to this loadable library and
                   then resolve those symbols and it would be practically for nothing. */
#ifdef TINYDRIVERS_MYSQL_DRIVER
                {u"TinyMySql build type"_s,        DriversLibraryInfo::isDebugBuild()
                                                   ? Debug_ : Release_},
                {u"TinyMySql full build type"_s,   DriversLibraryInfo::mysqlBuild()},
#endif
                {u"Qt build type"_s,               QLibraryInfo::isDebugBuild()
                                                   ? Debug_ : Release_},
                {u"Qt full build type"_s,          QLibraryInfo::build()},
            }},
        {u"Compiler"_s,
            std::map<QString, AboutValue> {
                {u"Compiler version"_s, TINYORM_COMPILER_STRING},
#ifdef TINYORM_SIMULATED_STRING
                {u"Simulated compiler version"_s, TINYORM_SIMULATED_STRING},
#endif
            }},
    };
}

void Application::showCommandsList(const int exitCode)
{
    // FUTURE tom, allocate on heap to make func const, I would have to pass the const Application & to the Command base class and instead of to use one Application::m_parser I would have to create own parser for Command, problem is with the saveOptions(), prependOptions(), and initializeParser(), they are used in commands and they modify Application instance silverqx
    ListCommand(*this, m_parser).run();

    exitApplication(exitCode);
}

void Application::exitApplication(const int exitCode)
{
    ::exit(exitCode); // NOLINT(concurrency-mt-unsafe)
}

/* Commands factory */

std::unique_ptr<Command>
Application::createCommand(const QString &command, const OptionalParserRef parser,
                           const bool showHelp)
{
    // Use a custom parser if passed as the argument, needed by CallsCommands::call()
    auto parserRef = parser ? *parser : std::ref(m_parser);

    if (command == About)
        return std::make_unique<AboutCommand>(*this, parserRef);

#if defined(__linux__) || defined(__MINGW32__)
    if (command == CompleteBash)
        return std::make_unique<BashCommand>(*this, parserRef);
#endif

    if (command == CompletePwsh)
        return std::make_unique<PwshCommand>(*this, parserRef);

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

    for (const auto *const argv : m_argv)
        arguments << QString::fromUtf8(argv);

    return arguments;
}

const std::vector<std::shared_ptr<Application::Command>> &
Application::createCommandsVector()
{
    static const std::vector<std::shared_ptr<Command>> cached = std::invoke([this]
    {
        return commandNames()
                | ranges::views::transform([this](const QString &commandName)
                                           -> std::shared_ptr<Command>
        {
            return createCommand(commandName);
        })
                | ranges::to<std::vector<std::shared_ptr<Command>>>();
    });

    return cached;
}

const std::unordered_map<QString, std::tuple<int, int>> &
Application::commandsByNamespaceHash()
{
    /* First number is index where it starts (0-based), second the number where it ends
       (it's like the iterator's end() so it must point after).
       Look to the Application::commandNames() to understand this indexes.
       tuple is forwarded as args to the ranges::views::slice(). */
    static const std::unordered_map<QString, std::tuple<int, int>>
    cached = std::invoke([]
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
    });

    return cached;
}

const std::vector<std::reference_wrapper<const QString>> &
Application::commandNames()
{
    /* Order is important here (shown by defined order by the list command and also must
       match namespaces order). */
    static const std::vector<std::reference_wrapper<const QString>> cached {
        // global namespace
        About, Env, Help, Inspire, Integrate, List, Migrate,
        // db
        DbSeed, DbWipe,
        // make
        MakeMigration, MakeModel, /*MakeProject,*/ MakeSeeder,
        // migrate
        MigrateFresh,  MigrateInstall,  MigrateRefresh, MigrateReset, MigrateRollback,
        MigrateStatus, MigrateUninstall,
        // complete
#if defined(__linux__) || defined(__MINGW32__)
        CompleteBash,
#endif
        CompletePwsh,
    };

    return cached;
}

const std::vector<std::reference_wrapper<const QString>> &
Application::namespaceNames()
{
    // Order is important here - zipped with the commandsIndexes()
    static const std::vector<std::reference_wrapper<const QString>> cached {
        // global namespace
        NsGlobal, EMPTY,
        // all other namespaces
        NsDb, NsMake, NsMigrate,
        // Hidden namespaces
        NsComplete, // Also contains a conditionally compiled BashCommand and therefore is at the end
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

       Order is important here - zipped with the namespaceNames(). */
    static const std::vector<std::tuple<int, int>> cached {
        { 0,  7}, // global
        { 0,  7}, // "" - also global

        { 7,  9}, // db
        { 9, 12}, // make
        {12, 19}, // migrate
#if defined(__linux__) || defined(__MINGW32__)
        {19, 21}, // complete (hidden namespace, will be excluded from the output)
        { 7, 21}, // namespaced
        { 0, 21}, // all
#else
        {19, 20}, // complete (hidden namespace, will be excluded from the output)
        { 7, 20}, // namespaced
        { 0, 20}, // all
#endif
    };

    return cached;
}

bool Application::isCommandHidden(const QString &command)
{
    static const std::unordered_set<QString> hiddenCommands {
#if defined(__linux__) || defined(__MINGW32__)
        Tom::Constants::CompleteBash,
#endif
        Tom::Constants::CompletePwsh,
    };

    return hiddenCommands.contains(command);
}

bool Application::isNamespaceHidden(const QString &namespaceName)
{
    static const std::unordered_set<QString> hiddenNamespaces {
        Tom::Constants::NsComplete,
    };

    return hiddenNamespaces.contains(namespaceName);
}

QList<CommandLineOption> Application::getCommandOptionsSignature(const QString &command)
{
    // Ownership of a unique_ptr()
    return createCommand(command, std::nullopt, false)->optionsSignature();
}

std::unique_ptr<QCoreApplication>
Application::createQCoreApplication(int &argc, char **argv)
{
    /* Only one instance of the QCoreApplication can exist in the whole application,
       auto tests create their own instance.
       The m_qtApplication isn't used anywhere but must be created if it doesn't exist.
       I decided to use the QCoreApplication everywhere instead of eg. the m_qtApplication
       or qtApplication()-> because it's much more clear what is happening.
       Don't use the ternary operator here because Clang-Tidy throws false positive
       warning! */
    if (QCoreApplication::instance() == nullptr)
        return std::make_unique<QCoreApplication>(argc, argv);

    return nullptr;
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

const Concerns::InteractsWithIO &Application::io() const noexcept
{
    /* This is our internal thing so the Q_ASSERT() is enough. I tried to make it public
       because the InteractsWithIO() class contains useful methods, but it's not fully
       ready until the TomApplication::run() method call and that's a problem.
       And that's why I made it protected. 🫤
       See the following commit for the whole refactor:
       tom removed tabulate/table.hpp from headers 🙌 (bef2b13f) */
    Q_ASSERT(m_io);
    return *m_io;
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

/* private */

bool Application::inUnitTests() noexcept
{
    return g_inUnitTests;
}
#endif

} // namespace Tom

TINYORM_END_COMMON_NAMESPACE

// CUR tom, commands I want to implement; test, schema:dump, model:show silverqx
