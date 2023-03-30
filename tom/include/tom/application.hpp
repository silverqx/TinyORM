#pragma once
#ifndef TOM_APPLICATION_HPP
#define TOM_APPLICATION_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QCommandLineParser>

#include <filesystem>
#include <typeindex>
#include <typeinfo>

#include <range/v3/view/slice.hpp>

#include "tom/config.hpp" // IWYU pragma: keep

#include "tom/concerns/guesscommandname.hpp"
#include "tom/concerns/interactswithio.hpp"
#include "tom/tomtypes.hpp"
#include "tom/types/commandlineoption.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{
    class ConnectionResolverInterface;
    class DatabaseManager;
}

namespace Tom {

namespace Commands
{
    class CompleteCommand;
    class HelpCommand;
    class ListCommand;
} // namespace Commands
namespace Concerns
{
    class CallsCommands;
    class PrintsOptions;
} // namespace Concerns

    class Migration;
    class MigrationRepository;
    class Migrator;
    class Seeder;

    /*! Tom application. */
    class SHAREDLIB_EXPORT Application : public Concerns::InteractsWithIO,
                                         public Concerns::GuessCommandName
    {
        Q_DISABLE_COPY(Application)

        // To access saveOptions()
        friend Commands::Command;
        // To access createCommand(), namespaceNames(), guessCommandXyz() related methods
        friend Commands::CompleteCommand;
        // To access createCommand(), m_options
        friend Commands::HelpCommand;
        // To access showVersion(), m_options
        friend Commands::ListCommand;
        // To access m_options
        friend Concerns::PrintsOptions;
        // To access initializeParser() and createCommand()
        friend Concerns::CallsCommands;
        // To access createCommandsVector(), errorWall(), exitApplication()
        friend Concerns::GuessCommandName;

        /*! Alias for the ConnectionResolverInterface. */
        using ConnectionResolverInterface = Orm::ConnectionResolverInterface;
        /*! Alias for the DatabaseManager. */
        using DatabaseManager = Orm::DatabaseManager;

    public:
        /*! Constructor. */
        Application(int &argc, char **argv, std::shared_ptr<DatabaseManager> db,
                    const char *environmentEnvName = "TOM_ENV",
                    QString migrationTable = QStringLiteral("migrations"),
                    std::vector<std::shared_ptr<Migration>> migrations = {},
                    std::vector<std::shared_ptr<Seeder>> seeders = {});
        /*! Virtual destructor. */
        inline ~Application() override = default;

        /*! Instantiate/initialize all migration classes. */
        template<typename ...Migrations>
        Application &migrations();

        /*! Instantiate/initialize all seeder classes. */
        template<typename ...Seeders>
        Application &seeders();

        /*! Run the tom application. */
        int run();

        /*! Log exception caught in the main exception handler in a current thread. */
        static void logException(const std::exception &e, bool noAnsi = false);

        /* Getters / Setters */
        /*! Get a current application environment. */
        inline const QString &environment() const noexcept;
        /*! Get database manager. */
        inline DatabaseManager &db() const noexcept;

        /*! Get command-line parser. */
        inline const QCommandLineParser &parser() const noexcept;
        /*! Is the application running in an interactive mode? */
        inline bool isInteractive() const noexcept;
        /*! Obtain current command-line arguments. */
        QStringList arguments() const;

        /*! Set the migration repository table name. */
        inline Application &migrationTable(QString table);

        /*! Alias for the filesystem path. */
        using fspath = std::filesystem::path;

        /*! Set the default migrations path for the make:migration command. */
        Application &migrationsPath(fspath path);
        /*! Set the default seeders path for the make:seeder command. */
        Application &seedersPath(fspath path);

        /*! Get the default migrations path used by the make:migration command. */
        inline const fspath &getMigrationsPath() const noexcept;
        /*! Get the default models path used by the make:model command. */
        inline const fspath &getModelsPath() const noexcept;
        /*! Get the default seeders path used by the make:seeder command. */
        inline const fspath &getSeedersPath() const noexcept;

        /*! Get a reference to the all migrations instances. */
        inline const std::vector<std::shared_ptr<Migration>> &
        getMigrations() const noexcept;
        /*! Get a reference to the all seeders instances. */
        inline const std::vector<std::shared_ptr<Seeder>> &
        getSeeders() const noexcept;

#ifdef TINYTOM_TESTS_CODE
        /*! Alias for the test output row from the status command. */
        using StatusRow = std::vector<std::string>;

        /*! Get result of the status command (used in auto tests). */
        static std::vector<StatusRow> status();
        /*! Enable logic for unit testing? */
        static void enableInUnitTests() noexcept;
#endif

    protected:
        /*! Alias for the commands' base class. */
        using Command = Commands::Command;

        /*! Fix m_argc/m_argv data members if the argv is empty. */
        void fixEmptyArgv();
        /*! Processes the specified function at application's normal exit. */
//        static void initializeAtExit();

        /*! Initialize the command-line parser. */
        void initializeParser(QCommandLineParser &parser);
        /*! Save a copy of application options passed to the Qt's parser. */
        QList<QCommandLineOption>
        saveOptions(QList<CommandLineOption> &&options);

        /*! Prepend command options before common options (used by the help command). */
        void prependOptions(QList<CommandLineOption> &&options);

        /* Run command */
        /*! Parse current application's command-line. */
        void parseCommandLine();

        /*! Initialize environment value, order:
            development -> value from env. variable -> --env command-line argument. */
        void initializeEnvironment();
        /*! Obtain command name to run. */
        QString getCommandName();

        /*! Action to do if the passed command was not found. */
        enum struct CommandNotFound
        {
            /*! Show all commands list using the list command. */
            ShowCommandsList,
            /*! Show a command not defined error wall. */
            ShowErrorWall,
        };
        /*! Show all commands list using the list command. */
        constexpr static CommandNotFound
        ShowCommandsList = CommandNotFound::ShowCommandsList;
        /*! Show a command not defined error wall. */
        constexpr static CommandNotFound
        ShowErrorWall    = CommandNotFound::ShowErrorWall;

        /*! Get the command name including the guess command name logic. */
        QString getCommandName(const QString &name, CommandNotFound notFound);

        /* Early exit during parse command-line */
        /*! Display the version information and exits. */
        Q_NORETURN void showVersion() const;
        /*! Display the version information. */
        void printVersion() const;
        /*! Invoke the list command. */
        Q_NORETURN void showCommandsList(int exitCode);
        /*! Exit the application with post routines. */
        Q_NORETURN static void exitApplication(int exitCode);

        /* Commands factory */
        /*! Alias for an optional command-line parser reference. */
        using OptionalParserRef =
                std::optional<std::reference_wrapper<QCommandLineParser>>;

        /*! Create command by the given name. */
        std::unique_ptr<Command>
        createCommand(const QString &command, OptionalParserRef parser = std::nullopt,
                      bool showHelp = true);

        /*! Migration repository instance. */
        std::shared_ptr<MigrationRepository> createMigrationRepository() const;
        /*! Migrator instance. */
        std::shared_ptr<Migrator> createMigrator() const;

        /* Others */
        /*! Get arguments list from the m_argv array. */
        QStringList prepareArguments() const;

        /*! Get all supported commands list (used by the list command). */
        const std::vector<std::shared_ptr<Command>> &createCommandsVector();
        /*! Hash that maps namespaces to command indexes from createCommandsVector(). */
        static const std::unordered_map<QString, std::tuple<int, int>> &
        commandsByNamespaceHash();
        /*! Obtain all commands in the given namespace. */
        inline auto getCommandsInNamespace(const QString &name);

        /*! Get all supported commands' names. */
        static const std::vector<std::reference_wrapper<const QString>> &commandNames();
        /*! Get all commands' namespace names. */
        static const std::vector<std::reference_wrapper<const QString>> &namespaceNames();
        /*! Get commands index positions in namespaces. */
        static const std::vector<std::tuple<int, int>> &commandsIndexes();

        /*! Get options signature for the given command. */
        QList<CommandLineOption> getCommandOptionsSignature(const QString &command);

        /*! Initialize the migrations or seeders path (prepend pwd and make_prefered). */
        static fspath initializePath(fspath &&path);

        /*! Get database connection resolver. */
        std::shared_ptr<ConnectionResolverInterface> connectionResolver() const noexcept;

        /*! Throw if no connection configuration is registered. */
        void throwIfNoConnectionConfig() const;
        /*! Throw if a default connection is empty. */
        void throwIfEmptyDefaultConnection() const;

        /*! Current application argc. */
        int &m_argc; // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
        /*! Current application argv. */
        char **m_argv;

        /*! DatabaseManager instance. */
        std::shared_ptr<DatabaseManager> m_db;
        /*! The migration repository instance (cache only one instance created). */
        mutable std::shared_ptr<MigrationRepository> m_repository = nullptr;
        /*! The migrator service instance (cache only one instance created). */
        mutable std::shared_ptr<Migrator> m_migrator = nullptr;

        /* Only one instance can exist in the whole application, auto tests create their
           own QCoreApplication instance so this has to be excluded. */
#ifndef TINYTOM_TESTS_CODE
        /*! Qt's application instance. */
        QCoreApplication m_qtApplication;
        /*! Determine whether the TomApplication has its own QCoreApplication instance. */
        bool hasQtApplication = true;
#else
        /*! Determine whether the TomApplication has its own QCoreApplication instance. */
        bool hasQtApplication = false;
#endif
        /*! Command-line parser. */
        QCommandLineParser m_parser {};

        /*! Current environment. */
        QString m_environment = QStringLiteral("development");
        /*! Environment variable name that holds a current environment value. */
        const char *m_environmentEnvName;
        /*! Migration repository table name. */
        QString m_migrationTable;
        /*! Migrations path for the make:migration command. */
        std::filesystem::path m_migrationsPath;
        /*! Models path for the make:model command. */
        std::filesystem::path m_modelsPath;
        /*! Seeders path for the make:seeder command. */
        std::filesystem::path m_seedersPath;

        /*! Migrations vector to process. */
        std::vector<std::shared_ptr<Migration>> m_migrations;
        /*! Cache the migration properties by the migration type-id. */
        std::unordered_map<std::type_index,
                           MigrationProperties> m_migrationsProperties {};

        /*! Seeders vector to process. */
        std::vector<std::shared_ptr<Seeder>> m_seeders;

        /*! Is this input means interactive? */
        bool m_interactive = true;

        /*! Application options (more info at the cpp file beginning). */
        QList<CommandLineOption> m_options {};

        /* Auto tests helpers */
#ifdef TINYTOM_TESTS_CODE
    public:
        /*! Run the tom application with the given arguments (used in auto tests). */
        int runWithArguments(QStringList &&arguments);
#endif
    };

