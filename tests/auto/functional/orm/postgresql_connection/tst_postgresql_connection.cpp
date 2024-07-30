#include <QCoreApplication>
#include <QTest>

#include "orm/databasemanager.hpp"
#include "orm/postgresconnection.hpp"
#include "orm/utils/type.hpp"

#include "databases.hpp"
#include "macros.hpp"

using Orm::Constants::EMPTY;
using Orm::Constants::PUBLIC;
using Orm::Constants::search_path;
using Orm::Constants::username_;

using Orm::DatabaseManager;
using Orm::PostgresConnection;

using TypeUtils = Orm::Utils::Type;

using TestUtils::Databases;

/* The searchpath_Undefined_PostgreSQL() test method hardly depends on the following
   PostgreSQL configuration:

   search_path = "$user", public */

class tst_PostgreSQL_Connection : public QObject // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();

    /* Normal/real connection */
    void searchpath_Empty_PostgreSQL() const;
    void searchpath_Empty_SingleQuotes_PostgreSQL() const;
    void searchpath_Empty_DoubleQuotes_PostgreSQL() const;
    void searchpath_Undefined_PostgreSQL() const;

    void searchpath_WithUserVariable_Quoted_QString_PostgreSQL() const;
    void searchpath_WithUserVariable_Quoted_QStringList_PostgreSQL() const;
    void searchpath_WithUserVariable_UnQuoted_QString_PostgreSQL() const;
    void searchpath_WithUserVariable_UnQuoted_QStringList_PostgreSQL() const;

    /* Pretending */
    void searchpath_Pretend_Empty_PostgreSQL() const;
    void searchpath_Pretend_Empty_SingleQuotes_PostgreSQL() const;
    void searchpath_Pretend_Empty_DoubleQuotes_PostgreSQL() const;
    void searchpath_Pretend_Undefined_PostgreSQL() const;

    void searchpath_Pretend_WithUserVariable_Quoted_QString_PostgreSQL() const;
    void searchpath_Pretend_WithUserVariable_Quoted_QStringList_PostgreSQL() const;
    void searchpath_Pretend_WithUserVariable_UnQuoted_QString_PostgreSQL() const;
    void searchpath_Pretend_WithUserVariable_UnQuoted_QStringList_PostgreSQL() const;

// NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    /*! Test case class name. */
    inline static const auto *ClassName = "tst_PostgreSQL_Connection";

    /*! Connection name used in this test case. */
    QString m_connection;
    /*! The Database Manager used in this test case. */
    std::shared_ptr<DatabaseManager> m_dm;
};

/* private slots */

// NOLINTBEGIN(readability-convert-member-functions-to-static)
void tst_PostgreSQL_Connection::initTestCase()
{
    m_dm = Databases::createDatabaseManager();

    // Default connection must be empty
    QVERIFY(m_dm->getDefaultConnection().isEmpty());
}

/* Normal/real connection */

void tst_PostgreSQL_Connection::searchpath_Empty_PostgreSQL() const
{
    // Add a new database connection
    const auto connectionName = Databases::createConnectionTempFrom(
                                    Databases::POSTGRESQL,
                                    {ClassName, QString::fromUtf8(__func__)}, // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    {
        {search_path, EMPTY},
    });

    if (!connectionName)
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::POSTGRESQL)
              .toUtf8().constData(), );

    // Connection configuration
    QCOMPARE(m_dm->getConfigValue(search_path, *connectionName),
             QVariant {EMPTY});

    // Verify
    auto &connection = dynamic_cast<PostgresConnection &>(
                           m_dm->connection(*connectionName));

    QVERIFY(connection.searchPath().isEmpty());

    // Restore
    QVERIFY(Databases::removeConnection(*connectionName));
}

