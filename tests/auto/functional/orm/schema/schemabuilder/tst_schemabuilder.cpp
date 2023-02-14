#include <QCoreApplication>
#include <QtTest>

#include <filesystem>
#include <unordered_set>

#include "orm/schema.hpp"
#include "orm/utils/query.hpp"
#include "orm/utils/type.hpp"

#include "databases.hpp"

namespace fs = std::filesystem;

using fspath = std::filesystem::path;

using Orm::Constants::EMPTY;
using Orm::Constants::ID;
using Orm::Constants::NAME;
using Orm::Constants::NOSPACE;
using Orm::Constants::QMYSQL;
using Orm::Constants::QPSQL;
using Orm::Constants::QSQLITE;
using Orm::Constants::database_;
using Orm::Constants::spatial_ref_sys;
using Orm::Constants::username_;

using Orm::Schema;
using Orm::SchemaNs::Blueprint;

using QueryUtils = Orm::Utils::Query;
using TypeUtils = Orm::Utils::Type;

using TestUtils::Databases;

class tst_SchemaBuilder : public QObject // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();

    void createDatabase_dropAllTables_dropDatabaseIfExists() const;
    void getAllTables() const;
    void getAllViews_dropAllViews() const;

    void getColumnListing() const;

    void hasTable() const;

// NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    /*! Database name used in tests. */
    inline static const auto DatabaseExample = QStringLiteral("tinyorm_example_database");

    /*! Get all database tables for the given connection. */
    static QSet<QString> getAllTablesFor(const QString &connection);
    /*! Get all database views for the given connection. */
    static QSet<QString> getAllViewsFor(const QString &connection);

    /*! Determine whether a given database exists (router method). */
    bool hasDatabase(const QString &database, const QString &connection) const;
    /*! Determine whether a given database exists (MySQL). */
    bool hasDatabase_MySql(const QString &database, const QString &connection) const;
    /*! Determine whether a given database exists (PostgreSQL). */
    bool hasDatabase_Postgres(const QString &database, const QString &connection) const;
    /*! Determine whether a given database exists (SQLite). */
    static bool hasDatabase_Sqlite(const fspath &database);

    /*! Create a new alternative connection on the DatabaseExample database. */
    QString alternativeConnection(const QString &connection) const;
    /*! Create a new alternative connection on the DatabaseExample database (MySQL). */
    QString alternativeConnection_MySql() const;
    /*! Create a new alternative connection on the DatabaseExample database
        (PostgreSQL). */
    QString alternativeConnection_Postgres() const;
    /*! Create a new alternative connection on the DatabaseExample database (SQLite). */
    QString alternativeConnection_Sqlite() const;

    /*! The Database Manager instance in the TinyUtils. */
    std::shared_ptr<Orm::DatabaseManager> m_dm;
};

/* private slots */

// NOLINTBEGIN(readability-convert-member-functions-to-static)
void tst_SchemaBuilder::initTestCase()
{
    const auto connections = Databases::createConnections();

    if (connections.isEmpty())
        QSKIP(TestUtils::AutoTestSkippedAny.arg(TypeUtils::classPureBasename(*this))
                                           .toUtf8().constData(), );

    QTest::addColumn<QString>("connection");

    // Run all tests for all supported database connections
    for (const auto &connection : connections)
        QTest::newRow(connection.toUtf8().constData()) << connection;

    m_dm = Databases::manager();
}

