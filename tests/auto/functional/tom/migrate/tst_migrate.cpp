#include <QCoreApplication>
#include <QTest>

#ifdef TINYORM_USING_TINYDRIVERS
#  include "orm/exceptions/runtimeerror.hpp"
#endif
#include "orm/utils/type.hpp"

#include "tom/application.hpp"
#include "tom/commands/migrations/statuscommand.hpp"

#include "databases.hpp"
#include "macros.hpp"

#include "migrations/2014_10_12_000000_create_posts_table.hpp"
#include "migrations/2014_10_12_100000_add_factor_column_to_posts_table.hpp"
#include "migrations/2014_10_12_200000_create_properties_table.hpp"
#include "migrations/2014_10_12_300000_create_phones_table.hpp"

using Orm::Exceptions::RuntimeError;

using TypeUtils = Orm::Utils::Type;

using TomApplication = Tom::Application;

using Tom::Commands::Migrations::StatusCommand;
using Tom::Constants::Migrate;
using Tom::Constants::MigrateInstall;
using Tom::Constants::MigrateRefresh;
using Tom::Constants::MigrateReset;
using Tom::Constants::MigrateRollback;
using Tom::Constants::MigrateStatus;
using Tom::Constants::MigrateUninstall;

using TestUtils::Databases;

using namespace Migrations; // NOLINT(google-build-using-namespace)

class tst_Migrate : public QObject // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

public:
    /*! Alias for the test output row. */
    using StatusRow = StatusCommand::StatusRow;
    /*! Type used for comparing results of the status command. */
    using Status = std::vector<StatusRow>;

private Q_SLOTS:
    void initTestCase();
    void cleanup() const;
    void cleanupTestCase() const;

    void migrate() const;
    void migrate_Step() const;

    void reset() const;

    void rollback() const;
    void rollback_AfterMigrateWithStep() const;

    void rollback_Step() const;
    void rollback_Step_AfterMigrateWithStep() const;

    void rollback_Batch_MoreMigrations() const;
    void rollback_Batch_AfterMigrateWithStep() const;

    void refresh() const;
    void refresh_AfterMigrateWithStep() const;

    void refresh_Step() const;
    void refresh_StepMigrate() const;
    void refresh_Step_StepMigrate() const;

// NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    /*! Prepare arguments and invoke runCommand(). */
    [[nodiscard]] static int
    invokeCommand(const QString &connection, const QString &name,
                  const std::vector<const char *> &arguments = {},
                  const QString &migrationTable = MigrationsTable);
    /*! Create a tom application instance and invoke the given command. */
    static int runCommand(int &argc, const std::vector<const char *> &argv,
                          const QString &migrationTable);

    /*! Invoke the status command to obtain results. */
    inline static int invokeTestStatusCommand(const QString &connection);
    /*! Get result of the last status command. */
    static Status status();
    /*! Create a status object for comparing with the result of the status(). */
    static Status createStatus(std::initializer_list<StatusRow> rows);
    /*! Create a status object to be equal after complete rollback. */
    static Status createResetStatus();

    /*! Prepare the migration database for running. */
    void prepareDatabase() const;

    /*! Throw if the environment is production because it needs confirmation. */
    static void throwIfWrongEnvironment(const char *environmentEnvName);

    /*! Migrations table name (used by this test case). */
    inline static const auto MigrationsTable = sl("migrations_unit_testing");
    /*! Migrations table name (to cleanup the tom example migrations only). */
    inline static const auto MigrationsTomTable = sl("migrations_example");

    /*! Created database connections (needed by the cleanupTestCase()). */
    QStringList m_connections;
};

/*! Alias for the test output row. */
using Status   = tst_Migrate::Status;
/*! Type used for comparing results of the status command. */
using StatusRow = tst_Migrate::StatusRow;

/* Extracted common code to re-use. */
namespace
{
    // Status
    const auto *const Yes = "Yes";
    const auto *const No  = "No";

    // Batches
    const auto *const s_1  = "1";
    const auto *const s_2  = "2";
    const auto *const s_3  = "3";
    const auto *const s_4  = "4";
    const auto *const s_5  = "5";