void tst_PostgreSQL_Connection::searchpath_Empty_SingleQuotes_PostgreSQL() const
{
    // Add a new database connection
    const auto connectionName = Databases::createConnectionTempFrom(
                                    Databases::POSTGRESQL,
                                    {ClassName, QString::fromUtf8(__func__)}, // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    {
        {search_path, sl("''")},
    });

    if (!connectionName)
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::POSTGRESQL)
              .toUtf8().constData(), );

    // Connection configuration
    QCOMPARE(m_dm->getConfigValue(search_path, *connectionName),
             QVariant {sl("''")});

    // Verify
    auto &connection = dynamic_cast<PostgresConnection &>(
                           m_dm->connection(*connectionName));

    QVERIFY(connection.searchPath().isEmpty());

    // Restore
    QVERIFY(Databases::removeConnection(*connectionName));
}

void tst_PostgreSQL_Connection::searchpath_Empty_DoubleQuotes_PostgreSQL() const
{
    // Add a new database connection
    const auto connectionName = Databases::createConnectionTempFrom(
                                    Databases::POSTGRESQL,
                                    {ClassName, QString::fromUtf8(__func__)}, // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    {
        {search_path, sl("\"\"")},
    });

    if (!connectionName)
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::POSTGRESQL)
              .toUtf8().constData(), );

    // Connection configuration
    QCOMPARE(m_dm->getConfigValue(search_path, *connectionName),
             QVariant {sl("\"\"")});

    // Verify
    auto &connection = dynamic_cast<PostgresConnection &>(
                           m_dm->connection(*connectionName));

    QVERIFY(connection.searchPath().isEmpty());

    // Restore
    QVERIFY(Databases::removeConnection(*connectionName));
}

void tst_PostgreSQL_Connection::searchpath_Undefined_PostgreSQL() const
{
    // Add a new database connection
    const auto connectionName = Databases::createConnectionTempFrom(
                                    Databases::POSTGRESQL,
                                    {ClassName, QString::fromUtf8(__func__)}, // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
                                    {}, {search_path});

    if (!connectionName)
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::POSTGRESQL)
              .toUtf8().constData(), );

    // Connection configuration
    QVERIFY(!m_dm->hasConfigValue(search_path, *connectionName));

    // Verify
    auto &connection = dynamic_cast<PostgresConnection &>(
                           m_dm->connection(*connectionName));

    const auto username = connection.getConfig(username_).value<QString>();
    QVERIFY(!username.isEmpty());

    QCOMPARE(connection.searchPath(),
             (QStringList {username, PUBLIC}));

    // Restore
    QVERIFY(Databases::removeConnection(*connectionName));
}

void tst_PostgreSQL_Connection::
     searchpath_WithUserVariable_Quoted_QString_PostgreSQL() const
{
    // Add a new database connection
    const auto connectionName = Databases::createConnectionTempFrom(
                                    Databases::POSTGRESQL,
                                    {ClassName, QString::fromUtf8(__func__)}, // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    {
        {search_path, sl("\"$user\", public")},
    });

    if (!connectionName)
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::POSTGRESQL)
              .toUtf8().constData(), );

    // Connection configuration
    QCOMPARE(m_dm->getConfigValue(search_path, *connectionName),
             (QVariant {QString {"\"$user\", public"}}));

    // Verify
    auto &connection = dynamic_cast<PostgresConnection &>(
                           m_dm->connection(*connectionName));

    const auto username = connection.getConfig(username_).value<QString>();
    QVERIFY(!username.isEmpty());

    QCOMPARE(connection.searchPath(),
             (QStringList {username, PUBLIC}));

    // Restore
    QVERIFY(Databases::removeConnection(*connectionName));
}

