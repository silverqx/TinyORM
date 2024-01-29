#include <QCoreApplication>
#include <QtTest>

#include "orm/constants.hpp"
#include "orm/utils/configuration.hpp"
#include "orm/utils/type.hpp"

#include "orm/drivers/sqldatabase.hpp"
#include "orm/drivers/sqldriver.hpp"
#include "orm/drivers/sqlquery.hpp"

#include "databases.hpp"

using Orm::Constants::EMPTY;
using Orm::Constants::H127001;
using Orm::Constants::NOSPACE;
using Orm::Constants::P5432;
using Orm::Constants::QMYSQL;
using Orm::Constants::UTF8MB4;
using Orm::Constants::UTF8MB40900aici;
using Orm::Constants::charset_;
using Orm::Constants::collation_;
using Orm::Constants::database_;
using Orm::Constants::driver_;
using Orm::Constants::host_;
using Orm::Constants::options_;
using Orm::Constants::password_;
using Orm::Constants::port_;
using Orm::Constants::username_;

using ConfigUtils = Orm::Utils::Configuration;
using TypeUtils = Orm::Utils::Type;

using Orm::Drivers::CursorPosition;
using Orm::Drivers::NumericalPrecisionPolicy;
using Orm::Drivers::SqlDatabase;
using Orm::Drivers::SqlDriver;
using Orm::Drivers::SqlQuery;

using TestUtils::Databases;

class tst_SqlDatabaseManager : public QObject // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

private Q_SLOTS:
    void MySQL_removeConnection_Connected() const;
    void MySQL_removeConnection_NotConnected() const;

    void default_MySQL_ConfigurationValues() const;

    void MySQL_addUseAndRemoveConnection_FiveTimes() const;
    void MySQL_addUseAndRemoveThreeConnections_FiveTimes() const;

// NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    /*! Test case class name. */
    inline static const auto *ClassName = "tst_SqlDatabaseManager";
};

/* private slots */

// NOLINTBEGIN(readability-convert-member-functions-to-static)
void tst_SqlDatabaseManager::MySQL_removeConnection_Connected() const
{
    const auto databaseName = qEnvironmentVariable("DB_MYSQL_DATABASE", EMPTY);
    const auto driverName = QMYSQL;

    // Add a new database connection
    const auto connectionName = Databases::createDriversConnectionTemp(
                                    Databases::MYSQL_DRIVERS,
                                    {ClassName, QString::fromUtf8(__func__)}, // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    {
        {driver_,    driverName},
        {host_,      qEnvironmentVariable("DB_MYSQL_HOST",      H127001)},
        {port_,      qEnvironmentVariable("DB_MYSQL_PORT",      P5432)},
        {database_,  databaseName},
        {username_,  qEnvironmentVariable("DB_MYSQL_USERNAME",  EMPTY)},
        {password_,  qEnvironmentVariable("DB_MYSQL_PASSWORD",  EMPTY)},
        {charset_,   qEnvironmentVariable("DB_MYSQL_CHARSET",   UTF8MB4)},
        {collation_, qEnvironmentVariable("DB_MYSQL_COLLATION", UTF8MB40900aici)},
        {options_,   ConfigUtils::mysqlSslOptions()},
    });

    if (!connectionName)
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::MYSQL_DRIVERS)
              .toUtf8().constData(), );

    // Connection is already open so pass the false to verify it
    const auto connection = Databases::driversConnection(*connectionName, false);
    const auto openedConnections = Databases::driversOpenedConnectionNames();

    QVERIFY(connection.isValid());
    QVERIFY(connection.isOpen());
    QVERIFY(!connection.isOpenError());
    QCOMPARE(connection.connectionName(), *connectionName);
    QCOMPARE(connection.databaseName(), databaseName);
    QCOMPARE(connection.driverName(), driverName);
    QCOMPARE(openedConnections.size(), 1);
    QCOMPARE(openedConnections.first(), *connectionName);
    QCOMPARE(Databases::driversConnectionNames().size(), 1);

    // Remove opened connection
    /* This will generate expected warning about the connection is still in use
       it can't be avoided because we want to test the connection.isValid(). */
    Databases::removeDriversConnection(*connectionName);

    // The sqldriver must be invalidated immediately
    QVERIFY(!connection.isValid());
    QVERIFY(Databases::driversOpenedConnectionNames().isEmpty());
    QVERIFY(Databases::driversConnectionNames().isEmpty());
}

