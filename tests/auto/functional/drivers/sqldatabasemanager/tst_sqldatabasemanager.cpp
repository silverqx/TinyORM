#include <QCoreApplication>
#include <QTest>

/* We need to include MySQL or MariaDB mysql.h to find out whether we are linking against
   the MySQL or MariaDB client library, metadata-related unit tests need this to work
   correctly. */
#if __has_include(<mysql/mysql.h>)
#  include <mysql/mysql.h> // IWYU pragma: keep
#elif __has_include(<mysql.h>)
#  include <mysql.h> // IWYU pragma: keep
#else
#  error Can not find the <mysql.h> header file, please install the MySQL C client \
library.
#endif

#include "orm/drivers/exceptions/invalidargumenterror.hpp"
#include "orm/drivers/sqldatabase.hpp"
#include "orm/drivers/sqldriver.hpp"
#include "orm/drivers/sqlquery.hpp"

#include "orm/constants.hpp"
#include "orm/utils/type.hpp"

#include "databases.hpp"
#include "macros.hpp"

using namespace Qt::StringLiterals; // NOLINT(google-build-using-namespace)

using Orm::Constants::EMPTY;
using Orm::Constants::NOSPACE;
using Orm::Constants::QMYSQL;
using Orm::Constants::SEMICOLON;
using Orm::Constants::driver_;
using Orm::Constants::host_;

using TypeUtils = Orm::Utils::Type;

using Orm::Drivers::CursorPosition;
using Orm::Drivers::Exceptions::InvalidArgumentError;
using Orm::Drivers::NumericalPrecisionPolicy;
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

    void MySQL_addExistingConnection_ThrowException() const;

    void MySQL_enableOptionalMetadata_ThrowException() const;
    void MySQL_disableOptionalMetadata_CLIENT_OPTIONAL() const;
    void MySQL_MySQL_disableOptionalMetadata_MYSQL_OPT() const;
    void MySQL_MariaDB_disableOptionalMetadata_MYSQL_OPT() const;

// NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    /*! Test case class name. */
    inline static const auto *ClassName = "tst_SqlDatabaseManager";
};

/* private slots */

// NOLINTBEGIN(readability-convert-member-functions-to-static)
void tst_SqlDatabaseManager::MySQL_removeConnection_Connected() const
{
    // Add a new database connection
    const auto connectionName = Databases::createDriversConnectionTempFrom(
                                    Databases::MYSQL_DRIVERS,
                                    {ClassName, QString::fromUtf8(__func__)}); // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)

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
    QCOMPARE(connection.databaseName(), qEnvironmentVariable("DB_MYSQL_DATABASE", EMPTY));
    QCOMPARE(connection.driverName(), QMYSQL);
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
        {host_,   u"example.com"_s},
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
        query.exec(u"select count(*) as user_count from users"_s);

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
        QCOMPARE(query.value(u"user_count"_s).value<quint64>(), 5);

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
                    {ClassName, NOSPACE.arg(QString::fromUtf8(__func__), u"1"_s)});// NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)

        const auto connectionName2 =
                Databases::createDriversConnectionTempFrom(
                    Databases::MYSQL_DRIVERS,
                    {ClassName, NOSPACE.arg(QString::fromUtf8(__func__), u"2"_s)});// NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)

        const auto connectionName3 =
                Databases::createDriversConnectionTempFrom(
                    Databases::MYSQL_DRIVERS,
                    {ClassName, NOSPACE.arg(QString::fromUtf8(__func__), u"3"_s)});// NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)

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
            query1.exec(u"select count(*) as user_count from users"_s);

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
            QCOMPARE(query1.value(u"user_count"_s).value<quint64>(), 5);
        }
        // Execute some database query on connection2
        SqlQuery query2(connection2);
        {
            query2.exec(u"select count(*) as user_count from users"_s);

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
            QCOMPARE(query2.value(u"user_count"_s).value<quint64>(), 5);
        }
        // Execute some database query on connection3
        SqlQuery query3(connection3);
        {
            query3.exec(u"select count(*) as user_count from users"_s);

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
            QCOMPARE(query3.value(u"user_count"_s).value<quint64>(), 5);
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

void tst_SqlDatabaseManager::MySQL_addExistingConnection_ThrowException() const
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

    TVERIFY_THROWS_EXCEPTION(
                InvalidArgumentError,
                Databases::createDriversConnectionTemp(
                    Databases::MYSQL_DRIVERS,
                    {ClassName, QString::fromUtf8(__func__)},
    {
        {driver_, QMYSQL},
    },
        false));

    // Re-verify
    QVERIFY(connection.isValid());
    QVERIFY(!connection.isOpen());
    QVERIFY(!connection.isOpenError());
    // CUR drivers revisit here the SqlError check after exceptions refactor; add it or ... silverqx
    QCOMPARE(Databases::driversConnectionNames().size(), 1);
    QVERIFY(Databases::driversOpenedConnectionNames().isEmpty());

    // Restore
    /* This will generate expected warning about the connection is still in use
       it can't be avoided because we want to test the query.driver/Weak(). */
    Databases::removeDriversConnection(*connectionName);

    // The sqldriver must be invalidated immediately
    QVERIFY(!connection.isValid());
    QVERIFY(Databases::driversConnectionNames().isEmpty());
    QVERIFY(Databases::driversOpenedConnectionNames().isEmpty());
}

