#include <QCoreApplication>
#include <QtTest>

#include "orm/constants.hpp"
#include "orm/databasemanager.hpp"

#include "databases.hpp"

using Orm::Constants::charset_;
using Orm::Constants::collation_;
using Orm::Constants::database_;
using Orm::Constants::driver_;
using Orm::Constants::H127001;
using Orm::Constants::host_;
using Orm::Constants::P3306;
using Orm::Constants::password_;
using Orm::Constants::port_;
using Orm::Constants::QMYSQL;
using Orm::Constants::ROOT;
using Orm::Constants::username_;
using Orm::Constants::UTF8MB4;
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
            QStringLiteral(
                "tinyorm_mysql_tests-tst_DatabaseMannager-removeConnection_Connected");
    const auto databaseName = qEnvironmentVariable("DB_MYSQL_DATABASE", "");
    const auto driverName = QMYSQL;

    // Create database connection
    // Ownership of a unique_ptr()
    auto db = DatabaseManager::create({
        {connectionName, {
            {driver_,    driverName},
            {host_,      qEnvironmentVariable("DB_MYSQL_HOST", H127001)},
            {port_,      qEnvironmentVariable("DB_MYSQL_PORT", P3306)},
            {database_,  databaseName},
            {username_,  qEnvironmentVariable("DB_MYSQL_USERNAME", ROOT)},
            {password_,  qEnvironmentVariable("DB_MYSQL_PASSWORD", "")},
            {charset_,   qEnvironmentVariable("DB_MYSQL_CHARSET", UTF8MB4)},
            {collation_, qEnvironmentVariable("DB_MYSQL_COLLATION",
                                              QStringLiteral("utf8mb4_0900_ai_ci"))},
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
    const auto connectionName = QStringLiteral("dummy_connection");

    // Ownership of a unique_ptr()
    auto db = DatabaseManager::create({
        {driver_, QMYSQL},
        {host_,   "example.com"},
    }, connectionName);

    QVERIFY(db->removeConnection(connectionName));
}

QTEST_MAIN(tst_DatabaseManager)

#include "tst_databasemanager.moc"
