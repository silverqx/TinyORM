#include <QCoreApplication>
#include <QTest>

#include <filesystem>

#include "orm/db.hpp"
#include "orm/postgresconnection.hpp"
#include "orm/schema.hpp"
#include "orm/utils/type.hpp"

#include "databases.hpp"
#include "macros.hpp"

namespace fs = std::filesystem;

using fspath = std::filesystem::path;

using Orm::Constants::ID;
using Orm::Constants::NAME;
using Orm::Constants::NOSPACE;
using Orm::Constants::PUBLIC;
using Orm::Constants::QMYSQL;
using Orm::Constants::QPSQL;
using Orm::Constants::QSQLITE;
using Orm::Constants::database_;
using Orm::Constants::username_;

using Orm::DB;
using Orm::PostgresConnection;
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
    void cleanupTestCase() const;

    /* SchemaBuilder related */
    void createDatabase_dropAllTables_dropDatabaseIfExists() const;
    void getAllTables() const;
    void getAllViews_dropAllViews() const;

    void getColumnListing() const;

    void hasTable() const;

    /* Blueprint commands */
    void createTable_WithComment() const;
    void modifyTable_WithComment() const;

// NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    /*! Table or database name used in tests. */
    inline static const auto Firewalls = sl("firewalls");
    /*! Test case class name. */
    inline static const auto *ClassName = "tst_SchemaBuilder";
    /*! Database name used in the current test case. */
    inline static const auto DatabaseName = sl("tinyorm_tests_schemabuilder");

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

    /*! Get a table comment (router method). */
    static QVariant getTableComment(const QString &table, const QString &connection);
    /*! Get a table comment (MySQL). */
    static QVariant getTableComment_MySql(const QString &table,
                                          const QString &connection);
    /*! Get a table comment (PostgreSQL). */
    static QVariant getTableComment_Postgres(const QString &table,
                                             const QString &connection);

    /*! Create a new alternative connection on the DatabaseExample database. */
    static std::optional<QString> alternativeConnection(const QString &connection);
    /*! Create a new alternative connection on the DatabaseExample database (MySQL). */
    static std::optional<QString> alternativeConnection_MySql();
    /*! Create a new alternative connection on the DatabaseExample database (MariaDB). */
    static std::optional<QString> alternativeConnection_Maria();
    /*! Create a new alternative connection on the DatabaseExample database
        (PostgreSQL). */
    static std::optional<QString> alternativeConnection_Postgres();
    /*! Create a new alternative connection on the DatabaseExample database (SQLite). */
    static std::optional<QString> alternativeConnection_Sqlite();

    /*! Created database connections (needed by the cleanupTestCase()). */
    QStringList m_connections;
};

/* private slots */

// NOLINTBEGIN(readability-convert-member-functions-to-static)
void tst_SchemaBuilder::initTestCase()
{
    m_connections = Databases::createConnections();

    if (m_connections.isEmpty())
        QSKIP(TestUtils::AutoTestSkippedAny.arg(TypeUtils::classPureBasename(*this))
                                           .toUtf8().constData(), );

    QTest::addColumn<QString>("connection");

    // Run all tests for all supported database connections
    for (const auto &connection : std::as_const(m_connections))
        QTest::newRow(connection.toUtf8().constData()) << connection;
}

void tst_SchemaBuilder::cleanupTestCase() const
{
    // dropDatabaseIfExists()
    for (const auto &connection : m_connections) {
        auto database = DatabaseName;
        if (DB::driverName(connection) == QSQLITE)
            database.append(sl(".sqlite3"));

        Schema::on(connection).dropDatabaseIfExists(database);

        QVERIFY(!hasDatabase(database, connection));
    }
}

/* SchemaBuilder related */

void tst_SchemaBuilder::createDatabase_dropAllTables_dropDatabaseIfExists() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    // createDatabase()
    {
        auto database = DatabaseName;
        if (DB::driverName(connection) == QSQLITE)
            database.append(sl(".sqlite3"));

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
        const QSet<QString> tables {sl("tinyorm_test_table_to_drop_1"),
                                    sl("tinyorm_test_table_to_drop_2")};

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
            database.append(sl(".sqlite3"));

        QVERIFY(hasDatabase(database, connection));

        Schema::on(connection).dropDatabaseIfExists(database);

        QVERIFY(!hasDatabase(database, connection));
    }
}

void tst_SchemaBuilder::getAllTables() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    const auto tablesActual = getAllTablesFor(connection);

    QSet<QString> tablesExpected {
        "albums", "album_images", "datetimes", "empty_with_default_values",
        "file_property_properties", "migrations", "roles", "role_tag", "role_user",
        "settings", "state_torrent", "tag_properties", "tag_torrent", "torrents",
        "torrent_peers", "torrent_previewable_files",
        "torrent_previewable_file_properties", "torrent_states", "torrent_tags", "types",
        "users", "user_phones",
    };

    // Avoid to failing if the tom migrations are migrated
    const std::initializer_list<QString> il {
        "migrations_example", "phones", "posts", "properties",
    };
    if (tablesActual.contains(il))
        tablesExpected += il;

    QCOMPARE(tablesActual, tablesExpected);
}

void tst_SchemaBuilder::getAllViews_dropAllViews() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    // Verify views before creating new views, no views can't exist at the beginning
    QVERIFY(getAllViewsFor(connection).isEmpty());

    // View names to create and drop
    QSet<QString> views {sl("tinyorm_test_view_to_drop_1"),
                         sl("tinyorm_test_view_to_drop_2")};

    // Create new views to drop
    for (const auto &view : views)
        DB::on(connection).unprepared(sl("create view %1 as select id, name from users")
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
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    auto columnsList = Schema::on(connection).getColumnListing("roles");

    // Move to the unordered_set as returned column names are not ordered
    std::unordered_set<QString> columns;
    columns.reserve(static_cast<decltype (columns)::size_type>(columnsList.size()));
    std::ranges::move(columnsList, std::inserter(columns, columns.end()));

    QCOMPARE(columns,
             std::unordered_set<QString>({ID, NAME, sl("added_on")}));
}

