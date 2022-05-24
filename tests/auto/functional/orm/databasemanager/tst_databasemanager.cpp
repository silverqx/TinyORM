#include <QCoreApplication>
#include <QtTest>

#include "orm/constants.hpp"
#include "orm/databasemanager.hpp"

#include "databases.hpp"

using Orm::Constants::charset_;
using Orm::Constants::database_;
using Orm::Constants::driver_;
using Orm::Constants::H127001;
using Orm::Constants::host_;
using Orm::Constants::P5432;
using Orm::Constants::password_;
using Orm::Constants::port_;
using Orm::Constants::PUBLIC;
using Orm::Constants::QPSQL;
using Orm::Constants::schema_;
using Orm::Constants::username_;
using Orm::Constants::UTF8;

using Orm::DatabaseManager;
using Orm::Support::DatabaseConfiguration;

class tst_DatabaseManager : public QObject // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

private slots:
    void initTestCase();

    void removeConnection_Connected() const;
    void removeConnection_NotConnected() const;

// NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    /*! The Database Manager used in this test case. */
    std::shared_ptr<DatabaseManager> m_dm {};
};

void tst_DatabaseManager::initTestCase()
{
    m_dm = DatabaseManager::create();
}

void tst_DatabaseManager::removeConnection_Connected() const
{
    // Skip autotest if all env. variables are empty
    const std::vector<const char *> envVariables {
        "DB_PGSQL_HOST", "DB_PGSQL_PORT", "DB_PGSQL_DATABASE", "DB_PGSQL_SCHEMA",
        "DB_PGSQL_USERNAME", "DB_PGSQL_PASSWORD", "DB_PGSQL_CHARSET"
    };

    if (TestUtils::Databases::allEnvVariablesEmpty(envVariables))
        QSKIP("Autotest skipped because DB_PGSQL_* environment variables for PostgreSQL "
              "connection were not defined.", );

    const auto connectionName =
            QStringLiteral(
                "tinyorm_pgsql_tests-tst_DatabaseMannager-removeConnection_Connected");
    const auto databaseName = qEnvironmentVariable("DB_PGSQL_DATABASE", "");
    const auto driverName = QPSQL;

    // Create database connection
    m_dm->addConnections({
        {connectionName, {
            {driver_,   driverName},
            {host_,     qEnvironmentVariable("DB_PGSQL_HOST", H127001)},
            {port_,     qEnvironmentVariable("DB_PGSQL_PORT", P5432)},
            {database_, databaseName},
            {schema_,   qEnvironmentVariable("DB_PGSQL_SCHEMA", PUBLIC)},
            {username_, qEnvironmentVariable("DB_PGSQL_USERNAME",
                                             QStringLiteral("postgres"))},
            {password_, qEnvironmentVariable("DB_PGSQL_PASSWORD", "")},
            {charset_,  qEnvironmentVariable("DB_PGSQL_CHARSET", UTF8)},
        }},
    // Don't setup any default connection
    }, "");

    // Open connection
    auto &connection = m_dm->connection(connectionName);
    const auto openedConnections = m_dm->openedConnectionNames();

    QCOMPARE(connection.getName(), connectionName);
    QCOMPARE(connection.getDatabaseName(), databaseName);
    QCOMPARE(connection.driverName(), driverName);
    QCOMPARE(openedConnections.size(), 1);
    QCOMPARE(openedConnections.first(), connectionName);
    QCOMPARE(m_dm->connectionNames().size(), 1);
    QVERIFY(m_dm->getDefaultConnection().isEmpty());

    // Remove opened connection
    QVERIFY(m_dm->removeConnection(connectionName));

    QVERIFY(m_dm->getDefaultConnection().isEmpty());
    QVERIFY(m_dm->openedConnectionNames().isEmpty());
    QVERIFY(m_dm->connectionNames().isEmpty());

    // Restore defaults
    m_dm->resetDefaultConnection();
    QCOMPARE(m_dm->getDefaultConnection(), DatabaseConfiguration::defaultConnectionName);
}

void tst_DatabaseManager::removeConnection_NotConnected() const
{
    const auto connectionName =
            QStringLiteral(
                "tinyorm_pgsql_tests-tst_DatabaseMannager-removeConnection_NotConnected");

    m_dm->addConnection({
        {driver_, QPSQL},
        {host_,   "example.com"},
    }, connectionName);

    m_dm->setDefaultConnection(connectionName);

    QVERIFY(m_dm->openedConnectionNames().isEmpty());
    QCOMPARE(m_dm->connectionNames().size(), 1);
    QCOMPARE(m_dm->getDefaultConnection(), connectionName);

    // Remove database connection that is not opened
    QVERIFY(m_dm->removeConnection(connectionName));

    QVERIFY(m_dm->openedConnectionNames().isEmpty());
    QVERIFY(m_dm->connectionNames().isEmpty());
    /* When the connection was also a default connection, then DM will reset
       the default connection. */
    QCOMPARE(m_dm->getDefaultConnection(), DatabaseConfiguration::defaultConnectionName);
}

QTEST_MAIN(tst_DatabaseManager)

#include "tst_databasemanager.moc"
