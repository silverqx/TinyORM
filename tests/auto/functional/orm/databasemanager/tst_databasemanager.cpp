#include <QCoreApplication>
#include <QtTest>

#include "orm/databasemanager.hpp"
#include "orm/exceptions/sqlitedatabasedoesnotexisterror.hpp"
#include "orm/utils/type.hpp"

#include "databases.hpp"

using Orm::Constants::EMPTY;
using Orm::Constants::H127001;
using Orm::Constants::NAME;
using Orm::Constants::P5432;
using Orm::Constants::PUBLIC;
using Orm::Constants::QMYSQL;
using Orm::Constants::QPSQL;
using Orm::Constants::QSQLITE;
using Orm::Constants::SSL_CA;
using Orm::Constants::SSL_CERT;
using Orm::Constants::SSL_KEY;
using Orm::Constants::UTF8;
using Orm::Constants::Version;
using Orm::Constants::application_name;
using Orm::Constants::charset_;
using Orm::Constants::check_database_exists;
using Orm::Constants::database_;
using Orm::Constants::dont_drop;
using Orm::Constants::driver_;
using Orm::Constants::host_;
using Orm::Constants::options_;
using Orm::Constants::password_;
using Orm::Constants::port_;
using Orm::Constants::prefix_;
using Orm::Constants::prefix_indexes;
using Orm::Constants::qt_timezone;
using Orm::Constants::return_qdatetime;
using Orm::Constants::search_path;
using Orm::Constants::spatial_ref_sys;
using Orm::Constants::ssl_cert;
using Orm::Constants::sslcert;
using Orm::Constants::sslkey;
using Orm::Constants::sslmode_;
using Orm::Constants::sslrootcert;
using Orm::Constants::username_;
using Orm::Constants::verify_full;

using Orm::DB;
using Orm::DatabaseManager;
using Orm::Exceptions::SQLiteDatabaseDoesNotExistError;
using Orm::QtTimeZoneConfig;
using Orm::QtTimeZoneType;
using Orm::Support::DatabaseConfiguration;

using TypeUtils = Orm::Utils::Type;

using TestUtils::Databases;

class tst_DatabaseManager : public QObject // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();

    void removeConnection_Connected() const;
    void removeConnection_NotConnected() const;

    void default_MySQL_ConfigurationValues() const;
    void default_MariaDB_ConfigurationValues() const;
    void default_PostgreSQL_ConfigurationValues() const;
    void default_SQLite_ConfigurationValues() const;

    void ssl_MySQL_ConfigurationValues() const;
    void ssl_MariaDB_ConfigurationValues() const;
    void ssl_PostgreSQL_ConfigurationValues() const;

    void sqlite_MemoryDriver() const;

    void sqlite_CheckDatabaseExists_True() const;
    void sqlite_CheckDatabaseExists_False() const;

    void addUseAndRemoveConnection_FiveTimes() const;

// NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    /*! Test case class name. */
    inline static const auto *ClassName = "tst_DatabaseManager";

    /*! Path to the SQLite database file, for testing the 'check_database_exists'
        configuration option. */
    static const QString &checkDatabaseExistsFile();

    /*! The Database Manager used in this test case. */
    std::shared_ptr<DatabaseManager> m_dm {};
    /*! Number of connections created in the initTestCase() method. */
    QStringList::size_type m_initialConnectionsCount = 0;
};

/* private slots */

// NOLINTBEGIN(readability-convert-member-functions-to-static)
void tst_DatabaseManager::initTestCase()
{
    const auto connections = Databases::createConnections({Databases::MYSQL});

    if (connections.isEmpty())
        QSKIP(TestUtils::AutoTestSkippedAny.arg(TypeUtils::classPureBasename(*this))
                                           .toUtf8().constData(), );

    m_dm = Databases::manager();
    // Used to test and compare number of connections in DM (or opened connections)
    m_initialConnectionsCount = connections.size();

    // Default connection must be empty
    QVERIFY(m_dm->getDefaultConnection().isEmpty());
}

