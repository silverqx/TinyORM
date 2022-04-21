#pragma once
#ifndef TOM_APPLICATION_HPP
#define TOM_APPLICATION_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QCommandLineParser>

#include "tom/config.hpp"

#include "tom/concerns/interactswithio.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{
    class DatabaseManager;
}

namespace Tom {

namespace Commands
{
    class Command;
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

    /*! Tom application. */
    class SHAREDLIB_EXPORT Application : public Concerns::InteractsWithIO
    {
        Q_DISABLE_COPY(Application)

        // To access saveOptions()
        friend Commands::Command;
        // To access createCommand()
        friend Commands::HelpCommand;
        // To access showVersion()
        friend Commands::ListCommand;
        // To access m_options
        friend Concerns::PrintsOptions;
        // To access initializeParser() and createCommand()
        friend Concerns::CallsCommands;

        /*! Alias for the DatabaseManager. */
        using DatabaseManager = Orm::DatabaseManager;

    public:
        /*! Constructor. */
        Application(int &argc, char **argv, std::shared_ptr<DatabaseManager> db,
                    const char *environmentEnvName = "TOM_ENV",
                    QString migrationTable = QLatin1String("migrations"),
                    std::vector<std::shared_ptr<Migration>> migrations = {});
        /*! Default destructor. */
        inline ~Application() = default;

        /*! Instantiate/initialize all migration classes. */
        template<typename ...M>
        Application &migrations();

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
        void initializeAtExit() const;

        /*! Initialize the command-line parser. */
        void initializeParser(QCommandLineParser &parser);
        /*! Save a copy of application options passed to the Qt's parser. */
        const QList<QCommandLineOption> &
        saveOptions(QList<QCommandLineOption> &&options);

        /*! Prepend command options before common options (used by the help command). */
        QList<QCommandLineOption>
        prependOptions(QList<QCommandLineOption> &&options);

        /* Run command */
        /*! Parse current application's command line. */
        void parseCommandLine();

        /*! Initialize environment value, order:
            development -> value from env. variable -> --env command-line argument. */
        void initializeEnvironment();
        /*! Obtain command name to run. */
        QString getCommandName();

        /* Early exit during parse command-line */
        /*! Display the version information and exits. */
        Q_NORETURN void showVersion() const;
        /*! Display the version information. */
        void printVersion() const;
        /*! Invoke the list command. */
        Q_NORETURN void showCommandsList(int exitCode);
        /*! Exit the application with post routines. */
        Q_NORETURN void exitApplication(int exitCode) const;

        /* Commands factory */
        /*! Alias for an optional command-line parser reference. */
        using OptionalParserRef =
                std::optional<std::reference_wrapper<QCommandLineParser>>;

        /*! Create command by the given name. */
        std::unique_ptr<Command>
        createCommand(const QString &command, OptionalParserRef parser = std::nullopt,
                      bool showHelp = true);

        /*! Migration repository instance. */
        std::shared_ptr<MigrationRepository> createMigrationRepository();
        /*! Migrator instance. */
        std::shared_ptr<Migrator> createMigrator();

        /* Others */
        /*! Get all supported commands list (used by the list command). */
        const std::vector<std::shared_ptr<Command>> &createCommandsVector();
        /*! Get all supported commands' names. */
        const std::vector<const char *> &commandNames() const;
        /*! Get arguments list from the m_argv array. */
        QStringList prepareArguments() const;

        /*! Current application argc. */
        int &m_argc;
        /*! Current application argv. */
        char **m_argv;

        /*! DatabaseManager instance. */
        std::shared_ptr<DatabaseManager> m_db;
        /*! The migration repository instance. */
        std::shared_ptr<MigrationRepository> m_repository = nullptr;
        /*! The migrator service instance. */
        std::shared_ptr<Migrator> m_migrator = nullptr;

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
        /*! Command line parser. */
        QCommandLineParser m_parser {};

        /*! Current environment. */
        QString m_environment = QStringLiteral("development");
        /*! Environment variable name that holds a current environment value. */
        const char *m_environmentEnvName;
        /*! Migration repository table name. */
        QString m_migrationTable;

        /*! Migrations vector to process. */
        std::vector<std::shared_ptr<Migration>> m_migrations;
        /*! Is this input means interactive? */
        bool m_interactive = true;

        /*! Application options. */
        QList<QCommandLineOption> m_options {};

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

} // namespace Tom

TINYORM_END_COMMON_NAMESPACE

#endif // TOM_APPLICATION_HPP