void tst_SqlDatabaseManager::MySQL_removeConnection_NotConnected() const
{
    // Add a new database connection
    const auto connectionName = Databases::createDriversConnectionTemp(
                                    Databases::MYSQL_DRIVERS,
                                    {ClassName, QString::fromUtf8(__func__)}, // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    {
        {driver_, QMYSQL},
        {host_,   sl("example.com")},
    },
        false);

    if (!connectionName)
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::MYSQL_DRIVERS)
              .toUtf8().constData(), );

    // Connection must be closed
    const auto connection = Databases::driversConnection(*connectionName, false);

    QVERIFY(connection.isValid());
    QVERIFY(!connection.isOpen());
    QVERIFY(!connection.isOpenError());
    QVERIFY(Databases::driversOpenedConnectionNames().isEmpty());
    QCOMPARE(Databases::driversConnectionNames().size(), 1);

    // Remove database connection that is not opened
    /* This will generate expected warning about the connection is still in use
       it can't be avoided because we want to test the connection.isValid(). */
    Databases::removeDriversConnection(*connectionName);

    // The sqldriver must be invalidated immediately
    QVERIFY(!connection.isValid());
    QVERIFY(Databases::driversOpenedConnectionNames().isEmpty());
    QCOMPARE(Databases::driversConnectionNames().size(), 0);
}

// No need to test with other database drivers because the behavior is the same
void tst_SqlDatabaseManager::default_MySQL_ConfigurationValues() const
{
    // Add a new database connection
    const auto connectionName = Databases::createDriversConnectionTemp(
                                    Databases::MYSQL_DRIVERS,
                                    {ClassName, QString::fromUtf8(__func__)}, // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    {
        {driver_, QMYSQL},
    },
        false);

    if (!connectionName)
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::MYSQL_DRIVERS)
              .toUtf8().constData(), );

    // Connection must be closed
    const auto connection = Databases::driversConnection(*connectionName, false);

    // Main section
    QVERIFY(connection.isValid());
    QVERIFY(!connection.isOpen());
    QVERIFY(!connection.isOpenError());
    // CUR drivers revisit here the SqlError check after exceptions refactor; add it or ... silverqx
    QCOMPARE(Databases::driversConnectionNames().size(), 1);
    QVERIFY(Databases::driversOpenedConnectionNames().isEmpty());

    // No need to distinguish between empty and null QString values
    QCOMPARE(connection.driverName(), QMYSQL);
    QCOMPARE(connection.connectionName(), *connectionName);
    QVERIFY(connection.hostName().isEmpty());
    QCOMPARE(connection.port(), -1);
    QVERIFY(connection.databaseName().isEmpty());
    QVERIFY(connection.userName().isEmpty());
    QVERIFY(connection.password().isEmpty());
    QVERIFY(connection.connectOptions().isEmpty());
    const auto driver = connection.driverWeak().lock();
    QCOMPARE(connection.numericalPrecisionPolicy(),
             driver->defaultNumericalPrecisionPolicy());
    QVERIFY(Databases::driversIsThreadCheck());

    QCOMPARE(driver->dbmsType(), SqlDriver::MySqlServer);
    QCOMPARE(driver->defaultNumericalPrecisionPolicy(),
             NumericalPrecisionPolicy::LowPrecisionDouble);

    // Restore
    /* This will generate expected warning about the connection is still in use
       it can't be avoided because we want to test the connection.isValid(). */
    Databases::removeDriversConnection(*connectionName);

    // The sqldriver must be invalidated immediately
    QVERIFY(!connection.isValid());
    QVERIFY(Databases::driversConnectionNames().isEmpty());
    QVERIFY(Databases::driversOpenedConnectionNames().isEmpty());
}