void tst_SqlDatabaseManager::MySQL_enableOptionalMetadata_ThrowException() const
{
    /* This test method tests two things, unsupported metadata-related options but it
       also tests or behaves the same for MariaDB because MariaDB doesn't support
       metadata at all. So the result is in both cases the same, it must throw. */

    // CLIENT_OPTIONAL_RESULTSET_METADATA
    {
        const auto connectionName =
                Databases::createDriversConnectionTempFrom(
                    Databases::MYSQL_DRIVERS, {ClassName, QString::fromUtf8(__func__)}, // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
                    false);

        if (!connectionName)
            QSKIP(TestUtils::AutoTestSkipped
                  .arg(TypeUtils::classPureBasename(*this), Databases::MYSQL_DRIVERS)
                  .toUtf8().constData(), );

        auto connection = Databases::driversConnection(*connectionName, false);
        /* Setting it this way, I will not refactor the createDriversConnectionTempFrom()
           to be able modify options_ hash. */
        connection.setConnectOptions(
                    QStringList({connection.connectOptions(),
                                 u"CLIENT_OPTIONAL_RESULTSET_METADATA"_s}) // Must support option without the value (=ON/OFF; w/o value == ON)
                    .join(SEMICOLON));

        TVERIFY_THROWS_EXCEPTION(InvalidArgumentError, connection.open());

        // Restore
        /* This will generate expected warning about the connection is still in use
           it can't be avoided because we want to test the query.driver/Weak(). */
        Databases::removeDriversConnection(*connectionName);

        // The sqldriver must be invalidated immediately
        QVERIFY(!connection.isValid());
        QVERIFY(Databases::driversConnectionNames().isEmpty());
        QVERIFY(Databases::driversOpenedConnectionNames().isEmpty());
    }

    // MYSQL_OPT_OPTIONAL_RESULTSET_METADATA
    {
        const auto connectionName =
                Databases::createDriversConnectionTempFrom(
                    Databases::MYSQL_DRIVERS, {ClassName, QString::fromUtf8(__func__)}, // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
                    false);

        if (!connectionName)
            QSKIP(TestUtils::AutoTestSkipped
                  .arg(TypeUtils::classPureBasename(*this), Databases::MYSQL_DRIVERS)
                  .toUtf8().constData(), );

        auto connection = Databases::driversConnection(*connectionName, false);
        /* Setting it this way, I will not refactor the createDriversConnectionTempFrom()
           to be able modify options_ hash. */
        connection.setConnectOptions(
                    QStringList({connection.connectOptions(),
                                 u"MYSQL_OPT_OPTIONAL_RESULTSET_METADATA=ON"_s})
                    .join(SEMICOLON));

        TVERIFY_THROWS_EXCEPTION(InvalidArgumentError, connection.open());

        // Restore
        /* This will generate expected warning about the connection is still in use
           it can't be avoided because we want to test the query.driver/Weak(). */
        Databases::removeDriversConnection(*connectionName);

        // The sqldriver must be invalidated immediately
        QVERIFY(!connection.isValid());
        QVERIFY(Databases::driversConnectionNames().isEmpty());
        QVERIFY(Databases::driversOpenedConnectionNames().isEmpty());
    }
}

void tst_SqlDatabaseManager::MySQL_disableOptionalMetadata_CLIENT_OPTIONAL() const
{
    /* We don't need to skip this test if linked against the MariaDB client library
       because the CLIENT_OPTIONAL_RESULTSET_METADATA is passed as the string everywhere
       and TinyMySql catch this and throws. */

    /* CLIENT_OPTIONAL_RESULTSET_METADATA=OFF
       Setting CLIENT_OPTIONAL_RESULTSET_METADATA to OFF is another story than setting
       MYSQL_OPT_OPTIONAL_RESULTSET_METADATA to OFF, it must still throw an exception
       because it's a pure flag, so if it's set it will be OR-ed to the connection flags
       and there is nothing like detecting if it's a flag option and if is set to OFF
       then remove this flag from connection flags, so must throw. */
    const auto connectionName =
            Databases::createDriversConnectionTempFrom(
                Databases::MYSQL_DRIVERS, {ClassName, QString::fromUtf8(__func__)}, // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
                false);

    if (!connectionName)
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::MYSQL_DRIVERS)
              .toUtf8().constData(), );

    auto connection = Databases::driversConnection(*connectionName, false);
    /* Setting it this way, I will not refactor the createDriversConnectionTempFrom()
       to be able modify options_ hash. */
    connection.setConnectOptions(
                QStringList({connection.connectOptions(),
                             u"CLIENT_OPTIONAL_RESULTSET_METADATA=OFF"_s}) // Must support option without the value (=ON/OFF; w/o value == ON)
                .join(SEMICOLON));

    TVERIFY_THROWS_EXCEPTION(InvalidArgumentError, connection.open());

    // Restore
    /* This will generate expected warning about the connection is still in use
       it can't be avoided because we want to test the query.driver/Weak(). */
    Databases::removeDriversConnection(*connectionName);

    // The sqldriver must be invalidated immediately
    QVERIFY(!connection.isValid());
    QVERIFY(Databases::driversConnectionNames().isEmpty());
    QVERIFY(Databases::driversOpenedConnectionNames().isEmpty());
}