    // Migration names
    const auto *const
    s_2014_10_12_000000_create_posts_table =
            "2014_10_12_000000_create_posts_table";
    const auto *const
    s_2014_10_12_100000_add_factor_column_to_posts_table =
            "2014_10_12_100000_add_factor_column_to_posts_table";
    const auto *const
    s_2014_10_12_200000_create_properties_table =
            "2014_10_12_200000_create_properties_table";
    const auto *const
    s_2014_10_12_300000_create_phones_table =
            "2014_10_12_300000_create_phones_table";

    // Fully migrated w/o --step
    const std::initializer_list<StatusRow>
    FullyMigrated = {
        {Yes, s_2014_10_12_000000_create_posts_table,               s_1},
        {Yes, s_2014_10_12_100000_add_factor_column_to_posts_table, s_1},
        {Yes, s_2014_10_12_200000_create_properties_table,          s_1},
        {Yes, s_2014_10_12_300000_create_phones_table,              s_1},
    };

    // Fully migrated with --step
    const std::initializer_list<StatusRow>
    FullyStepMigrated = {
        {Yes, s_2014_10_12_000000_create_posts_table,               s_1},
        {Yes, s_2014_10_12_100000_add_factor_column_to_posts_table, s_2},
        {Yes, s_2014_10_12_200000_create_properties_table,          s_3},
        {Yes, s_2014_10_12_300000_create_phones_table,              s_4},
    };

} // namespace

/* private slots */

// NOLINTBEGIN(readability-convert-member-functions-to-static)
void tst_Migrate::initTestCase()
{
    m_connections = Databases::createConnections({Databases::MYSQL,
                                                  Databases::MARIADB,
                                                  Databases::POSTGRESQL,
                                                  Databases::SQLITE,
                                                 });

    if (m_connections.isEmpty())
        QSKIP(TestUtils::AutoTestSkippedAny.arg(TypeUtils::classPureBasename(*this))
                                           .toUtf8().constData(), );

    QTest::addColumn<QString>("connection");

    // Run all tests for all supported database connections
    for (const auto &connection : std::as_const(m_connections))
        QTest::newRow(connection.toUtf8().constData()) << connection;

    /* Modify the migrate:status command to not output a status table to the console but
       instead return a result as the vector, this vector is then used for comparing
       results. */
    TomApplication::enableInUnitTests();

    // Prepare the migration database for running
    prepareDatabase();
}

void tst_Migrate::cleanup() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    /* All test methods need this except for two of them (reset and I don't remember
       second), I will not implement special logic to skip this for these two methods. */
    {
        auto exitCode = invokeCommand(connection, MigrateReset);

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    {
        auto exitCode = invokeTestStatusCommand(connection);

        QVERIFY(exitCode == EXIT_SUCCESS);
        QCOMPARE(status(), createResetStatus());
    }
}

void tst_Migrate::cleanupTestCase() const
{
    /* Uninstall the migration repository, eg. the tst_SchemaBuilder::getAllTables()
       depends on it. */
    for (const auto &connection : m_connections) {
        // Ownership of a unique_ptr()
        const auto &schema = Databases::manager().connection(connection)
                             .getSchemaBuilder();

        // Nothing to cleanup, the migration repository was already uninstalled
        if (!schema.hasTable(MigrationsTable))
            return;

        // Reset the migrations table
        {
            const auto exitCode = invokeCommand(connection, MigrateUninstall,
                                                {"--reset"});

            QVERIFY(exitCode == EXIT_SUCCESS);
        }

        {
            auto exitCode = invokeTestStatusCommand(connection);

            QVERIFY(exitCode == EXIT_FAILURE);
        }

        QVERIFY(!schema.hasTable(MigrationsTable));
    }
}

void tst_Migrate::migrate() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    {
        auto exitCode = invokeCommand(connection, Migrate);

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    {
        auto exitCode = invokeTestStatusCommand(connection);

        QVERIFY(exitCode == EXIT_SUCCESS);
        QCOMPARE(status(), createStatus(FullyMigrated));
    }
}

void tst_Migrate::migrate_Step() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    {
        auto exitCode = invokeCommand(connection, Migrate, {"--step"});

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    {
        auto exitCode = invokeTestStatusCommand(connection);

        QVERIFY(exitCode == EXIT_SUCCESS);
        QCOMPARE(status(), createStatus(FullyStepMigrated));
    }
}

