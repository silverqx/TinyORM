#include <QCoreApplication>
#include <QtTest>

#include "orm/utils/type.hpp"

#include "tom/application.hpp"
#include "tom/commands/migrations/statuscommand.hpp"

#include "databases.hpp"

#include "migrations/2014_10_12_000000_create_posts_table.hpp"
#include "migrations/2014_10_12_100000_add_factor_column_to_posts_table.hpp"
#include "migrations/2014_10_12_200000_create_properties_table.hpp"
#include "migrations/2014_10_12_300000_create_phones_table.hpp"

using TypeUtils = Orm::Utils::Type;

using TomApplication = Tom::Application;

using Tom::Commands::Migrations::StatusCommand;
using Tom::Constants::Migrate;
using Tom::Constants::MigrateInstall;
using Tom::Constants::MigrateRefresh;
using Tom::Constants::MigrateReset;
using Tom::Constants::MigrateRollback;
using Tom::Constants::MigrateStatus;

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
    void initTestCase() const;
    void cleanup() const;

    void migrate() const;
    void migrate_Step() const;

    void reset() const;

    void rollback_OnMigrate() const;
    void rollback_OnMigrateWithStep() const;

    void rollback_Step_OnMigrate() const;
    void rollback_Step_OnMigrateWithStep() const;

    void refresh_OnMigrate() const;
    void refresh_OnMigrateWithStep() const;

    void refresh_Step() const;
    void refresh_StepMigrate() const;
    void refresh_Step_StepMigrate() const;

// NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    /*! Prepare arguments and invoke runCommand(). */
    [[nodiscard]] int
    static invokeCommand(const QString &connection, const QString &name,
                         std::vector<const char *> &&arguments = {});
    /*! Create a tom application instance and invoke the given command. */
    static int runCommand(int &argc, const std::vector<const char *> &argv);

    /*! Invoke the status command to obtain results. */
    inline static int invokeTestStatusCommand(const QString &connection);
    /*! Get result of the last status command. */
    static Status status();
    /*! Create a status object for comparing with the result of the status(). */
    static Status createStatus(std::initializer_list<StatusRow> rows);
    /*! Create a status object to be equal after complete rollback. */
    static Status createResetStatus();

    /*! Prepare the migration database for running. */
    static void prepareDatabase(const QStringList &connections);

    /*! Migrations table name. */
    inline static const auto MigrationsTable = QStringLiteral("migrations_unit_testing");
};

/*! Alias for the test output row. */
using Status   = tst_Migrate::Status;
/*! Type used for comparing results of the status command. */
using StatusRow = tst_Migrate::StatusRow;

/* Extracted common code to re-use. */
namespace
{
    // Status
    const auto *Yes = "Yes";
    const auto *No  = "No";

    // Batches
    const auto *s_1  = "1";
    const auto *s_2  = "2";
    const auto *s_3  = "3";
    const auto *s_4  = "4";