void tst_SchemaBuilder::createDatabase_dropAllTables_dropDatabaseIfExists() const
{
    QFETCH_GLOBAL(QString, connection);

    // createDatabase()
    {
        auto database = DatabaseExample;
        if (m_dm->driverName(connection) == QSQLITE)
            database.append(QStringLiteral(".sqlite3"));

        QVERIFY(!hasDatabase(database, connection));

        Schema::on(connection).createDatabase(database);

        QVERIFY(hasDatabase(database, connection));
    }

    // dropAllTables()
    {
        const auto alternativeConnection = this->alternativeConnection(connection);

        const QSet<QString> tables {QStringLiteral("tinyorm_test_table_to_drop_1"),
                                    QStringLiteral("tinyorm_test_table_to_drop_2")};

        // Create new tables in another database
        for (const auto &table : tables)
            Schema::on(alternativeConnection)
                    .create(table, [](Blueprint &table)
            {
                table.id();
                table.string(NAME);
            });

        // Verify created tables
        {
            const auto tablesActual = getAllTablesFor(alternativeConnection);
            // Nicer name
            const auto &tablesExpected = tables;

            QCOMPARE(tablesActual, tablesExpected);
        }

        // Fire it up
        Schema::on(alternativeConnection).dropAllTables();

        // Verify dropAllTables()
        {
            const auto tablesActual = getAllTablesFor(alternativeConnection);

            QVERIFY(tablesActual.isEmpty());
        }

        // Restore
        QVERIFY(m_dm->removeConnection(alternativeConnection));
    }

    // dropDatabaseIfExists()
    {
        auto database = DatabaseExample;
        if (m_dm->driverName(connection) == QSQLITE)
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

    QSet<QString> tablesExpected {
        "datetimes", "file_property_properties", "migrations", "roles", "role_user",
        "settings", "tag_properties", "tag_torrent", "torrents", "torrent_peers",
        "torrent_previewable_files", "torrent_previewable_file_properties",
        "torrent_tags", "types", "users", "user_phones"
    };

    if (m_dm->driverName(connection) == QPSQL)
        tablesExpected << spatial_ref_sys;

    QCOMPARE(tablesActual, tablesExpected);
}

void tst_SchemaBuilder::getAllViews_dropAllViews() const
{
    QFETCH_GLOBAL(QString, connection);

    const auto isPostgres = m_dm->driverName(connection) == QPSQL;

    // The PostgreSQL database also contains the postgis views
    const QSet<QString> viewsPostgis {QStringLiteral("geography_columns"),
                                      QStringLiteral("geometry_columns")};

    // Verify views before creating new views
    {
        const auto viewsActual = getAllViewsFor(connection);

        /* PostgreSQL always contains the postgis views because the postgis extension
           is installed and also required by some tests. */
        if (isPostgres)
            QCOMPARE(viewsActual, viewsPostgis);

        // No views can't exist at the beginning
        else
            QVERIFY(viewsActual.isEmpty());
    }

    // View names to create and drop
    QSet<QString> views {QStringLiteral("tinyorm_test_view_to_drop_1"),
                         QStringLiteral("tinyorm_test_view_to_drop_2")};

    // Create new views to drop
    for (const auto &view : views)
        m_dm->on(connection)
                .unprepared(QStringLiteral(
                                "create view %1 as select id, name from users")
                            .arg(view));

    // Verify newly created views
    {
        const auto viewsActual = getAllViewsFor(connection);

        // Nicer name (last usage so can be also modified to avoid a copy)
        auto &viewsExpected = views;
        if (isPostgres)
            viewsExpected += viewsPostgis;

        // Fire it up
        QCOMPARE(viewsActual, viewsExpected);
    }

    // Fire it up
    Schema::on(connection).dropAllViews();

    // Verify dropAllViews()
    {
        const auto viewsActual = getAllViewsFor(connection);

        if (isPostgres)
            QCOMPARE(viewsActual, viewsPostgis);
        else
            QVERIFY(viewsActual.isEmpty());
    }
}

void tst_SchemaBuilder::getColumnListing() const
{
    QFETCH_GLOBAL(QString, connection);

    auto columnsList = Schema::on(connection).getColumnListing("roles");

    // Move to the unordered_set as returned column names are not ordered
    std::unordered_set<QString> columns;
    columns.reserve(columnsList.size());
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

bool tst_SchemaBuilder::hasDatabase(const QString &database,
                                    const QString &connection) const
{
    const auto driver = m_dm->driverName(connection);

    if (driver == QMYSQL)
        return hasDatabase_MySql(database, connection);

    if (driver == QPSQL)
        return hasDatabase_Postgres(database, connection);

    if (driver == QSQLITE)
        return hasDatabase_Sqlite(database.toUtf8().constData());

    Q_UNREACHABLE();
}

bool tst_SchemaBuilder::hasDatabase_MySql(const QString &database,
                                          const QString &connection) const
{
    auto query = m_dm->on(connection)
                 .select(QStringLiteral("show databases"),
                         {m_dm->getConfigValue(username_, connection)});

    while (query.next())
        if (query.value(QStringLiteral("Database")).value<QString>() == database)
            return true;

    return false;
}

bool tst_SchemaBuilder::hasDatabase_Postgres(const QString &database,
                                             const QString &connection) const
{
    auto query = m_dm->on(connection)
                 .select(
                     QStringLiteral(
                         "select datname from pg_database "
                         "join pg_user on pg_database.datdba = pg_user.usesysid "
                         "where datdba = "
                           "(select usesysid from pg_user where usename = ?)"),
                     {m_dm->getConfigValue(username_, connection)});

    while (query.next())
        if (query.value(QStringLiteral("datname")).value<QString>() == database)
            return true;

    return false;
}

bool tst_SchemaBuilder::hasDatabase_Sqlite(const fspath &database)
{
    return fs::exists(database);
}

QString tst_SchemaBuilder::alternativeConnection(const QString &connection) const
{
    const auto driver = m_dm->driverName(connection);

    if (driver == QMYSQL)
        return alternativeConnection_MySql();

    if (driver == QPSQL)
        return alternativeConnection_Postgres();

    if (driver == QSQLITE)
        return alternativeConnection_Sqlite();

    Q_UNREACHABLE();
}

QString tst_SchemaBuilder::alternativeConnection_MySql() const
{
    auto connectionName =
            QStringLiteral(
                "tinyorm_mysql_tests-tst_SchemaBuilder-alternativeConnection");

    // Make configuration copy so I can modify it
    auto configuration = Databases::configuration(Databases::MYSQL)->get();

    // Prepare/modify configuration
    configuration[database_] = DatabaseExample;

    // Add a new database connection
    m_dm->addConnections({
        {connectionName, configuration},
    // Don't setup any default connection
    }, EMPTY);

    return connectionName;
}

QString tst_SchemaBuilder::alternativeConnection_Postgres() const
{
     auto connectionName =
            QStringLiteral(
                "tinyorm_pgsql_tests-tst_SchemaBuilder-alternativeConnection");

    // Make configuration copy so I can modify it
    auto configuration = Databases::configuration(Databases::POSTGRESQL)->get();

    // Prepare/modify configuration
    configuration[database_] = DatabaseExample;

    // Add a new database connection
    m_dm->addConnections({
        {connectionName, configuration},
    // Don't setup any default connection
    }, EMPTY);

    return connectionName;
}

QString tst_SchemaBuilder::alternativeConnection_Sqlite() const
{
    auto connectionName =
            QStringLiteral(
                "tinyorm_sqlite_tests-tst_SchemaBuilder-alternativeConnection");

    // Make configuration copy so I can modify it
    auto configuration = Databases::configuration(Databases::SQLITE)->get();

    // Prepare/modify configuration
    configuration[database_] = NOSPACE.arg(DatabaseExample, ".sqlite3");

    // Add a new database connection
    m_dm->addConnections({
        {connectionName, configuration},
    // Don't setup any default connection
    }, EMPTY);

    return connectionName;
}

QTEST_MAIN(tst_SchemaBuilder)

#include "tst_schemabuilder.moc"
