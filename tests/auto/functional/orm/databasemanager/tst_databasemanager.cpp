#include <QCoreApplication>
#include <QtTest>

#include "orm/databasemanager.hpp"

#include "databases.hpp"

using Orm::DatabaseManager;

class tst_DatabaseManager : public QObject
{
    Q_OBJECT

private slots:
    void removeConnection_Connected() const;
    void removeConnection_NotConnected() const;
};

void tst_DatabaseManager::removeConnection_Connected() const
{
    const auto connectionName =
            "tinyorm_mysql_tests-tst_DatabaseMannager-removeConnection_Connected";
    const auto databaseName = qEnvironmentVariable("DB_MYSQL_DATABASE", "");
    const auto driverName = "QMYSQL";

    // Create database connection
    // Ownership of a unique_ptr()
    auto db = DatabaseManager::create({
        {connectionName, {
            {"driver",    driverName},
            {"host",      qEnvironmentVariable("DB_MYSQL_HOST", "127.0.0.1")},
            {"port",      qEnvironmentVariable("DB_MYSQL_PORT", "3306")},
            {"database",  databaseName},
            {"username",  qEnvironmentVariable("DB_MYSQL_USERNAME", "root")},
            {"password",  qEnvironmentVariable("DB_MYSQL_PASSWORD", "")},
            {"charset",   qEnvironmentVariable("DB_MYSQL_CHARSET", "utf8mb4")},
            {"collation", qEnvironmentVariable("DB_MYSQL_COLLATION",
                                               "utf8mb4_0900_ai_ci")},
        }},
    // Don't setup any default connection
    }, "");

    // Open connection
    auto &connection = db->connection(connectionName);
    const auto openedConnections = db->openedConnectionNames();

    QCOMPARE(connection.getName(), connectionName);
    QCOMPARE(connection.getDatabaseName(), databaseName);
    QCOMPARE(connection.driverName(), driverName);
    QCOMPARE(openedConnections.size(), 1);
    QCOMPARE(openedConnections.first(), connectionName);

    // Remove opened connection
    QVERIFY(db->removeConnection(connectionName));
}

void tst_DatabaseManager::removeConnection_NotConnected() const
{
    const auto connectionName = "dummy_connection";

    // Ownership of a unique_ptr()
    auto db = DatabaseManager::create({
        {"driver", "QMYSQL"},
        {"host",   "example.com"},
    }, connectionName);

    QVERIFY(db->removeConnection(connectionName));
}

QTEST_MAIN(tst_DatabaseManager)

#include "tst_databasemanager.moc"