    // Migration names
    const auto *
    s_2014_10_12_000000_create_posts_table =
            "2014_10_12_000000_create_posts_table";
    const auto *
    s_2014_10_12_100000_add_factor_column_to_posts_table =
            "2014_10_12_100000_add_factor_column_to_posts_table";
    const auto *
    s_2014_10_12_200000_create_properties_table =
            "2014_10_12_200000_create_properties_table";
    const auto *
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
void tst_Migrate::initTestCase() const
{
    const auto &connections =
            Databases::createConnections({Databases::MYSQL, Databases::POSTGRESQL});

    if (connections.isEmpty())
        QSKIP(TestUtils::AutoTestSkippedAny.arg(TypeUtils::classPureBasename(*this))
                                           .toUtf8().constData(), );

    QTest::addColumn<QString>("connection");

    // Run all tests for all supported database connections
    for (const auto &connection : connections)
        QTest::newRow(connection.toUtf8().constData()) << connection;

    /* Modify the migrate:status command to not output a status table to the console but
       instead return a result as the vector, this vector is then used for comparing
       results. */
    TomApplication::enableInUnitTests();

    // Prepare the migration database for running
    prepareDatabase(connections);
}

void tst_Migrate::cleanup() const
{
    QFETCH_GLOBAL(QString, connection);

    /* All test methods need this except for two of them (reset and I don't remember
       second), I will not implement special logic to skip this for these two methods. */
    {
        auto exitCode = invokeCommand(connection, MigrateReset);

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    {
        auto exitCode = invokeTestStatusCommand(connection);

        QVERIFY(exitCode == EXIT_SUCCESS);
        QCOMPARE(createResetStatus(), status());
    }
}

void tst_Migrate::migrate() const
{
    QFETCH_GLOBAL(QString, connection);

    {
        auto exitCode = invokeCommand(connection, Migrate);

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    {
        auto exitCode = invokeTestStatusCommand(connection);

        QVERIFY(exitCode == EXIT_SUCCESS);
        QCOMPARE(createStatus(FullyMigrated), status());
    }
}

void tst_Migrate::migrate_Step() const
{
    QFETCH_GLOBAL(QString, connection);

    {
        auto exitCode = invokeCommand(connection, Migrate, {"--step"});

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    {
        auto exitCode = invokeTestStatusCommand(connection);

        QVERIFY(exitCode == EXIT_SUCCESS);
        QCOMPARE(createStatus(FullyStepMigrated), status());
    }
}

void tst_Migrate::reset() const
{
    QFETCH_GLOBAL(QString, connection);

    {
        auto exitCode = invokeCommand(connection, MigrateReset);

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    {
        auto exitCode = invokeTestStatusCommand(connection);

        QVERIFY(exitCode == EXIT_SUCCESS);
        QCOMPARE(createResetStatus(), status());
    }
}

void tst_Migrate::rollback_OnMigrate() const
{
    QFETCH_GLOBAL(QString, connection);

    {
        auto exitCode = invokeCommand(connection, Migrate);

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    {
        auto exitCode = invokeTestStatusCommand(connection);

        QVERIFY(exitCode == EXIT_SUCCESS);
        QCOMPARE(createStatus(FullyMigrated), status());
    }

    // rollback on previous migrate w/o --step
    {
        auto exitCode = invokeCommand(connection, MigrateRollback);

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    {
        auto exitCode = invokeTestStatusCommand(connection);

        QVERIFY(exitCode == EXIT_SUCCESS);
        QCOMPARE(createResetStatus(), status());
    }
}

void tst_Migrate::rollback_OnMigrateWithStep() const
{
    QFETCH_GLOBAL(QString, connection);

    {
        auto exitCode = invokeCommand(connection, Migrate, {"--step"});

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    {
        auto exitCode = invokeTestStatusCommand(connection);

        QVERIFY(exitCode == EXIT_SUCCESS);
        QCOMPARE(createStatus(FullyStepMigrated), status());
    }

    // rollback on previous migrate with --step
    {
        auto exitCode = invokeCommand(connection, MigrateRollback);

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    {
        auto exitCode = invokeTestStatusCommand(connection);

        QVERIFY(exitCode == EXIT_SUCCESS);
        QCOMPARE(createStatus({
            {Yes, s_2014_10_12_000000_create_posts_table,               s_1},
            {Yes, s_2014_10_12_100000_add_factor_column_to_posts_table, s_2},
            {Yes, s_2014_10_12_200000_create_properties_table,          s_3},
            {No,  s_2014_10_12_300000_create_phones_table},
        }), status());
    }
}

void tst_Migrate::rollback_Step_OnMigrate() const
{
    QFETCH_GLOBAL(QString, connection);

    {
        auto exitCode = invokeCommand(connection, Migrate);

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    {
        auto exitCode = invokeTestStatusCommand(connection);

        QVERIFY(exitCode == EXIT_SUCCESS);
        QCOMPARE(createStatus(FullyMigrated), status());
    }

    // rollback on previous migrate w/o --step
    {
        auto exitCode = invokeCommand(connection, MigrateRollback, {"--step=2"});

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    {
        auto exitCode = invokeTestStatusCommand(connection);

        QVERIFY(exitCode == EXIT_SUCCESS);
        QCOMPARE(createStatus({
            {Yes, s_2014_10_12_000000_create_posts_table,               s_1},
            {Yes, s_2014_10_12_100000_add_factor_column_to_posts_table, s_1},
            {No,  s_2014_10_12_200000_create_properties_table},
            {No,  s_2014_10_12_300000_create_phones_table},
        }), status());
    }
}

void tst_Migrate::rollback_Step_OnMigrateWithStep() const
{
    QFETCH_GLOBAL(QString, connection);

    {
        auto exitCode = invokeCommand(connection, Migrate, {"--step"});

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    {
        auto exitCode = invokeTestStatusCommand(connection);

        QVERIFY(exitCode == EXIT_SUCCESS);
        QCOMPARE(createStatus(FullyStepMigrated), status());
    }

    // rollback on previous migrate with --step
    {
        auto exitCode = invokeCommand(connection, MigrateRollback, {"--step=2"});

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    {
        auto exitCode = invokeTestStatusCommand(connection);

        QVERIFY(exitCode == EXIT_SUCCESS);
        QCOMPARE(createStatus({
            {Yes, s_2014_10_12_000000_create_posts_table,               s_1},
            {Yes, s_2014_10_12_100000_add_factor_column_to_posts_table, s_2},
            {No,  s_2014_10_12_200000_create_properties_table},
            {No,  s_2014_10_12_300000_create_phones_table},
        }), status());
    }
}

void tst_Migrate::refresh_OnMigrate() const
{
    QFETCH_GLOBAL(QString, connection);

    {
        auto exitCode = invokeCommand(connection, Migrate);

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    {
        auto exitCode = invokeTestStatusCommand(connection);

        QVERIFY(exitCode == EXIT_SUCCESS);
        QCOMPARE(createStatus(FullyMigrated), status());
    }

    // refresh on previous migrate w/o --step
    {
        auto exitCode = invokeCommand(connection, MigrateRefresh);

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    {
        auto exitCode = invokeTestStatusCommand(connection);

        QVERIFY(exitCode == EXIT_SUCCESS);
        QCOMPARE(createStatus(FullyMigrated), status());
    }
}

void tst_Migrate::refresh_OnMigrateWithStep() const
{
    QFETCH_GLOBAL(QString, connection);

    {
        auto exitCode = invokeCommand(connection, Migrate, {"--step"});

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    {
        auto exitCode = invokeTestStatusCommand(connection);

        QVERIFY(exitCode == EXIT_SUCCESS);
        QCOMPARE(createStatus(FullyStepMigrated), status());
    }

    // refresh on previous migrate with --step
    {
        auto exitCode = invokeCommand(connection, MigrateRefresh);

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    {
        auto exitCode = invokeTestStatusCommand(connection);

        QVERIFY(exitCode == EXIT_SUCCESS);
        QCOMPARE(createStatus(FullyMigrated), status());
    }
}

void tst_Migrate::refresh_Step() const
{
    QFETCH_GLOBAL(QString, connection);

    {
        auto exitCode = invokeCommand(connection, Migrate);

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    {
        auto exitCode = invokeTestStatusCommand(connection);

        QVERIFY(exitCode == EXIT_SUCCESS);
        QCOMPARE(createStatus(FullyMigrated), status());
    }

    // refresh on previous migrate w/o --step
    {
        auto exitCode = invokeCommand(connection, MigrateRefresh, {"--step=2"});

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    {
        auto exitCode = invokeTestStatusCommand(connection);

        QVERIFY(exitCode == EXIT_SUCCESS);
        QCOMPARE(createStatus({
            {Yes, s_2014_10_12_000000_create_posts_table,               s_1},
            {Yes, s_2014_10_12_100000_add_factor_column_to_posts_table, s_1},
            {Yes, s_2014_10_12_200000_create_properties_table,          s_2},
            {Yes, s_2014_10_12_300000_create_phones_table,              s_2},
        }), status());
    }
}

void tst_Migrate::refresh_StepMigrate() const
{
    QFETCH_GLOBAL(QString, connection);

    {
        auto exitCode = invokeCommand(connection, Migrate);

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    {
        auto exitCode = invokeTestStatusCommand(connection);

        QVERIFY(exitCode == EXIT_SUCCESS);
        QCOMPARE(createStatus(FullyMigrated), status());
    }

    // refresh on previous migrate w/o --step
    {
        auto exitCode = invokeCommand(connection, MigrateRefresh, {"--step-migrate"});

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    {
        auto exitCode = invokeTestStatusCommand(connection);

        QVERIFY(exitCode == EXIT_SUCCESS);
        QCOMPARE(createStatus(FullyStepMigrated), status());
    }
}

void tst_Migrate::refresh_Step_StepMigrate() const
{
    QFETCH_GLOBAL(QString, connection);

    {
        auto exitCode = invokeCommand(connection, Migrate);

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    {
        auto exitCode = invokeTestStatusCommand(connection);

        QVERIFY(exitCode == EXIT_SUCCESS);
        QCOMPARE(createStatus(FullyMigrated), status());
    }

    // refresh on previous migrate w/o --step
    {
        auto exitCode = invokeCommand(connection, MigrateRefresh,
                                      {"--step=2", "--step-migrate"});

        QVERIFY(exitCode == EXIT_SUCCESS);
    }

    {
        auto exitCode = invokeTestStatusCommand(connection);

        QVERIFY(exitCode == EXIT_SUCCESS);
        QCOMPARE(createStatus({
            {Yes, s_2014_10_12_000000_create_posts_table,               s_1},
            {Yes, s_2014_10_12_100000_add_factor_column_to_posts_table, s_1},
            {Yes, s_2014_10_12_200000_create_properties_table,          s_2},
            {Yes, s_2014_10_12_300000_create_phones_table,              s_3},
        }), status());
    }
}
// NOLINTEND(readability-convert-member-functions-to-static)

/* private */

int tst_Migrate::invokeCommand(const QString &connection, const QString &name,
                               std::vector<const char *> &&arguments)
{
    static const auto connectionTmpl = QStringLiteral("--database=%1");

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
    std::ranges::move(arguments, std::back_inserter(argv));

    int argc = static_cast<int>(argv.size());

    return runCommand(argc, argv);
}

int tst_Migrate::runCommand(int &argc, const std::vector<const char *> &argv)
{
    try {
        // env. should be always development so passed {} for env. name
        return TomApplication(argc, const_cast<char **>(argv.data()),
                              Databases::manager(), "TOM_TESTS_ENV", MigrationsTable)
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

void tst_Migrate::prepareDatabase(const QStringList &connections)
{
    for (const auto &connection : connections) {
        // Ownership of a unique_ptr()
        const auto schema = Databases::manager()->connection(connection)
                            .getSchemaBuilder();

        // Create the migrations table if needed
        if (!schema->hasTable(MigrationsTable)) {
            auto exitCode = invokeCommand(connection, MigrateInstall);

            QVERIFY(exitCode == EXIT_SUCCESS);

            return;
        }

        // Reset the migrations table
        auto exitCode = invokeCommand(connection, MigrateReset);

        QVERIFY(exitCode == EXIT_SUCCESS);
    }
}

QTEST_MAIN(tst_Migrate)

#include "tst_migrate.moc"