void tst_DatabaseManager::removeConnection_Connected() const
{
    const auto databaseName = qEnvironmentVariable("DB_PGSQL_DATABASE", "");
    const auto driverName = QPSQL;

    // Add a new database connection
    const auto connectionName = Databases::createConnectionTemp(
                                    Databases::POSTGRESQL,
                                    {ClassName, QString::fromUtf8(__func__)}, // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    {
        {driver_,          driverName},
        {application_name, QStringLiteral("TinyORM tests - tst_databasemanager")},
        {host_,            qEnvironmentVariable("DB_PGSQL_HOST", H127001)},
        {port_,            qEnvironmentVariable("DB_PGSQL_PORT", P5432)},
        {database_,        databaseName},
        {search_path,      qEnvironmentVariable("DB_PGSQL_SEARCHPATH", PUBLIC)},
        {username_,        qEnvironmentVariable("DB_PGSQL_USERNAME",
                                                QStringLiteral("postgres"))},
        {password_,        qEnvironmentVariable("DB_PGSQL_PASSWORD", "")},
        {charset_,         qEnvironmentVariable("DB_PGSQL_CHARSET", UTF8)},
    });

    if (!connectionName)
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::POSTGRESQL)
              .toUtf8().constData(), );

    // Open connection
    auto &connection = m_dm->connection(*connectionName);
    const auto openedConnections = m_dm->openedConnectionNames();

    QCOMPARE(connection.getName(), *connectionName);
    QCOMPARE(connection.getDatabaseName(), databaseName);
    QCOMPARE(connection.driverName(), driverName);
    QCOMPARE(openedConnections.size(), 1);
    QCOMPARE(openedConnections.first(), *connectionName);
    QCOMPARE(m_dm->connectionNames().size(), m_initialConnectionsCount + 1);
    QVERIFY(m_dm->getDefaultConnection().isEmpty());

    // Remove opened connection
    QVERIFY(Databases::removeConnection(*connectionName));

    QVERIFY(m_dm->getDefaultConnection().isEmpty());
    QVERIFY(m_dm->openedConnectionNames().isEmpty());
    QCOMPARE(m_dm->connectionNames().size(), m_initialConnectionsCount);

    // Restore defaults
    m_dm->resetDefaultConnection();
    QCOMPARE(m_dm->getDefaultConnection(), DatabaseConfiguration::defaultConnectionName);
}

void tst_DatabaseManager::removeConnection_NotConnected() const
{
    // Add a new database connection
    const auto connectionName = Databases::createConnectionTemp(
                                    Databases::POSTGRESQL,
                                    {ClassName, QString::fromUtf8(__func__)}, // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    {
        {driver_, QPSQL},
        {host_,   "example.com"},
    });

    if (!connectionName)
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::POSTGRESQL)
              .toUtf8().constData(), );

    m_dm->setDefaultConnection(*connectionName);

    QVERIFY(m_dm->openedConnectionNames().isEmpty());
    QCOMPARE(m_dm->connectionNames().size(), m_initialConnectionsCount + 1);
    QCOMPARE(m_dm->getDefaultConnection(), *connectionName);

    // Remove database connection that is not opened
    QVERIFY(Databases::removeConnection(*connectionName));

    QVERIFY(m_dm->openedConnectionNames().isEmpty());
    QCOMPARE(m_dm->connectionNames().size(), m_initialConnectionsCount);
    /* When the connection was also a default connection, then DM will reset
       the default connection. */
    QCOMPARE(m_dm->getDefaultConnection(), DatabaseConfiguration::defaultConnectionName);
}

void tst_DatabaseManager::default_MySQL_ConfigurationValues() const
{
    // Add a new database connection
    const auto connectionName = Databases::createConnectionTemp(
                                    Databases::MYSQL,
                                    {ClassName, QString::fromUtf8(__func__)}, // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    {
        {driver_, "qmysql"},
    });

    if (!connectionName)
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::MYSQL)
              .toUtf8().constData(), );

    // Original configuration
    // Connection isn't created and configuration options are not parsed yet
    const auto &originalConfig = m_dm->originalConfig(*connectionName);
    QCOMPARE(originalConfig,
             QVariantHash({
                 {driver_, "qmysql"},
             }));

    /* Force the creation of a connection and parse the connection configuration options.
       The qt_timezone option is only parsed in the connection configuration,
       the original configuration is untouched. */
    m_dm->connection(*connectionName);
    QCOMPARE(originalConfig,
             QVariantHash({
                 {driver_,        QMYSQL},
                 {NAME,           *connectionName},
                 {database_,      EMPTY},
                 {prefix_,        EMPTY},
                 {prefix_indexes, false},
                 {options_,       QVariantHash()},
                 {Version,        {}},
             }));

    // Connection configuration
    QCOMPARE(m_dm->getConfig(*connectionName),
             QVariantHash({
                 {driver_,        QMYSQL},
                 {NAME,           *connectionName},
                 {database_,      EMPTY},
                 {prefix_,        EMPTY},
                 {prefix_indexes, false},
                 {options_,       QVariantHash()},
                 {Version,        {}},
                 {qt_timezone,    QVariant::fromValue(
                                      QtTimeZoneConfig {QtTimeZoneType::DontConvert, {}}
                                  )},
             }));

    // Restore
    QVERIFY(Databases::removeConnection(*connectionName));
}

