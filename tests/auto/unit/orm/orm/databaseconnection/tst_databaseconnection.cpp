#include <QCoreApplication>
#include <QtTest>

#include "orm/db.hpp"
#include "orm/mysqlconnection.hpp"

#include "database.hpp"

using Orm::MySqlConnection;

class tst_DatabaseConnection : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase_data() const;

    void pingDatabase() const;

    void isNotMaria_OnMySqlConnection() const;
};

void tst_DatabaseConnection::initTestCase_data() const
{
    QTest::addColumn<QString>("connection");

    // Run all tests for all supported database connections
    for (const auto &connection : TestUtils::Database::createConnections())
        QTest::newRow(connection.toUtf8().constData()) << connection;
}

void tst_DatabaseConnection::pingDatabase() const
{
    QFETCH_GLOBAL(QString, connection);

    auto &connection_ = DB::connection(connection);

    if (const auto driverName = connection_.driverName();
        driverName == "QSQLITE"
    )
        QSKIP(QStringLiteral("The '%1' database driver doesn't support ping command.")
              .arg(driverName).toUtf8().constData(), );

    const auto result = connection_.pingDatabase();

    QVERIFY2(result, "Ping database failed.");
}

void tst_DatabaseConnection::isNotMaria_OnMySqlConnection() const
{
    QFETCH_GLOBAL(QString, connection);

    auto &connection_ = DB::connection(connection);

    if (const auto driverName = connection_.driverName();
        driverName != "QMYSQL"
    )
        QSKIP(QStringLiteral("The '%1' database driver doesn't implement isMaria() "
                             "method.")
              .arg(driverName).toUtf8().constData(), );

    const auto expected = !(connection == "tinyorm_mysql_tests");

    const auto isMaria = dynamic_cast<MySqlConnection &>(connection_).isMaria();

    QCOMPARE(isMaria, expected);
}

QTEST_MAIN(tst_DatabaseConnection)

#include "tst_databaseconnection.moc"