    /* public */

    template<typename ...Migrations>
    Application &Application::migrations()
    {
        m_migrations = {std::make_shared<Migrations>()...};

        // Correct sort order is checked in the Migrator::createMigrationNamesMap()

        /* Helps to avoid declaring the FileName, connection, and withinTransaction as
           getters in migrations, I can tell that this is really crazy. 🤪🙃😎 */
        m_migrationsProperties = {
            {typeid (Migrations), {Migrations::FileName,
                                   Migrations().connection,
                                   Migrations().withinTransaction}}...
        };

        /* Can be also written as the following, but I don't like it so. 😎
           dynamic_cast<Migrations &>(*g_migrations.at(index)).connection,
           dynamic_cast<Migrations &>(*g_migrations.at(index++)).withinTransaction */

        return *this;
    }

    template<typename ...Seeders>
    Application &Application::seeders()
    {
        m_seeders = {std::make_shared<Seeders>()...};

        return *this;
    }

    /* Getters / Setters */

    const QString &Application::environment() const noexcept
    {
        return m_environment;
    }

    Application::DatabaseManager &Application::db() const noexcept
    {
        return *m_db;
    }

    const QCommandLineParser &Application::parser() const noexcept
    {
        return m_parser;
    }

    bool Application::isInteractive() const noexcept
    {
        return m_interactive;
    }

    Application &Application::migrationTable(QString table)
    {
        m_migrationTable = std::move(table);

        return *this;
    }

    const std::filesystem::path &Application::getMigrationsPath() const noexcept
    {
        return m_migrationsPath;
    }

    const std::filesystem::path &Application::getModelsPath() const noexcept
    {
        return m_modelsPath;
    }

    const std::filesystem::path &Application::getSeedersPath() const noexcept
    {
        return m_seedersPath;
    }

    const std::vector<std::shared_ptr<Migration>> &
    Application::getMigrations() const noexcept
    {
        return m_migrations;
    }

    const std::vector<std::shared_ptr<Seeder>> &
    Application::getSeeders() const noexcept
    {
        return m_seeders;
    }

    /* protected */

    auto Application::getCommandsInNamespace(const QString &name)
    {
        Q_ASSERT(commandsByNamespaceHash().contains(name));

        return createCommandsVector()
                | std::apply(ranges::views::slice, commandsByNamespaceHash().at(name));
    }

} // namespace Tom

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_APPLICATION_HPP