void tst_SchemaBuilder::hasTable() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    QVERIFY(Schema::on(connection).hasTable("users"));
}

/* Blueprint commands */

void tst_SchemaBuilder::createTable_WithComment() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    if (DB::driverName(connection) == QSQLITE)
        QSKIP("The SQLite database doesn't support table comments.", );

    QVERIFY(!Schema::on(connection).hasTable(Firewalls));

    // Create table with comment
    const auto tableComment = sl("Example 'table' comment");

    Schema::on(connection)
            .create(Firewalls, [&tableComment](Blueprint &table)
    {
        table.comment(tableComment);

        table.id();
        table.string(NAME);
    });

    QVERIFY(Schema::on(connection).hasTable(Firewalls));

    // Verify
    QCOMPARE(getTableComment(Firewalls, connection), QVariant(tableComment));

    // Restore
    Schema::drop(Firewalls, connection);

    QVERIFY(!Schema::on(connection).hasTable(Firewalls));
}

void tst_SchemaBuilder::modifyTable_WithComment() const
{
    QFETCH_GLOBAL(QString, connection); // NOLINT(modernize-type-traits)

    if (DB::driverName(connection) == QSQLITE)
        QSKIP("The SQLite database doesn't support table comments.", );

    QVERIFY(!Schema::on(connection).hasTable(Firewalls));

    // Create table with comment
    {
        const auto tableComment = sl("Example 'table' comment");

        Schema::on(connection)
                .create(Firewalls, [&tableComment](Blueprint &table)
        {
            table.comment(tableComment);

            table.id();
            table.string(NAME);
        });

        QVERIFY(Schema::on(connection).hasTable(Firewalls));

        // Verify
        QCOMPARE(getTableComment(Firewalls, connection), QVariant(tableComment));
    }

    // Modify the table comment
    {
        const auto tableComment = sl("Example modified 'table' comment");

        Schema::on(connection)
                .table(Firewalls, [&tableComment](Blueprint &table)
        {
            table.comment(tableComment);
        });

        QVERIFY(Schema::on(connection).hasTable(Firewalls));

        // Verify
        QCOMPARE(getTableComment(Firewalls, connection), QVariant(tableComment));
    }

    // Restore
    Schema::drop(Firewalls, connection);

    QVERIFY(!Schema::on(connection).hasTable(Firewalls));
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

    // Is the same for the MySQL and MariaDB
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
    auto query = DB::on(connection).select(sl("show databases"));

    while (query.next())
        if (query.value(sl("Database")).value<QString>() == database)
            return true;

    return false;
}

bool tst_SchemaBuilder::hasDatabase_Postgres(const QString &database,
                                             const QString &connection)
{
    auto query = DB::on(connection)
                 .select(sl("select datname from pg_database "
                            "join pg_user on pg_database.datdba = pg_user.usesysid "
                            "where datdba = "
                              "(select usesysid from pg_user where usename = ?)"),
                     {DB::getConfigValue(username_, connection)});

    while (query.next())
        if (query.value(sl("datname")).value<QString>() == database)
            return true;

    return false;
}

bool tst_SchemaBuilder::hasDatabase_Sqlite(const fspath &database)
{
    return fs::exists(database);
}

QVariant tst_SchemaBuilder::getTableComment(const QString &table,
                                            const QString &connection)
{
    const auto driver = DB::driverName(connection);

    // Is the same for the MySQL and MariaDB
    if (driver == QMYSQL)
        return getTableComment_MySql(table, connection);

    if (driver == QPSQL)
        return getTableComment_Postgres(table, connection);

    Q_UNREACHABLE();
}

QVariant tst_SchemaBuilder::getTableComment_MySql(const QString &table,
                                                  const QString &connection)
{
    return DB::table("information_schema.tables", connection)
            ->whereEq("table_schema",
                      DB::getConfigValue(database_, connection).value<QString>())
            .whereEq("table_name", table)
            .select("table_comment")
            .value("table_comment");
}

QVariant tst_SchemaBuilder::getTableComment_Postgres(const QString &table,
                                                     const QString &connection)
{
    // This is useless as schema will always be 'public', but the logic is right
    const auto schema = dynamic_cast<PostgresConnection &>(DB::connection(connection))
                        .searchPath().constFirst();
    Q_ASSERT(schema == PUBLIC);

    auto query = DB::on(connection).unprepared(
                     sl("select obj_description('%1.%2'::regclass) as table_comment")
                     .arg(schema, table));

    [[maybe_unused]]
    const auto ok = query.first();
    Q_ASSERT(ok);
    Q_ASSERT(query.isValid());

    return query.value(sl("table_comment"));
}

std::optional<QString>
tst_SchemaBuilder::alternativeConnection(const QString &connection)
{
    const auto driver = DB::driverName(connection);

    if (driver == QMYSQL) {
        if (connection == Databases::MYSQL)
            return alternativeConnection_MySql();

        if (connection == Databases::MARIADB)
            return alternativeConnection_Maria();

        Q_UNREACHABLE();
    }

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

std::optional<QString> tst_SchemaBuilder::alternativeConnection_Maria()
{
    // Add a new database connection
    auto connectionName =
            Databases::createConnectionTempFrom(
                Databases::MARIADB, {ClassName, QString::fromUtf8(__func__)}, // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
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