void tst_SqlDatabaseManager::MySQL_MySQL_disableOptionalMetadata_MYSQL_OPT() const
{
    /* The metadata-related flags don't exist if linked against MariaDB client library
       (the next unit test tests this). */
#ifdef MARIADB_VERSION_ID
    QSKIP("The MariaDB database doesn't support "
          "MYSQL_OPT_OPTIONAL_RESULTSET_METADATA.", );
#endif

    /* MYSQL_OPT_OPTIONAL_RESULTSET_METADATA=OFF
       Setting it to off is correct because it will call mysql_options(), MySQL client
       internally removes the CLIENT_OPTIONAL_RESULTSET_METADATA flag from connection
       flags. */
    const auto connectionName =
            Databases::createDriversConnectionTempFrom(
                Databases::MYSQL_DRIVERS, {ClassName, QString::fromUtf8(__func__)}, // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
                false);

    if (!connectionName)
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::MYSQL_DRIVERS)
              .toUtf8().constData(), );

    auto connection = Databases::driversConnection(*connectionName, false);
    /* Setting it this way, I will not refactor the createDriversConnectionTempFrom()
       to be able modify options_ hash. */
    connection.setConnectOptions(
                QStringList({connection.connectOptions(),
                             u"MYSQL_OPT_OPTIONAL_RESULTSET_METADATA=OFF"_s})
                .join(SEMICOLON));

    QVERIFY(connection.open());

    // Restore
    /* This will generate expected warning about the connection is still in use
       it can't be avoided because we want to test the query.driver/Weak(). */
    Databases::removeDriversConnection(*connectionName);

    // The sqldriver must be invalidated immediately
    QVERIFY(!connection.isValid());
    QVERIFY(Databases::driversConnectionNames().isEmpty());
    QVERIFY(Databases::driversOpenedConnectionNames().isEmpty());
}

void tst_SqlDatabaseManager::MySQL_MariaDB_disableOptionalMetadata_MYSQL_OPT() const
{
    /* The metadata-related flags don't exist if linked against MariaDB client library,
       this unit test tests this. */
#ifndef MARIADB_VERSION_ID
    QSKIP("The MySQL database supports MYSQL_OPT_OPTIONAL_RESULTSET_METADATA, this "
          "unit test is for MariaDB client library, so skipping.", );
#endif

    /* MYSQL_OPT_OPTIONAL_RESULTSET_METADATA=OFF
       Setting it to off is correct because it will call mysql_options(), MySQL client
       internally removes the CLIENT_OPTIONAL_RESULTSET_METADATA flag from connection
       flags. */
    const auto connectionName =
            Databases::createDriversConnectionTempFrom(
                Databases::MYSQL_DRIVERS, {ClassName, QString::fromUtf8(__func__)}, // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
                false);

    if (!connectionName)
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::MYSQL_DRIVERS)
              .toUtf8().constData(), );

    auto connection = Databases::driversConnection(*connectionName, false);
    /* Setting it this way, I will not refactor the createDriversConnectionTempFrom()
       to be able modify options_ hash. */
    connection.setConnectOptions(
                QStringList({connection.connectOptions(),
                             u"MYSQL_OPT_OPTIONAL_RESULTSET_METADATA=OFF"_s})
                .join(SEMICOLON));

    TVERIFY_THROWS_EXCEPTION(InvalidArgumentError, connection.open());

    // Restore
    /* This will generate expected warning about the connection is still in use
       it can't be avoided because we want to test the query.driver/Weak(). */
    Databases::removeDriversConnection(*connectionName);

    // The sqldriver must be invalidated immediately
    QVERIFY(!connection.isValid());
    QVERIFY(Databases::driversConnectionNames().isEmpty());
    QVERIFY(Databases::driversOpenedConnectionNames().isEmpty());
}
// NOLINTEND(readability-convert-member-functions-to-static)

QTEST_MAIN(tst_SqlDatabaseManager)

#include "tst_sqldatabasemanager.moc"
