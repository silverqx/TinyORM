#include <QCoreApplication>
#include <QtTest>

#include <filesystem>
#include <unordered_set>

#include "orm/db.hpp"
#include "orm/schema.hpp"
#include "orm/utils/query.hpp"
#include "orm/utils/type.hpp"

#include "databases.hpp"

namespace fs = std::filesystem;

using fspath = std::filesystem::path;

using Orm::Constants::ID;
using Orm::Constants::NAME;
using Orm::Constants::NOSPACE;
using Orm::Constants::QMYSQL;
using Orm::Constants::QPSQL;
using Orm::Constants::QSQLITE;
using Orm::Constants::database_;
using Orm::Constants::username_;

using Orm::DB;
using Orm::Schema;
using Orm::SchemaNs::Blueprint;

using QueryUtils = Orm::Utils::Query;
using TypeUtils = Orm::Utils::Type;

using TestUtils::Databases;

class tst_SchemaBuilder : public QObject // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase() const;

    void createDatabase_dropAllTables_dropDatabaseIfExists() const;
    void getAllTables() const;
    void getAllViews_dropAllViews() const;

    void getColumnListing() const;

    void hasTable() const;

// NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    /*! Test case class name. */
    inline static const auto *ClassName = "tst_SchemaBuilder";
    /*! Database name used in the current test case. */
    inline static const auto DatabaseName = QStringLiteral("tinyorm_tests_schemabuilder");

    /*! Get all database tables for the given connection. */
    static QSet<QString> getAllTablesFor(const QString &connection);
    /*! Get all database views for the given connection. */
    static QSet<QString> getAllViewsFor(const QString &connection);

    /*! Determine whether a given database exists (router method). */
    static bool hasDatabase(const QString &database, const QString &connection);
    /*! Determine whether a given database exists (MySQL). */
    static bool hasDatabase_MySql(const QString &database, const QString &connection);
    /*! Determine whether a given database exists (PostgreSQL). */
    static bool hasDatabase_Postgres(const QString &database, const QString &connection);
    /*! Determine whether a given database exists (SQLite). */
    static bool hasDatabase_Sqlite(const fspath &database);

    /*! Create a new alternative connection on the DatabaseExample database. */
    static std::optional<QString> alternativeConnection(const QString &connection);
    /*! Create a new alternative connection on the DatabaseExample database (MySQL). */
    static std::optional<QString> alternativeConnection_MySql();
    /*! Create a new alternative connection on the DatabaseExample database
        (PostgreSQL). */
    static std::optional<QString> alternativeConnection_Postgres();
    /*! Create a new alternative connection on the DatabaseExample database (SQLite). */
    static std::optional<QString> alternativeConnection_Sqlite();
};

/* private slots */

// NOLINTBEGIN(readability-convert-member-functions-to-static)
void tst_SchemaBuilder::initTestCase() const
{
    const auto connections = Databases::createConnections();

    if (connections.isEmpty())
        QSKIP(TestUtils::AutoTestSkippedAny.arg(TypeUtils::classPureBasename(*this))
                                           .toUtf8().constData(), );

    QTest::addColumn<QString>("connection");

    // Run all tests for all supported database connections
    for (const auto &connection : connections)
        QTest::newRow(connection.toUtf8().constData()) << connection;
}

void tst_SchemaBuilder::createDatabase_dropAllTables_dropDatabaseIfExists() const
{
    QFETCH_GLOBAL(QString, connection);

    // createDatabase()
    {
        auto database = DatabaseName;
        if (DB::driverName(connection) == QSQLITE)
            database.append(QStringLiteral(".sqlite3"));

        QVERIFY(!hasDatabase(database, connection));

        Schema::on(connection).createDatabase(database);

        QVERIFY(hasDatabase(database, connection));
    }

    /* Create an alternative connection for connecting to another database to test
       getAllTable() and dropAllTables(). */
    const auto alternativeConnection =
            tst_SchemaBuilder::alternativeConnection(connection);

    // dropAllTables()
    {
        const QSet<QString> tables {QStringLiteral("tinyorm_test_table_to_drop_1"),
                                    QStringLiteral("tinyorm_test_table_to_drop_2")};

        // Create new tables in another database
        for (const auto &table : tables)
            Schema::on(*alternativeConnection)
                    .create(table, [](Blueprint &table_)
            {
                table_.id();
                table_.string(NAME);
            });

        // Verify created tables
        {
            const auto tablesActual = getAllTablesFor(*alternativeConnection);
            // Nicer name
            const auto &tablesExpected = tables;

            QCOMPARE(tablesActual, tablesExpected);
        }

        // Fire it up
        Schema::on(*alternativeConnection).dropAllTables();

        // Verify dropAllTables()
        {
            const auto tablesActual = getAllTablesFor(*alternativeConnection);

            QVERIFY(tablesActual.isEmpty());
        }

        // Restore
        QVERIFY(Databases::removeConnection(*alternativeConnection));
    }

    // dropDatabaseIfExists()
    {
        auto database = DatabaseName;
        if (DB::driverName(connection) == QSQLITE)
            database.append(QStringLiteral(".sqlite3"));

        QVERIFY(hasDatabase(database, connection));

        Schema::on(connection).dropDatabaseIfExists(database);

        QVERIFY(!hasDatabase(database, connection));
    }
}

void tst_SchemaBuilder::getAllTables() const
{
    QFETCH_GLOBAL(QString, connection);

    const auto tablesActual = getAllTablesFor(connection);

    const QSet<QString> tablesExpected {
        "datetimes", "file_property_properties", "migrations", "roles", "role_user",
        "settings", "tag_properties", "tag_torrent", "torrents", "torrent_peers",
        "torrent_previewable_files", "torrent_previewable_file_properties",
        "torrent_tags", "types", "users", "user_phones"
    };

    QCOMPARE(tablesActual, tablesExpected);
}