void tst_DatabaseManager::default_MariaDB_ConfigurationValues() const
{
    /* The MariaDB connection should be absolutelly the same and should behave
       the same as the MySQL connection. */

    // Add a new database connection
    const auto connectionName = Databases::createConnectionTemp(
                                    Databases::MARIADB,
                                    {ClassName, QString::fromUtf8(__func__)}, // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    {
        {driver_, "qmysql"},
    });

    if (!connectionName)
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::MARIADB)
              .toUtf8().constData(), );

    // Original configuration
    // Connection isn't created and configuration options are not parsed yet
    const auto &originalConfig = m_dm->originalConfig(*connectionName);
    QCOMPARE(originalConfig,
             QVariantHash({
                 {driver_, "qmysql"},
             }));

    /* Force the creation of a connection and parse the connection configuration options.
       The qt_timezone option is only parsed in the connection configuration,
       the original configuration is untouched. */
    m_dm->connection(*connectionName);
    QCOMPARE(originalConfig,
             QVariantHash({
                 {driver_,        QMYSQL},
                 {NAME,           *connectionName},
                 {database_,      EMPTY},
                 {prefix_,        EMPTY},
                 {prefix_indexes, false},
                 {options_,       QVariantHash()},
                 {Version,        {}},
             }));

    // Connection configuration
    QCOMPARE(m_dm->getConfig(*connectionName),
             QVariantHash({
                 {driver_,        QMYSQL},
                 {NAME,           *connectionName},
                 {database_,      EMPTY},
                 {prefix_,        EMPTY},
                 {prefix_indexes, false},
                 {options_,       QVariantHash()},
                 {Version,        {}},
                 {qt_timezone,    QVariant::fromValue(
                                      QtTimeZoneConfig {QtTimeZoneType::DontConvert, {}}
                                  )},
             }));

    // Restore
    QVERIFY(Databases::removeConnection(*connectionName));
}

void tst_DatabaseManager::default_PostgreSQL_ConfigurationValues() const
{
    // Add a new database connection
    const auto connectionName = Databases::createConnectionTemp(
                                    Databases::POSTGRESQL,
                                    {ClassName, QString::fromUtf8(__func__)}, // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    {
        {driver_, "qpsql"},
    });

    if (!connectionName)
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::POSTGRESQL)
              .toUtf8().constData(), );

    // Original configuration
    // Connection isn't created and configuration options are not parsed yet
    const auto &originalConfig = m_dm->originalConfig(*connectionName);
    QCOMPARE(originalConfig,
             QVariantHash({
                 {driver_, "qpsql"},
             }));

    /* Force the creation of a connection and parse the connection configuration options.
       The qt_timezone option is only parsed in the connection configuration,
       the original configuration is untouched. */
    m_dm->connection(*connectionName);
    QCOMPARE(originalConfig,
             QVariantHash({
                 {driver_,        QPSQL},
                 {NAME,           *connectionName},
                 {database_,      EMPTY},
                 {prefix_,        EMPTY},
                 {prefix_indexes, false},
                 {options_,       QVariantHash()},
                 {dont_drop,      QStringList {spatial_ref_sys}},
             }));

    // Connection configuration
    QCOMPARE(m_dm->getConfig(*connectionName),
             QVariantHash({
                 {driver_,        QPSQL},
                 {NAME,           *connectionName},
                 {database_,      EMPTY},
                 {prefix_,        EMPTY},
                 {prefix_indexes, false},
                 {options_,       QVariantHash()},
                 {dont_drop,      QStringList {spatial_ref_sys}},
                 {qt_timezone,    QVariant::fromValue(
                                      QtTimeZoneConfig {QtTimeZoneType::DontConvert, {}}
                                  )},
             }));

    // Restore
    QVERIFY(Databases::removeConnection(*connectionName));
}