void tst_PostgreSQL_Connection::
     searchpath_WithUserVariable_Quoted_QStringList_PostgreSQL() const
{
    // Add a new database connection
    const auto connectionName = Databases::createConnectionTempFrom(
                                    Databases::POSTGRESQL,
                                    {ClassName, QString::fromUtf8(__func__)}, // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    {
        {search_path, QStringList {"\"$user\"", PUBLIC}},
    });

    if (!connectionName)
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::POSTGRESQL)
              .toUtf8().constData(), );

    // Connection configuration
    QCOMPARE(m_dm->getConfigValue(search_path, *connectionName),
             (QVariant {QStringList {"\"$user\"", PUBLIC}}));

    // Verify
    auto &connection = dynamic_cast<PostgresConnection &>(
                           m_dm->connection(*connectionName));

    const auto username = connection.getConfig(username_).value<QString>();
    QVERIFY(!username.isEmpty());

    QCOMPARE(connection.searchPath(),
             (QStringList {username, PUBLIC}));

    // Restore
    QVERIFY(Databases::removeConnection(*connectionName));
}

void tst_PostgreSQL_Connection::
     searchpath_WithUserVariable_UnQuoted_QString_PostgreSQL() const
{
    /* The PostgresConnector::quoteSearchPath() should correctly handle the unquoted
       $user variable. */

    // Add a new database connection
    const auto connectionName = Databases::createConnectionTempFrom(
                                    Databases::POSTGRESQL,
                                    {ClassName, QString::fromUtf8(__func__)}, // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    {
        {search_path, sl("$user, public")},
    });

    if (!connectionName)
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::POSTGRESQL)
              .toUtf8().constData(), );

    // Connection configuration
    QCOMPARE(m_dm->getConfigValue(search_path, *connectionName),
             (QVariant {QString {"$user, public"}}));

    // Verify
    auto &connection = dynamic_cast<PostgresConnection &>(
                           m_dm->connection(*connectionName));

    const auto username = connection.getConfig(username_).value<QString>();
    QVERIFY(!username.isEmpty());

    QCOMPARE(connection.searchPath(),
             (QStringList {username, PUBLIC}));

    // Restore
    QVERIFY(Databases::removeConnection(*connectionName));
}

void tst_PostgreSQL_Connection::
     searchpath_WithUserVariable_UnQuoted_QStringList_PostgreSQL() const
{
    /* The PostgresConnector::quoteSearchPath() should correctly handle the unquoted
       $user variable. */

    // Add a new database connection
    const auto connectionName = Databases::createConnectionTempFrom(
                                    Databases::POSTGRESQL,
                                    {ClassName, QString::fromUtf8(__func__)}, // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    {
        {search_path, QStringList {"$user", PUBLIC}},
    });

    if (!connectionName)
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::POSTGRESQL)
              .toUtf8().constData(), );

    // Connection configuration
    QCOMPARE(m_dm->getConfigValue(search_path, *connectionName),
             (QVariant {QStringList {"$user", PUBLIC}}));

    // Verify
    auto &connection = dynamic_cast<PostgresConnection &>(
                           m_dm->connection(*connectionName));

    const auto username = connection.getConfig(username_).value<QString>();
    QVERIFY(!username.isEmpty());

    QCOMPARE(connection.searchPath(),
             (QStringList {username, PUBLIC}));

    // Restore
    QVERIFY(Databases::removeConnection(*connectionName));
}

/* Pretending */

void tst_PostgreSQL_Connection::searchpath_Pretend_Empty_PostgreSQL() const
{
    // Add a new database connection
    const auto connectionName = Databases::createConnectionTempFrom(
                                    Databases::POSTGRESQL,
                                    {ClassName, QString::fromUtf8(__func__)}, // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    {
        {search_path, EMPTY},
    });

    if (!connectionName)
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::POSTGRESQL)
              .toUtf8().constData(), );

    // Connection configuration
    QCOMPARE(m_dm->getConfigValue(search_path, *connectionName),
             QVariant {EMPTY});

    // Verify
    auto &connection = dynamic_cast<PostgresConnection &>(
                           m_dm->connection(*connectionName));

    auto log = connection.pretend([&connection]
    {
        QVERIFY(connection.searchPath().isEmpty());
    });

    QVERIFY(log.isEmpty());

    // Restore
    QVERIFY(Databases::removeConnection(*connectionName));
}