void tst_Migrate::reset() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    {
        auto exitCode = invokeCommand(connection, MigrateReset);

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    {
        auto exitCode = invokeTestStatusCommand(connection);

        QVERIFY(exitCode == EXIT_SUCCESS);
        QCOMPARE(status(), createResetStatus());
    }
}

void tst_Migrate::rollback() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    {
        auto exitCode = invokeCommand(connection, Migrate);

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    {
        auto exitCode = invokeTestStatusCommand(connection);

        QVERIFY(exitCode == EXIT_SUCCESS);
        QCOMPARE(status(), createStatus(FullyMigrated));
    }

    // Rollback on previous migrate w/o --step
    {
        auto exitCode = invokeCommand(connection, MigrateRollback);

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    {
        auto exitCode = invokeTestStatusCommand(connection);

        QVERIFY(exitCode == EXIT_SUCCESS);
        QCOMPARE(status(), createResetStatus());
    }
}

void tst_Migrate::rollback_AfterMigrateWithStep() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    {
        auto exitCode = invokeCommand(connection, Migrate, {"--step"});

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    {
        auto exitCode = invokeTestStatusCommand(connection);

        QVERIFY(exitCode == EXIT_SUCCESS);
        QCOMPARE(status(), createStatus(FullyStepMigrated));
    }

    // Rollback on previous migrate with --step
    {
        auto exitCode = invokeCommand(connection, MigrateRollback);

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    {
        auto exitCode = invokeTestStatusCommand(connection);

        QVERIFY(exitCode == EXIT_SUCCESS);
        QCOMPARE(status(), createStatus({
            {Yes, s_2014_10_12_000000_create_posts_table,               s_1},
            {Yes, s_2014_10_12_100000_add_factor_column_to_posts_table, s_2},
            {Yes, s_2014_10_12_200000_create_properties_table,          s_3},
            {No,  s_2014_10_12_300000_create_phones_table},
        }));
    }
}

void tst_Migrate::rollback_Step() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    {
        auto exitCode = invokeCommand(connection, Migrate);

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    {
        auto exitCode = invokeTestStatusCommand(connection);

        QVERIFY(exitCode == EXIT_SUCCESS);
        QCOMPARE(status(), createStatus(FullyMigrated));
    }

    // Rollback on previous migrate w/o --step
    {
        auto exitCode = invokeCommand(connection, MigrateRollback, {"--step=2"});

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    {
        auto exitCode = invokeTestStatusCommand(connection);

        QVERIFY(exitCode == EXIT_SUCCESS);
        QCOMPARE(status(), createStatus({
            {Yes, s_2014_10_12_000000_create_posts_table,               s_1},
            {Yes, s_2014_10_12_100000_add_factor_column_to_posts_table, s_1},
            {No,  s_2014_10_12_200000_create_properties_table},
            {No,  s_2014_10_12_300000_create_phones_table},
        }));
    }
}

void tst_Migrate::rollback_Step_AfterMigrateWithStep() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    {
        auto exitCode = invokeCommand(connection, Migrate, {"--step"});

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    {
        auto exitCode = invokeTestStatusCommand(connection);

        QVERIFY(exitCode == EXIT_SUCCESS);
        QCOMPARE(status(), createStatus(FullyStepMigrated));
    }

    // Rollback on previous migrate with --step
    {
        auto exitCode = invokeCommand(connection, MigrateRollback, {"--step=2"});

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    {
        auto exitCode = invokeTestStatusCommand(connection);

        QVERIFY(exitCode == EXIT_SUCCESS);
        QCOMPARE(status(), createStatus({
            {Yes, s_2014_10_12_000000_create_posts_table,               s_1},
            {Yes, s_2014_10_12_100000_add_factor_column_to_posts_table, s_2},
            {No,  s_2014_10_12_200000_create_properties_table},
            {No,  s_2014_10_12_300000_create_phones_table},
        }));
    }
}

void tst_Migrate::rollback_Batch_MoreMigrations() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    // Prepare migrations to the exact state
    {
        auto exitCode = invokeCommand(connection, Migrate);

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    {
        auto exitCode = invokeCommand(connection, MigrateRollback, {"--step=3"});

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    {
        auto exitCode = invokeCommand(connection, Migrate);

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    // Verify preparations
    {
        auto exitCode = invokeTestStatusCommand(connection);

        QVERIFY(exitCode == EXIT_SUCCESS);
        QCOMPARE(status(), createStatus({
            {Yes, s_2014_10_12_000000_create_posts_table,               s_1},
            {Yes, s_2014_10_12_100000_add_factor_column_to_posts_table, s_2},
            {Yes, s_2014_10_12_200000_create_properties_table,          s_2},
            {Yes, s_2014_10_12_300000_create_phones_table,              s_2},
        }));
    }

    // Rollback on previous migrate w/o --step
    {
        auto exitCode = invokeCommand(connection, MigrateRollback, {"--batch=2"});

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    // Verify
    {
        auto exitCode = invokeTestStatusCommand(connection);

        QVERIFY(exitCode == EXIT_SUCCESS);
        QCOMPARE(status(), createStatus({
            {Yes, s_2014_10_12_000000_create_posts_table,               s_1},
            {No,  s_2014_10_12_100000_add_factor_column_to_posts_table     },
            {No,  s_2014_10_12_200000_create_properties_table              },
            {No,  s_2014_10_12_300000_create_phones_table                  },
        }));
    }
}

void tst_Migrate::rollback_Batch_AfterMigrateWithStep() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    {
        auto exitCode = invokeCommand(connection, Migrate, {"--step"});

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    {
        auto exitCode = invokeTestStatusCommand(connection);

        QVERIFY(exitCode == EXIT_SUCCESS);
        QCOMPARE(status(), createStatus(FullyStepMigrated));
    }

    // Rollback on previous migrate with --step
    {
        auto exitCode = invokeCommand(connection, MigrateRollback, {"--batch=2"});

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    {
        auto exitCode = invokeTestStatusCommand(connection);

        QVERIFY(exitCode == EXIT_SUCCESS);
        QCOMPARE(status(), createStatus({
            {Yes, s_2014_10_12_000000_create_posts_table,               s_1},
            {No,  s_2014_10_12_100000_add_factor_column_to_posts_table     },
            {Yes, s_2014_10_12_200000_create_properties_table,          s_3},
            {Yes, s_2014_10_12_300000_create_phones_table,              s_4},
        }));
    }

    // Migrate again
    {
        auto exitCode = invokeCommand(connection, Migrate);

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    // Verify the batch number
    {
        auto exitCode = invokeTestStatusCommand(connection);

        QVERIFY(exitCode == EXIT_SUCCESS);
        QCOMPARE(status(), createStatus({
            {Yes, s_2014_10_12_000000_create_posts_table,               s_1},
            {Yes, s_2014_10_12_100000_add_factor_column_to_posts_table, s_5},
            {Yes, s_2014_10_12_200000_create_properties_table,          s_3},
            {Yes, s_2014_10_12_300000_create_phones_table,              s_4},
        }));
    }

    // Rollback again
    {
        auto exitCode = invokeCommand(connection, MigrateRollback, {"--step=2"});

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    // Verify it
    {
        auto exitCode = invokeTestStatusCommand(connection);

        QVERIFY(exitCode == EXIT_SUCCESS);
        QCOMPARE(status(), createStatus({
            {Yes, s_2014_10_12_000000_create_posts_table,               s_1},
            {No,  s_2014_10_12_100000_add_factor_column_to_posts_table     },
            {Yes, s_2014_10_12_200000_create_properties_table,          s_3},
            {No,  s_2014_10_12_300000_create_phones_table,                 },
        }));
    }

    // Migrate again
    {
        auto exitCode = invokeCommand(connection, Migrate, {"--step"});

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    // Verify the batch numbers 🎉
    {
        auto exitCode = invokeTestStatusCommand(connection);

        QVERIFY(exitCode == EXIT_SUCCESS);
        QCOMPARE(status(), createStatus({
            {Yes, s_2014_10_12_000000_create_posts_table,               s_1},
            {Yes, s_2014_10_12_100000_add_factor_column_to_posts_table, s_4},
            {Yes, s_2014_10_12_200000_create_properties_table,          s_3},
            {Yes, s_2014_10_12_300000_create_phones_table,              s_5},
        }));
    }
}

void tst_Migrate::refresh() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    {
        auto exitCode = invokeCommand(connection, Migrate);

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    {
        auto exitCode = invokeTestStatusCommand(connection);

        QVERIFY(exitCode == EXIT_SUCCESS);
        QCOMPARE(status(), createStatus(FullyMigrated));
    }

    // Refresh on previous migrate w/o --step
    {
        auto exitCode = invokeCommand(connection, MigrateRefresh);

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    {
        auto exitCode = invokeTestStatusCommand(connection);

        QVERIFY(exitCode == EXIT_SUCCESS);
        QCOMPARE(status(), createStatus(FullyMigrated));
    }
}

void tst_Migrate::refresh_AfterMigrateWithStep() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    {
        auto exitCode = invokeCommand(connection, Migrate, {"--step"});

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    {
        auto exitCode = invokeTestStatusCommand(connection);

        QVERIFY(exitCode == EXIT_SUCCESS);
        QCOMPARE(status(), createStatus(FullyStepMigrated));
    }

    // Refresh on previous migrate with --step
    {
        auto exitCode = invokeCommand(connection, MigrateRefresh);

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    {
        auto exitCode = invokeTestStatusCommand(connection);

        QVERIFY(exitCode == EXIT_SUCCESS);
        QCOMPARE(status(), createStatus(FullyMigrated));
    }
}

void tst_Migrate::refresh_Step() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    {
        auto exitCode = invokeCommand(connection, Migrate);

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    {
        auto exitCode = invokeTestStatusCommand(connection);

        QVERIFY(exitCode == EXIT_SUCCESS);
        QCOMPARE(status(), createStatus(FullyMigrated));
    }

    // Refresh on previous migrate w/o --step
    {
        auto exitCode = invokeCommand(connection, MigrateRefresh, {"--step=2"});

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    {
        auto exitCode = invokeTestStatusCommand(connection);

        QVERIFY(exitCode == EXIT_SUCCESS);
        QCOMPARE(status(), createStatus({
            {Yes, s_2014_10_12_000000_create_posts_table,               s_1},
            {Yes, s_2014_10_12_100000_add_factor_column_to_posts_table, s_1},
            {Yes, s_2014_10_12_200000_create_properties_table,          s_2},
            {Yes, s_2014_10_12_300000_create_phones_table,              s_2},
        }));
    }
}

void tst_Migrate::refresh_StepMigrate() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    {
        auto exitCode = invokeCommand(connection, Migrate);

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    {
        auto exitCode = invokeTestStatusCommand(connection);

        QVERIFY(exitCode == EXIT_SUCCESS);
        QCOMPARE(status(), createStatus(FullyMigrated));
    }

    // Refresh on previous migrate w/o --step
    {
        auto exitCode = invokeCommand(connection, MigrateRefresh, {"--step-migrate"});

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    {
        auto exitCode = invokeTestStatusCommand(connection);

        QVERIFY(exitCode == EXIT_SUCCESS);
        QCOMPARE(status(), createStatus(FullyStepMigrated));
    }
}

void tst_Migrate::refresh_Step_StepMigrate() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    {
        auto exitCode = invokeCommand(connection, Migrate);

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    {
        auto exitCode = invokeTestStatusCommand(connection);

        QVERIFY(exitCode == EXIT_SUCCESS);
        QCOMPARE(status(), createStatus(FullyMigrated));
    }

    // Refresh on previous migrate w/o --step
    {
        auto exitCode = invokeCommand(connection, MigrateRefresh,
                                      {"--step=2", "--step-migrate"});

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    {
        auto exitCode = invokeTestStatusCommand(connection);

        QVERIFY(exitCode == EXIT_SUCCESS);
        QCOMPARE(status(), createStatus({
            {Yes, s_2014_10_12_000000_create_posts_table,               s_1},
            {Yes, s_2014_10_12_100000_add_factor_column_to_posts_table, s_1},
            {Yes, s_2014_10_12_200000_create_properties_table,          s_2},
            {Yes, s_2014_10_12_300000_create_phones_table,              s_3},
        }));
    }
}
// NOLINTEND(readability-convert-member-functions-to-static)

/* private */

int tst_Migrate::invokeCommand(const QString &connection, const QString &name,
                               const std::vector<const char *> &arguments,
                               const QString &migrationTable)
{
    static const auto connectionTmpl = sl("--database=%1");

    // Prepare fake argc and argv
    const auto nameArr = name.toUtf8();
    // DB connection to use
    const auto connectionArr = connectionTmpl.arg(connection).toUtf8();

    std::vector<const char *> argv {
#ifdef _WIN32
        "tom.exe",
#else
        "tom",
#endif
        nameArr.constData(),
        connectionArr.constData(),
//        "-vvv",
    };

    argv.reserve(arguments.size());
    std::ranges::copy(arguments, std::back_inserter(argv));

    int argc = static_cast<int>(argv.size());

    return runCommand(argc, argv, migrationTable);
}

int tst_Migrate::runCommand(int &argc, const std::vector<const char *> &argv,
                            const QString &migrationTable)
{
    // Current environment variable name
    const auto *const environmentEnvName = "TOM_TESTS_ENV";

    try {
        // Throw if the environment is production because it needs confirmation
        throwIfWrongEnvironment(environmentEnvName);

        // env. should be always local or development
        return TomApplication(argc, const_cast<char **>(argv.data()),
                              Databases::managerShared(), environmentEnvName,
                              migrationTable)
                .migrations<CreatePostsTable,
                            AddFactorColumnToPostsTable,
                            CreatePropertiesTable,
                            CreatePhonesTable>()
                // Fire it up 🔥🚀✨
                .runWithArguments({argv.cbegin(), argv.cend()});

    } catch (const std::exception &e) {

        TomApplication::logException(e, true);
    }

    return EXIT_FAILURE;
}

int tst_Migrate::invokeTestStatusCommand(const QString &connection)
{
    return invokeCommand(connection, MigrateStatus);
}

Status tst_Migrate::status()
{
    return TomApplication::status();
}

Status tst_Migrate::createStatus(std::initializer_list<StatusRow> rows)
{
    return rows;
}

Status tst_Migrate::createResetStatus()
{
    return {
        {No, s_2014_10_12_000000_create_posts_table},
        {No, s_2014_10_12_100000_add_factor_column_to_posts_table},
        {No, s_2014_10_12_200000_create_properties_table},
        {No, s_2014_10_12_300000_create_phones_table},
    };
}

void tst_Migrate::prepareDatabase() const
{
    for (const auto &connection : m_connections) {
        // Ownership of a unique_ptr()
        const auto &schema = Databases::manager().connection(connection)
                             .getSchemaBuilder();

        // Create the migrations table if needed
        {
            if (!schema.hasTable(MigrationsTable)) {
                auto exitCode = invokeCommand(connection, MigrateInstall);

                QVERIFY(exitCode == EXIT_SUCCESS);
            }

            // Reset the migrations table (created by this test case)
            else {
                auto exitCode = invokeCommand(connection, MigrateReset);

                QVERIFY(exitCode == EXIT_SUCCESS);
            }
        }

        // Remove the migrations from the tom example application to avoid tests failing
        {
            // Nothing to do, migration repository isn't installed
            if (!schema.hasTable(MigrationsTomTable))
                continue;

            // Drop all tables and uninstall the migration repository
            {
                auto exitCode = invokeCommand(connection, MigrateUninstall, {"--reset"},
                                              MigrationsTomTable);

                QVERIFY(exitCode == EXIT_SUCCESS);
            }

            QVERIFY(!schema.hasTable(MigrationsTomTable));
        }
    }
}

void tst_Migrate::throwIfWrongEnvironment(const char *const environmentEnvName)
{
    const auto environment = qEnvironmentVariable(environmentEnvName);

    if (environment != QLatin1String("production") &&
        environment != QLatin1String("prod")
    )
        return;

    throw RuntimeError(
                sl("The '%1' environment variable can't be 'prod' or 'production' "
                   "because production environment needs confirmation, please set it "
                   "to any other supported value as local, development, or testing.")
                .arg(environmentEnvName));
}

QTEST_MAIN(tst_Migrate)

#include "tst_migrate.moc"