void tst_DatabaseManager::default_SQLite_ConfigurationValues() const
{
    // Add a new database connection
    const auto connectionName = Databases::createConnectionTemp(
                                    Databases::SQLITE,
                                    {ClassName, QString::fromUtf8(__func__)}, // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    {
        {driver_, "qsqlite"},
    });

    if (!connectionName)
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::SQLITE)
              .toUtf8().constData(), );

    // Original configuration
    // Connection isn't created and configuration options are not parsed yet
    const auto &originalConfig = m_dm->originalConfig(*connectionName);
    QCOMPARE(originalConfig,
             QVariantHash({
                 {driver_, "qsqlite"},
             }));

    /* Force the creation of a connection and parse the connection configuration options.
       The qt_timezone option is only parsed in the connection configuration,
       the original configuration is untouched. */
    m_dm->connection(*connectionName);
    QCOMPARE(originalConfig,
             QVariantHash({
                 {driver_,          QSQLITE},
                 {NAME,             *connectionName},
                 {database_,        EMPTY},
                 {prefix_,          EMPTY},
                 {prefix_indexes,   false},
                 {options_,         QVariantHash()},
                 {return_qdatetime, true},
             }));

    // Connection configuration
    QCOMPARE(m_dm->getConfig(*connectionName),
             QVariantHash({
                 {driver_,          QSQLITE},
                 {NAME,             *connectionName},
                 {database_,        EMPTY},
                 {prefix_,          EMPTY},
                 {prefix_indexes,   false},
                 {options_,         QVariantHash()},
                 {return_qdatetime, true},
                 {qt_timezone,      QVariant::fromValue(
                                        QtTimeZoneConfig {QtTimeZoneType::DontConvert, {}}
                                    )},
             }));

    // Restore
    QVERIFY(Databases::removeConnection(*connectionName));
}

void tst_DatabaseManager::ssl_MySQL_ConfigurationValues() const
{
    // Prepare SSL configuration
    // Top level
    const auto sslCertValue = QStringLiteral("C:/example/mysql-cert.pem");
    const auto sslKeyValue  = QStringLiteral("C:/example/mysql-key.pem");
    const auto sslCaValue   = QStringLiteral("C:/example/mysql-ca.pem");
    // The 'options' level
    const auto sslCertOptionsValue = QStringLiteral("D:/example/client-cert.pem");
    const auto sslKeyOptionsValue  = QStringLiteral("D:/example/client-key.pem");

    const QVariantHash initialConfiguration {
        {driver_,   QMYSQL},
        // The ssl_cert is only alias to the "SSL_CERT", looks nicer
        {ssl_cert,  sslCertValue},
        // Lowercase SSL-related option names in top-level configuration
        {"ssl_key", sslKeyValue},
        {"ssl_ca",  sslCaValue},
        {options_,  QVariantHash({{SSL_CERT, sslCertOptionsValue},
                                  {SSL_KEY,  sslKeyOptionsValue}})}
    };

    // Add a new database connection
    const auto connectionName = Databases::createConnectionTemp(
                                    Databases::MYSQL,
                                    {ClassName, QString::fromUtf8(__func__)}, // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
                                    initialConfiguration);

    if (!connectionName)
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::MYSQL)
              .toUtf8().constData(), );

    // Original configuration
    // Connection isn't created and configuration options are not parsed yet
    const auto &originalConfig = m_dm->originalConfig(*connectionName);
    QCOMPARE(originalConfig, initialConfiguration);

    /* Force the creation of a connection and parse the connection configuration options.
       The qt_timezone option is only parsed in the connection configuration,
       the original configuration is untouched. */
    m_dm->connection(*connectionName);
    QCOMPARE(originalConfig,
             QVariantHash({
                 {driver_,        QMYSQL},
                 {NAME,           *connectionName},
                 {database_,      EMPTY},
                 {prefix_,        EMPTY},
                 {prefix_indexes, false},
                 {Version,        {}},
                 // The ssl_cert is only alias to the "SSL_CERT", looks nicer
                 {ssl_cert,       sslCertValue},
                 // Lowercase SSL-related option names in top-level configuration
                 {"ssl_key",      sslKeyValue},
                 {"ssl_ca",       sslCaValue},
                 {options_,       QVariantHash({{SSL_CERT, sslCertOptionsValue},
                                                {SSL_KEY,  sslKeyOptionsValue}})},
             }));

    // Connection configuration
    QCOMPARE(m_dm->getConfig(*connectionName),
             QVariantHash({
                 {driver_,        QMYSQL},
                 {NAME,           *connectionName},
                 {database_,      EMPTY},
                 {prefix_,        EMPTY},
                 {prefix_indexes, false},
                 {Version,        {}},
                 {qt_timezone,    QVariant::fromValue(
                                      QtTimeZoneConfig {QtTimeZoneType::DontConvert, {}}
                                  )},
                 // The ssl_cert is only alias to the "SSL_CERT", looks nicer
                 {ssl_cert,       sslCertValue},
                 // Lowercase SSL-related option names in top-level configuration
                 {"ssl_key",      sslKeyValue},
                 {"ssl_ca",       sslCaValue},
                 {options_,       QVariantHash({{SSL_CERT, sslCertValue},
                                                // Here will be uppercase
                                                {SSL_KEY,  sslKeyValue},
                                                {SSL_CA,   sslCaValue}})},
             }));

    // Restore
    QVERIFY(Databases::removeConnection(*connectionName));
}