void tst_SchemaBuilder::getAllViews_dropAllViews() const
{
    QFETCH_GLOBAL(QString, connection);

    // Verify views before creating new views, no views can't exist at the beginning
    QVERIFY(getAllViewsFor(connection).isEmpty());

    // View names to create and drop
    QSet<QString> views {QStringLiteral("tinyorm_test_view_to_drop_1"),
                         QStringLiteral("tinyorm_test_view_to_drop_2")};

    // Create new views to drop
    for (const auto &view : views)
        DB::on(connection)
                .unprepared(QStringLiteral(
                                "create view %1 as select id, name from users")
                            .arg(view));

    // Verify newly created views
    {
        const auto viewsActual = getAllViewsFor(connection);
        // Nicer name (last usage so can be also modified to avoid a copy)
        const auto &viewsExpected = views;

        // Fire it up
        QCOMPARE(viewsActual, viewsExpected);
    }

    // Fire it up
    Schema::on(connection).dropAllViews();

    // Verify dropAllViews()
    QVERIFY(getAllViewsFor(connection).isEmpty());
}

void tst_SchemaBuilder::getColumnListing() const
{
    QFETCH_GLOBAL(QString, connection);

    auto columnsList = Schema::on(connection).getColumnListing("roles");

    // Move to the unordered_set as returned column names are not ordered
    std::unordered_set<QString> columns;
    columns.reserve(static_cast<std::unordered_set<QString>::size_type>(
                        columnsList.size()));
    std::ranges::move(columnsList, std::inserter(columns, columns.end()));

    QCOMPARE(columns,
             std::unordered_set<QString>({ID, NAME, QStringLiteral("added_on")}));
}

void tst_SchemaBuilder::hasTable() const
{
    QFETCH_GLOBAL(QString, connection);

    QVERIFY(Schema::on(connection).hasTable("users"));
}
// NOLINTEND(readability-convert-member-functions-to-static)

/* private */

QSet<QString> tst_SchemaBuilder::getAllTablesFor(const QString &connection)
{
    auto query = Schema::on(connection).getAllTables();

    QSet<QString> tablesActual;
    tablesActual.reserve(QueryUtils::queryResultSize(query));

    while (query.next())
        tablesActual << query.value(0).value<QString>();

    return tablesActual;
}

QSet<QString> tst_SchemaBuilder::getAllViewsFor(const QString &connection)
{
    auto query = Schema::on(connection).getAllViews();

    QSet<QString> views;
    views.reserve(QueryUtils::queryResultSize(query));

    while (query.next())
        views << query.value(0).value<QString>();

    return views;
}

bool tst_SchemaBuilder::hasDatabase(const QString &database, const QString &connection)
{
    const auto driver = DB::driverName(connection);

    if (driver == QMYSQL)
        return hasDatabase_MySql(database, connection);

    if (driver == QPSQL)
        return hasDatabase_Postgres(database, connection);

    if (driver == QSQLITE)
        return hasDatabase_Sqlite(database.toUtf8().constData());

    Q_UNREACHABLE();
}

bool tst_SchemaBuilder::hasDatabase_MySql(const QString &database,
                                          const QString &connection)
{
    auto query = DB::on(connection)
                 .select(QStringLiteral("show databases"),
                         {DB::getConfigValue(username_, connection)});

    while (query.next())
        if (query.value(QStringLiteral("Database")).value<QString>() == database)
            return true;

    return false;
}

bool tst_SchemaBuilder::hasDatabase_Postgres(const QString &database,
                                             const QString &connection)
{
    auto query = DB::on(connection)
                 .select(
                     QStringLiteral(
                         "select datname from pg_database "
                         "join pg_user on pg_database.datdba = pg_user.usesysid "
                         "where datdba = "
                           "(select usesysid from pg_user where usename = ?)"),
                     {DB::getConfigValue(username_, connection)});

    while (query.next())
        if (query.value(QStringLiteral("datname")).value<QString>() == database)
            return true;

    return false;
}

bool tst_SchemaBuilder::hasDatabase_Sqlite(const fspath &database)
{
    return fs::exists(database);
}

std::optional<QString>
tst_SchemaBuilder::alternativeConnection(const QString &connection)
{
    const auto driver = DB::driverName(connection);

    if (driver == QMYSQL)
        return alternativeConnection_MySql();

    if (driver == QPSQL)
        return alternativeConnection_Postgres();

    if (driver == QSQLITE)
        return alternativeConnection_Sqlite();

    Q_UNREACHABLE();
}

std::optional<QString> tst_SchemaBuilder::alternativeConnection_MySql()
{
    // Add a new database connection
    auto connectionName =
            Databases::createConnectionTempFrom(
                Databases::MYSQL, {ClassName, QString::fromUtf8(__func__)}, // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
                {{database_, DatabaseName}});

    return connectionName;
}

std::optional<QString> tst_SchemaBuilder::alternativeConnection_Postgres()
{
    // Add a new database connection
    auto connectionName =
            Databases::createConnectionTempFrom(
                Databases::POSTGRESQL, {ClassName, QString::fromUtf8(__func__)}, // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
                {{database_, DatabaseName}});

    return connectionName;
}

std::optional<QString> tst_SchemaBuilder::alternativeConnection_Sqlite()
{
    // Add a new database connection
    auto connectionName =
            Databases::createConnectionTempFrom(
                Databases::SQLITE, {ClassName, QString::fromUtf8(__func__)}, // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
                {{database_, NOSPACE.arg(DatabaseName, ".sqlite3")}});

    return connectionName;
}

QTEST_MAIN(tst_SchemaBuilder)

#include "tst_schemabuilder.moc"