void tst_PostgreSQL_Connection::searchpath_Pretend_Empty_SingleQuotes_PostgreSQL() const
{
    // Add a new database connection
    const auto connectionName = Databases::createConnectionTempFrom(
                                    Databases::POSTGRESQL,
                                    {ClassName, QString::fromUtf8(__func__)}, // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    {
        {search_path, sl("''")},
    });

    if (!connectionName)
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::POSTGRESQL)
              .toUtf8().constData(), );

    // Connection configuration
    QCOMPARE(m_dm->getConfigValue(search_path, *connectionName),
             QVariant {sl("''")});

    // Verify
    auto &connection = dynamic_cast<PostgresConnection &>(
                           m_dm->connection(*connectionName));

    auto log = connection.pretend([&connection]
    {
        QVERIFY(connection.searchPath().isEmpty());
    });

    QVERIFY(log.isEmpty());

    // Restore
    QVERIFY(Databases::removeConnection(*connectionName));
}

void tst_PostgreSQL_Connection::searchpath_Pretend_Empty_DoubleQuotes_PostgreSQL() const
{
    // Add a new database connection
    const auto connectionName = Databases::createConnectionTempFrom(
                                    Databases::POSTGRESQL,
                                    {ClassName, QString::fromUtf8(__func__)}, // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    {
        {search_path, sl("\"\"")},
    });

    if (!connectionName)
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::POSTGRESQL)
              .toUtf8().constData(), );

    // Connection configuration
    QCOMPARE(m_dm->getConfigValue(search_path, *connectionName),
             QVariant {sl("\"\"")});

    // Verify
    auto &connection = dynamic_cast<PostgresConnection &>(
                           m_dm->connection(*connectionName));

    auto log = connection.pretend([&connection]
    {
        QVERIFY(connection.searchPath().isEmpty());
    });

    QVERIFY(log.isEmpty());

    // Restore
    QVERIFY(Databases::removeConnection(*connectionName));
}

void tst_PostgreSQL_Connection::searchpath_Pretend_Undefined_PostgreSQL() const
{
    // Add a new database connection
    const auto connectionName = Databases::createConnectionTempFrom(
                                    Databases::POSTGRESQL,
                                    {ClassName, QString::fromUtf8(__func__)}, // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
                                    {}, {search_path});

    if (!connectionName)
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::POSTGRESQL)
              .toUtf8().constData(), );

    // Connection configuration
    QVERIFY(!m_dm->hasConfigValue(search_path, *connectionName));

    // Verify
    auto &connection = dynamic_cast<PostgresConnection &>(
                           m_dm->connection(*connectionName));

    auto log = connection.pretend([&connection]
    {
        QCOMPARE(connection.searchPath(), QStringList {PUBLIC});
    });

    QVERIFY(log.isEmpty());

    // Restore
    QVERIFY(Databases::removeConnection(*connectionName));
}

void tst_PostgreSQL_Connection::
     searchpath_Pretend_WithUserVariable_Quoted_QString_PostgreSQL() const
{
    // Add a new database connection
    const auto connectionName = Databases::createConnectionTempFrom(
                                    Databases::POSTGRESQL,
                                    {ClassName, QString::fromUtf8(__func__)}, // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    {
        {search_path, sl("\"$user\", public")},
    });

    if (!connectionName)
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::POSTGRESQL)
              .toUtf8().constData(), );

    // Connection configuration
    QCOMPARE(m_dm->getConfigValue(search_path, *connectionName),
             (QVariant {QString {"\"$user\", public"}}));

    // Verify
    auto &connection = dynamic_cast<PostgresConnection &>(
                           m_dm->connection(*connectionName));

    auto log = connection.pretend([&connection]
    {
        const auto username = connection.getConfig(username_).value<QString>();
        QVERIFY(!username.isEmpty());

        QCOMPARE(connection.searchPath(),
                 (QStringList {username, PUBLIC}));
    });

    QVERIFY(log.isEmpty());

    // Restore
    QVERIFY(Databases::removeConnection(*connectionName));
}