void tst_DatabaseManager::ssl_MariaDB_ConfigurationValues() const
{
    /* Also the MariaDB SSL-related configuration options should behave the same
       as for the MySQL connection. */

    // Prepare SSL configuration
    // Top level
    const auto sslCertValue = QStringLiteral("C:/example/maria-cert.pem");
    const auto sslKeyValue  = QStringLiteral("C:/example/maria-key.pem");
    const auto sslCaValue   = QStringLiteral("C:/example/maria-ca.pem");
    // The 'options' level
    const auto sslCertOptionsValue = QStringLiteral("D:/example/client-cert.pem");
    const auto sslKeyOptionsValue  = QStringLiteral("D:/example/client-key.pem");

    const QVariantHash initialConfiguration {
        {driver_,   QMYSQL},
        // The ssl_cert is only alias to the "SSL_CERT", looks nicer
        {ssl_cert,  sslCertValue},
        // Lowercase SSL-related option names in top-level configuration
        {"ssl_key", sslKeyValue},
        {"ssl_ca",  sslCaValue},
        {options_,  QVariantHash({{SSL_CERT, sslCertOptionsValue},
                                  {SSL_KEY,  sslKeyOptionsValue}})}
    };

    // Add a new database connection
    const auto connectionName = Databases::createConnectionTemp(
                                    Databases::MARIADB,
                                    {ClassName, QString::fromUtf8(__func__)}, // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
                                    initialConfiguration);

    if (!connectionName)
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::MARIADB)
              .toUtf8().constData(), );

    // Original configuration
    // Connection isn't created and configuration options are not parsed yet
    const auto &originalConfig = m_dm->originalConfig(*connectionName);
    QCOMPARE(originalConfig, initialConfiguration);

    /* Force the creation of a connection and parse the connection configuration options.
       The qt_timezone option is only parsed in the connection configuration,
       the original configuration is untouched. */
    m_dm->connection(*connectionName);
    QCOMPARE(originalConfig,
             QVariantHash({
                 {driver_,        QMYSQL},
                 {NAME,           *connectionName},
                 {database_,      EMPTY},
                 {prefix_,        EMPTY},
                 {prefix_indexes, false},
                 {Version,        {}},
                 // The ssl_cert is only alias to the "SSL_CERT", looks nicer
                 {ssl_cert,       sslCertValue},
                 // Lowercase SSL-related option names in top-level configuration
                 {"ssl_key",      sslKeyValue},
                 {"ssl_ca",       sslCaValue},
                 {options_,       QVariantHash({{SSL_CERT, sslCertOptionsValue},
                                                {SSL_KEY,  sslKeyOptionsValue}})},
             }));

    // Connection configuration
    QCOMPARE(m_dm->getConfig(*connectionName),
             QVariantHash({
                 {driver_,        QMYSQL},
                 {NAME,           *connectionName},
                 {database_,      EMPTY},
                 {prefix_,        EMPTY},
                 {prefix_indexes, false},
                 {Version,        {}},
                 {qt_timezone,    QVariant::fromValue(
                                      QtTimeZoneConfig {QtTimeZoneType::DontConvert, {}}
                                  )},
                 // The ssl_cert is only alias to the "SSL_CERT", looks nicer
                 {ssl_cert,       sslCertValue},
                 // Lowercase SSL-related option names in top-level configuration
                 {"ssl_key",      sslKeyValue},
                 {"ssl_ca",       sslCaValue},
                 {options_,       QVariantHash({{SSL_CERT, sslCertValue},
                                                // Here will be uppercase
                                                {SSL_KEY,  sslKeyValue},
                                                {SSL_CA,   sslCaValue}})},
             }));

    // Restore
    QVERIFY(Databases::removeConnection(*connectionName));
}