void tst_SqlDatabaseManager::MySQL_addUseAndRemoveConnection_FiveTimes() const
{
    for (auto i = 0; i < 5; ++i) {
        // Add a new MYSQL database connection
        const auto connectionName =
                Databases::createDriversConnectionTempFrom(
                    Databases::MYSQL_DRIVERS, {ClassName, QString::fromUtf8(__func__)}); // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)

        if (i == 0 && !connectionName)
            QSKIP(TestUtils::AutoTestSkipped
                  .arg(TypeUtils::classPureBasename(*this), Databases::MYSQL_DRIVERS)
                  .toUtf8().constData(), );

        // Connection is already open so pass the false to verify it
        const auto connection = Databases::driversConnection(*connectionName, false);
        const auto openedConnections = Databases::driversOpenedConnectionNames();

        QVERIFY(connection.isValid());
        QVERIFY(connection.isOpen());
        QVERIFY(!connection.isOpenError());
        QCOMPARE(openedConnections.size(), 1);
        QCOMPARE(openedConnections.first(), *connectionName);
        QCOMPARE(Databases::driversConnectionNames().size(), 1);

        // Execute some database query
        SqlQuery query(connection);
        query.exec(sl("select count(*) as user_count from users"));

        // Verify the query
        QVERIFY(query.isActive());
        QVERIFY(query.isSelect());
        QVERIFY(!query.isValid());
        QCOMPARE(query.at(), CursorPosition::BeforeFirstRow);
        QCOMPARE(query.size(), 1);

        // Verify the result
        QVERIFY(query.first());
        QVERIFY(query.isValid());
        QCOMPARE(query.at(), 0);
        QCOMPARE(query.value(sl("user_count")).value<quint64>(), 5);

        // Restore
        /* This will generate expected warning about the connection is still in use
           it can't be avoided because we want to test the query.driver/Weak(). */
        Databases::removeDriversConnection(*connectionName);

        // The sqldriver must be invalidated immediately
        QVERIFY(query.driverWeak().expired());
        QCOMPARE(query.driver(), nullptr);
        QVERIFY(!connection.isValid());
        QVERIFY(Databases::driversConnectionNames().isEmpty());
        QVERIFY(Databases::driversOpenedConnectionNames().isEmpty());
    }
}