void tst_PostgreSQL_Connection::
     searchpath_Pretend_WithUserVariable_Quoted_QStringList_PostgreSQL() const
{
    /* The unquote $user variable must behave the same if pretending. */

    // Add a new database connection
    const auto connectionName = Databases::createConnectionTempFrom(
                                    Databases::POSTGRESQL,
                                    {ClassName, QString::fromUtf8(__func__)}, // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    {
        {search_path, QStringList {"\"$user\"", PUBLIC}},
    });

    if (!connectionName)
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::POSTGRESQL)
              .toUtf8().constData(), );

    // Connection configuration
    QCOMPARE(m_dm->getConfigValue(search_path, *connectionName),
             (QVariant {QStringList {"\"$user\"", PUBLIC}}));

    // Verify
    auto &connection = dynamic_cast<PostgresConnection &>(
                           m_dm->connection(*connectionName));

    auto log = connection.pretend([&connection]
    {
        const auto username = connection.getConfig(username_).value<QString>();
        QVERIFY(!username.isEmpty());

        QCOMPARE(connection.searchPath(),
                 (QStringList {username, PUBLIC}));
    });

    QVERIFY(log.isEmpty());

    // Restore
    QVERIFY(Databases::removeConnection(*connectionName));
}

void tst_PostgreSQL_Connection::
     searchpath_Pretend_WithUserVariable_UnQuoted_QString_PostgreSQL() const
{
    /* The unquote $user variable must behave the same if pretending. */

    // Add a new database connection
    const auto connectionName = Databases::createConnectionTempFrom(
                                    Databases::POSTGRESQL,
                                    {ClassName, QString::fromUtf8(__func__)}, // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    {
        {search_path, sl("$user, public")},
    });

    if (!connectionName)
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::POSTGRESQL)
              .toUtf8().constData(), );

    // Connection configuration
    QCOMPARE(m_dm->getConfigValue(search_path, *connectionName),
             (QVariant {QString {"$user, public"}}));

    // Verify
    auto &connection = dynamic_cast<PostgresConnection &>(
                           m_dm->connection(*connectionName));

    auto log = connection.pretend([&connection]
    {
        const auto username = connection.getConfig(username_).value<QString>();
        QVERIFY(!username.isEmpty());

        QCOMPARE(connection.searchPath(),
                 (QStringList {username, PUBLIC}));
    });

    QVERIFY(log.isEmpty());

    // Restore
    QVERIFY(Databases::removeConnection(*connectionName));
}

void tst_PostgreSQL_Connection::
     searchpath_Pretend_WithUserVariable_UnQuoted_QStringList_PostgreSQL() const
{
    // Add a new database connection
    const auto connectionName = Databases::createConnectionTempFrom(
                                    Databases::POSTGRESQL,
                                    {ClassName, QString::fromUtf8(__func__)}, // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    {
        {search_path, sl("$user, public")},
    });

    if (!connectionName)
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::POSTGRESQL)
              .toUtf8().constData(), );

    // Connection configuration
    QCOMPARE(m_dm->getConfigValue(search_path, *connectionName),
             (QVariant {QString {"$user, public"}}));

    // Verify
    auto &connection = dynamic_cast<PostgresConnection &>(
                           m_dm->connection(*connectionName));

    auto log = connection.pretend([&connection]
    {
        const auto username = connection.getConfig(username_).value<QString>();
        QVERIFY(!username.isEmpty());

        QCOMPARE(connection.searchPath(),
                 (QStringList {username, PUBLIC}));
    });

    QVERIFY(log.isEmpty());

    // Restore
    QVERIFY(Databases::removeConnection(*connectionName));
}
// NOLINTEND(readability-convert-member-functions-to-static)

QTEST_MAIN(tst_PostgreSQL_Connection)

#include "tst_postgresql_connection.moc"