void tst_DatabaseManager::ssl_PostgreSQL_ConfigurationValues() const
{
    // Prepare SSL configuration
    // Top level
    const auto sslmodeValue        = verify_full;
    const auto sslcertValue        = QStringLiteral("C:/example/psql.crt");
    const auto sslkeyValue         = QStringLiteral("C:/example/psql.key");
    const auto sslrootcertValue    = QStringLiteral("C:/example/ca.crt");
    // The 'options' level
    const auto sslcertOptionsValue = QStringLiteral("D:/example/pg.crt");
    const auto sslkeyOptionsValue  = QStringLiteral("D:/example/pg.key");

    const QVariantHash initialConfiguration({
        {driver_,     QPSQL},
        {sslmode_,    sslmodeValue},
        {sslcert,     sslcertValue},
        {sslkey,      sslkeyValue},
        {sslrootcert, sslrootcertValue},
        {options_,    QVariantHash({{sslcert, sslcertOptionsValue},
                                    {sslkey,  sslkeyOptionsValue}})}
    });
    // Add a new database connection
    const auto connectionName = Databases::createConnectionTemp(
                                    Databases::POSTGRESQL,
                                    {ClassName, QString::fromUtf8(__func__)}, // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
                                    initialConfiguration);

    if (!connectionName)
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::POSTGRESQL)
              .toUtf8().constData(), );

    // Original configuration
    // Connection isn't created and configuration options are not parsed yet
    const auto &originalConfig = m_dm->originalConfig(*connectionName);
    QCOMPARE(originalConfig, initialConfiguration);

    /* Force the creation of a connection and parse the connection configuration options.
       The SSL-related options are only parsed in the connection configuration,
       the original configuration is untouched. */
    m_dm->connection(*connectionName);
    QCOMPARE(originalConfig,
             QVariantHash({
                 {driver_,        QPSQL},
                 {NAME,           *connectionName},
                 {database_,      EMPTY},
                 {prefix_,        EMPTY},
                 {prefix_indexes, false},
                 {options_,       QVariantHash()},
                 {dont_drop,      QStringList {spatial_ref_sys}},
                 {sslmode_,       sslmodeValue},
                 {sslcert,        sslcertValue},
                 {sslkey,         sslkeyValue},
                 {sslrootcert,    sslrootcertValue},
                 {options_,       QVariantHash({{sslcert, sslcertOptionsValue},
                                                {sslkey,  sslkeyOptionsValue}})}
             }));

    // Connection configuration
    QCOMPARE(m_dm->getConfig(*connectionName),
             QVariantHash({
                 {driver_,        QPSQL},
                 {NAME,           *connectionName},
                 {database_,      EMPTY},
                 {prefix_,        EMPTY},
                 {prefix_indexes, false},
                 {options_,       QVariantHash()},
                 {dont_drop,      QStringList {spatial_ref_sys}},
                 {qt_timezone,    QVariant::fromValue(
                                      QtTimeZoneConfig {QtTimeZoneType::DontConvert, {}}
                                  )},
                 {sslmode_,       sslmodeValue},
                 {sslcert,        sslcertValue},
                 {sslkey,         sslkeyValue},
                 {sslrootcert,    sslrootcertValue},
                 {options_,       QVariantHash({{sslmode_,    sslmodeValue},
                                                {sslcert,     sslcertValue},
                                                {sslkey,      sslkeyValue},
                                                {sslrootcert, sslrootcertValue}})}
             }));

    // Restore
    QVERIFY(Databases::removeConnection(*connectionName));
}