// I will not complicate and abstract a code in the following test, it's good enough
void tst_SqlDatabaseManager::MySQL_addUseAndRemoveThreeConnections_FiveTimes() const
{
    for (auto i = 0; i < 5; ++i) {
        // Add 3 new MYSQL database connections
        const auto connectionName1 =
                Databases::createDriversConnectionTempFrom(
                    Databases::MYSQL_DRIVERS,
                    {ClassName, NOSPACE.arg(QString::fromUtf8(__func__), sl("1"))});// NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)

        const auto connectionName2 =
                Databases::createDriversConnectionTempFrom(
                    Databases::MYSQL_DRIVERS,
                    {ClassName, NOSPACE.arg(QString::fromUtf8(__func__), sl("2"))});// NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)

        const auto connectionName3 =
                Databases::createDriversConnectionTempFrom(
                    Databases::MYSQL_DRIVERS,
                    {ClassName, NOSPACE.arg(QString::fromUtf8(__func__), sl("3"))});// NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)

        if (i == 0 && !connectionName1)
            QSKIP(TestUtils::AutoTestSkipped
                      .arg(TypeUtils::classPureBasename(*this), Databases::MYSQL_DRIVERS)
                      .toUtf8().constData(), );

        // Connection is already open so pass the false to verify it
        const auto connection1 = Databases::driversConnection(*connectionName1, false);
        const auto connection2 = Databases::driversConnection(*connectionName2, false);
        const auto connection3 = Databases::driversConnection(*connectionName3, false);
        const auto openedConnections = Databases::driversOpenedConnectionNames();

        QVERIFY(connection1.isValid());
        QVERIFY(connection1.isOpen());
        QVERIFY(!connection1.isOpenError());
        QVERIFY(connection2.isValid());
        QVERIFY(connection2.isOpen());
        QVERIFY(!connection2.isOpenError());
        QVERIFY(connection3.isValid());
        QVERIFY(connection3.isOpen());
        QVERIFY(!connection3.isOpenError());

        QCOMPARE(openedConnections.size(), 3);
        QVERIFY(openedConnections.contains(*connectionName1));
        QVERIFY(openedConnections.contains(*connectionName2));
        QVERIFY(openedConnections.contains(*connectionName3));
        QCOMPARE(Databases::driversConnectionNames().size(), 3);

        // Execute some database query on connection1
        SqlQuery query1(connection1);
        {
            query1.exec(sl("select count(*) as user_count from users"));

            // Verify the query
            QVERIFY(query1.isActive());
            QVERIFY(query1.isSelect());
            QVERIFY(!query1.isValid());
            QCOMPARE(query1.at(), CursorPosition::BeforeFirstRow);
            QCOMPARE(query1.size(), 1);

            // Verify the result
            QVERIFY(query1.first());
            QVERIFY(query1.isValid());
            QCOMPARE(query1.at(), 0);
            QCOMPARE(query1.value(sl("user_count")).value<quint64>(), 5);
        }
        // Execute some database query on connection2
        SqlQuery query2(connection2);
        {
            query2.exec(sl("select count(*) as user_count from users"));

            // Verify the query
            QVERIFY(query2.isActive());
            QVERIFY(query2.isSelect());
            QVERIFY(!query2.isValid());
            QCOMPARE(query2.at(), CursorPosition::BeforeFirstRow);
            QCOMPARE(query2.size(), 1);

            // Verify the result
            QVERIFY(query2.first());
            QVERIFY(query2.isValid());
            QCOMPARE(query2.at(), 0);
            QCOMPARE(query2.value(sl("user_count")).value<quint64>(), 5);
        }
        // Execute some database query on connection3
        SqlQuery query3(connection3);
        {
            query3.exec(sl("select count(*) as user_count from users"));

            // Verify the query
            QVERIFY(query3.isActive());
            QVERIFY(query3.isSelect());
            QVERIFY(!query3.isValid());
            QCOMPARE(query3.at(), CursorPosition::BeforeFirstRow);
            QCOMPARE(query3.size(), 1);

            // Verify the result
            QVERIFY(query3.first());
            QVERIFY(query3.isValid());
            QCOMPARE(query3.at(), 0);
            QCOMPARE(query3.value(sl("user_count")).value<quint64>(), 5);
        }

        // Restore
        /* This will generate expected warning about the connection is still in use
           it can't be avoided because we want to test the query.driver/Weak(). */
        Databases::removeDriversConnection(*connectionName3);
        Databases::removeDriversConnection(*connectionName2);
        Databases::removeDriversConnection(*connectionName1);

        // The sqldriver must be invalidated immediately
        QVERIFY(query3.driverWeak().expired());
        QVERIFY(query2.driverWeak().expired());
        QVERIFY(query1.driverWeak().expired());
        QCOMPARE(query3.driver(), nullptr);
        QCOMPARE(query2.driver(), nullptr);
        QCOMPARE(query1.driver(), nullptr);
        QVERIFY(!connection3.isValid());
        QVERIFY(!connection2.isValid());
        QVERIFY(!connection1.isValid());
        QVERIFY(Databases::driversConnectionNames().isEmpty());
        QVERIFY(Databases::driversOpenedConnectionNames().isEmpty());
    }
}
// NOLINTEND(readability-convert-member-functions-to-static)

QTEST_MAIN(tst_SqlDatabaseManager)

#include "tst_sqldatabasemanager.moc"