void tst_DatabaseManager::sqlite_MemoryDriver() const
{
    // Add a new database connection
    const auto connectionName = Databases::createConnectionTemp(
                                    Databases::SQLITE,
                                    {ClassName, QString::fromUtf8(__func__)}, // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    {
        {driver_,   QSQLITE},
        {database_, QStringLiteral(":memory:")},
    });

    if (!connectionName)
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::SQLITE)
              .toUtf8().constData(), );

    auto &connection = m_dm->connection(*connectionName);

    // Create the database and insert some records
    connection.statement("create table tbl1 (one varchar(10), two smallint)");
    connection.insert("insert into tbl1 values(?, ?)", {"hello!", 10});
    connection.insert("insert into tbl1 values(?, ?)", {"goodbye", 20});

    auto query = connection.selectOne("select * from tbl1 where two = ?", {10});

    // Verify
    QVERIFY(query.isValid());
    QCOMPARE(query.value("one").value<QString>(), QStringLiteral("hello!"));
    QCOMPARE(query.value("two").value<int>(), 10);

    // Restore
    QVERIFY(Databases::removeConnection(*connectionName));
}

void tst_DatabaseManager::sqlite_CheckDatabaseExists_True() const
{
    // Add a new database connection
    const auto connectionName = Databases::createConnectionTemp(
                                    Databases::SQLITE,
                                    {ClassName, QString::fromUtf8(__func__)}, // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    {
        {driver_,               QSQLITE},
        {database_,             checkDatabaseExistsFile()},
        {check_database_exists, true},
    });

    if (!connectionName)
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::SQLITE)
              .toUtf8().constData(), );

    // Verify
    QVERIFY_EXCEPTION_THROWN(
                m_dm->connection(*connectionName)
                .statement("create table tbl1 (one varchar(10), two smallint)"),
                SQLiteDatabaseDoesNotExistError);

    // Restore
    QVERIFY(Databases::removeConnection(*connectionName));
}

void tst_DatabaseManager::sqlite_CheckDatabaseExists_False() const
{
    // Add a new database connection
    const auto connectionName = Databases::createConnectionTemp(
                                    Databases::SQLITE,
                                    {ClassName, QString::fromUtf8(__func__)}, // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    {
        {driver_,               QSQLITE},
        {database_,             checkDatabaseExistsFile()},
        {check_database_exists, false},
    });

    if (!connectionName)
        QSKIP(TestUtils::AutoTestSkipped
              .arg(TypeUtils::classPureBasename(*this), Databases::SQLITE)
              .toUtf8().constData(), );

    // Verify
    // QSqlDatabase automatically creates a SQLite database file
    m_dm->connection(*connectionName)
            .statement("create table tbl1 (one varchar(10), two smallint)");

    QVERIFY(QFile::exists(checkDatabaseExistsFile()));

    // Restore
    QVERIFY(Databases::removeConnection(*connectionName));

    // Remove the SQLite database file
    QVERIFY(QFile::remove(checkDatabaseExistsFile()));
    QVERIFY(!QFile::exists(checkDatabaseExistsFile()));
}

void tst_DatabaseManager::addUseAndRemoveConnection_FiveTimes() const
{
    for (auto i = 0; i < 5; ++i) {
        // Add a new MYSQL database connection
        const auto connectionName =
                Databases::createConnectionTempFrom(
                    Databases::MYSQL, {ClassName, QString::fromUtf8(__func__)}); // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)

        if (i == 0 && !connectionName)
            QSKIP(TestUtils::AutoTestSkipped
                  .arg(TypeUtils::classPureBasename(*this), Databases::MYSQL)
                  .toUtf8().constData(), );

        // Execute some database query
        QCOMPARE(m_dm->table("users", *connectionName)->count(), 5);

        // Restore
        QVERIFY(Databases::removeConnection(*connectionName));
    }
}
// NOLINTEND(readability-convert-member-functions-to-static)

/* private */

const QString &tst_DatabaseManager::checkDatabaseExistsFile()
{
    static const auto cached = []() -> QString
    {
        auto databasePath = qEnvironmentVariable("DB_SQLITE_DATABASE", EMPTY);

        /* Return EMPTY, the Databases::createConnectionTemp() will check env. variable
           and QSKIP() will be called if it's undefined. */
        if (databasePath.isEmpty())
            return EMPTY;

        databasePath.truncate(QDir::fromNativeSeparators(databasePath)
                              .lastIndexOf(QChar('/')));

        return databasePath + "/tinyorm_test-check_exists.sqlite3";
    }();

    return cached;
}

QTEST_MAIN(tst_DatabaseManager)

#include "tst_databasemanager.moc"
