#include <QCoreApplication>
#include <QtTest>

#include "orm/constants.hpp"
#include "orm/databasemanager.hpp"

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
using Orm::Constants::schema_;
using Orm::Constants::spatial_ref_sys;
using Orm::Constants::sslcert;
using Orm::Constants::sslkey;
using Orm::Constants::sslmode_;
using Orm::Constants::sslrootcert;
using Orm::Constants::username_;
using Orm::Constants::verify_full;

using Orm::DatabaseManager;
using Orm::QtTimeZoneConfig;
using Orm::QtTimeZoneType;
using Orm::Support::DatabaseConfiguration;

class tst_DatabaseManager : public QObject // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();

    void removeConnection_Connected() const;
    void removeConnection_NotConnected() const;

    void default_MySQL_ConfigurationValues() const;
    void default_PostgreSQL_ConfigurationValues() const;
    void default_SQLite_ConfigurationValues() const;

    void ssl_MySQL_ConfigurationValues() const;
    void ssl_PostgreSQL_ConfigurationValues() const;

// NOLINTNEXTLINE(readability-redundant-access-specifiers)
private:
    /*! The Database Manager used in this test case. */
    std::shared_ptr<DatabaseManager> m_dm {};
};

/* private slots */

// NOLINTBEGIN(readability-convert-member-functions-to-static)
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
            {driver_,          driverName},
            {application_name, QStringLiteral("TinyORM tests - tst_databasemanager")},
            {host_,            qEnvironmentVariable("DB_PGSQL_HOST", H127001)},
            {port_,            qEnvironmentVariable("DB_PGSQL_PORT", P5432)},
            {database_,        databaseName},
            {schema_,          qEnvironmentVariable("DB_PGSQL_SCHEMA", PUBLIC)},
            {username_,        qEnvironmentVariable("DB_PGSQL_USERNAME",
                                                    QStringLiteral("postgres"))},
            {password_,        qEnvironmentVariable("DB_PGSQL_PASSWORD", "")},
            {charset_,         qEnvironmentVariable("DB_PGSQL_CHARSET", UTF8)},
        }},
    // Don't setup any default connection
    }, EMPTY);

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

void tst_DatabaseManager::default_MySQL_ConfigurationValues() const
{
    const auto connectionName =
            QStringLiteral(
                "tinyorm_mysql_tests-tst_DatabaseMannager-"
                "default_MySQL_ConfigurationValues");

    // Create database connection
    m_dm->addConnections({
        {connectionName, {
            {driver_, "qmysql"},
        }},
    // Don't setup any default connection
    }, EMPTY);

    // Original configuration
    // Connection isn't created and configuration options are not parsed yet
    const auto &originalConfig = m_dm->originalConfig(connectionName);
    QCOMPARE(originalConfig,
             QVariantHash({
                 {driver_, "qmysql"},
             }));

    /* Force the creation of a connection and parse the connection configuration options.
       The qt_timezone option is only parsed in the connection configuration,
       the original configuration is untouched. */
    m_dm->connection(connectionName);
    QCOMPARE(originalConfig,
             QVariantHash({
                 {driver_,        QMYSQL},
                 {NAME,           connectionName},
                 {database_,      EMPTY},
                 {prefix_,        EMPTY},
                 {prefix_indexes, false},
                 {options_,       QVariantHash()},
                 {Version,        {}},
             }));

    // Connection configuration
    QCOMPARE(m_dm->getConfig(connectionName),
             QVariantHash({
                 {driver_,        QMYSQL},
                 {NAME,           connectionName},
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
    QVERIFY(m_dm->removeConnection(connectionName));
}

void tst_DatabaseManager::default_PostgreSQL_ConfigurationValues() const
{
    const auto connectionName =
            QStringLiteral(
                "tinyorm_pgsql_tests-tst_DatabaseMannager-"
                "default_PostgreSQL_ConfigurationValues");

    // Create database connection
    m_dm->addConnections({
        {connectionName, {
            {driver_, "qpsql"},
        }},
    // Don't setup any default connection
    }, EMPTY);

    // Original configuration
    // Connection isn't created and configuration options are not parsed yet
    const auto &originalConfig = m_dm->originalConfig(connectionName);
    QCOMPARE(originalConfig,
             QVariantHash({
                 {driver_, "qpsql"},
             }));

    /* Force the creation of a connection and parse the connection configuration options.
       The qt_timezone option is only parsed in the connection configuration,
       the original configuration is untouched. */
    m_dm->connection(connectionName);
    QCOMPARE(originalConfig,
             QVariantHash({
                 {driver_,        QPSQL},
                 {NAME,           connectionName},
                 {database_,      EMPTY},
                 {prefix_,        EMPTY},
                 {prefix_indexes, false},
                 {options_,       QVariantHash()},
                 {dont_drop,      QStringList {spatial_ref_sys}},
             }));

    // Connection configuration
    QCOMPARE(m_dm->getConfig(connectionName),
             QVariantHash({
                 {driver_,        QPSQL},
                 {NAME,           connectionName},
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
    QVERIFY(m_dm->removeConnection(connectionName));
}

void tst_DatabaseManager::default_SQLite_ConfigurationValues() const
{
    const auto connectionName =
            QStringLiteral(
                "tinyorm_sqlite_tests-tst_DatabaseMannager-"
                "default_SQLite_ConfigurationValues");

    // Create database connection
    m_dm->addConnections({
        {connectionName, {
            {driver_, "qsqlite"},
        }},
    // Don't setup any default connection
    }, EMPTY);

    // Original configuration
    // Connection isn't created and configuration options are not parsed yet
    const auto &originalConfig = m_dm->originalConfig(connectionName);
    QCOMPARE(originalConfig,
             QVariantHash({
                 {driver_, "qsqlite"},
             }));

    /* Force the creation of a connection and parse the connection configuration options.
       The qt_timezone option is only parsed in the connection configuration,
       the original configuration is untouched. */
    m_dm->connection(connectionName);
    QCOMPARE(originalConfig,
             QVariantHash({
                 {driver_,          QSQLITE},
                 {NAME,             connectionName},
                 {database_,        EMPTY},
                 {prefix_,          EMPTY},
                 {prefix_indexes,   false},
                 {options_,         QVariantHash()},
                 {return_qdatetime, true},
             }));

    // Connection configuration
    QCOMPARE(m_dm->getConfig(connectionName),
             QVariantHash({
                 {driver_,          QSQLITE},
                 {NAME,             connectionName},
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
    QVERIFY(m_dm->removeConnection(connectionName));
}

void tst_DatabaseManager::ssl_MySQL_ConfigurationValues() const
{
    const auto connectionName =
            QStringLiteral(
                "tinyorm_mysql_tests-tst_DatabaseMannager-"
                "ssl_MySQL_ConfigurationValues");

    // Top level
    const auto sslCertValue = QStringLiteral("C:/example/mysql-cert.pem");
    const auto sslKeyValue  = QStringLiteral("C:/example/mysql-key.pem");
    const auto sslCaValue   = QStringLiteral("C:/example/mysql-ca.pem");
    // The 'options' level
    const auto sslCertOptionsValue = QStringLiteral("D:/example/client-cert.pem");
    const auto sslKeyOptionsValue  = QStringLiteral("D:/example/client-key.pem");

    const QVariantHash initialConfiguration({
        {driver_,  QMYSQL},
        {SSL_CERT, sslCertValue},
        {SSL_KEY,  sslKeyValue},
        {SSL_CA,   sslCaValue},
        {options_, QVariantHash({{SSL_CERT, sslCertOptionsValue},
                                 {SSL_KEY,  sslKeyOptionsValue}})}
    });

    // Create database connection
    m_dm->addConnections({
        {connectionName, initialConfiguration},
    // Don't setup any default connection
    }, EMPTY);

    // Original configuration
    // Connection isn't created and configuration options are not parsed yet
    const auto &originalConfig = m_dm->originalConfig(connectionName);
    QCOMPARE(originalConfig, initialConfiguration);

    /* Force the creation of a connection and parse the connection configuration options.
       The qt_timezone option is only parsed in the connection configuration,
       the original configuration is untouched. */
    m_dm->connection(connectionName);
    QCOMPARE(originalConfig,
             QVariantHash({
                 {driver_,        QMYSQL},
                 {NAME,           connectionName},
                 {database_,      EMPTY},
                 {prefix_,        EMPTY},
                 {prefix_indexes, false},
                 {Version,        {}},
                 {SSL_CERT,       sslCertValue},
                 {SSL_KEY,        sslKeyValue},
                 {SSL_CA,         sslCaValue},
                 {options_,       QVariantHash({{SSL_CERT, sslCertOptionsValue},
                                                {SSL_KEY,  sslKeyOptionsValue}})},
             }));

    // Connection configuration
    QCOMPARE(m_dm->getConfig(connectionName),
             QVariantHash({
                 {driver_,        QMYSQL},
                 {NAME,           connectionName},
                 {database_,      EMPTY},
                 {prefix_,        EMPTY},
                 {prefix_indexes, false},
                 {Version,        {}},
                 {qt_timezone,    QVariant::fromValue(
                                      QtTimeZoneConfig {QtTimeZoneType::DontConvert, {}}
                                  )},
                 {SSL_CERT,       sslCertValue},
                 {SSL_KEY,        sslKeyValue},
                 {SSL_CA,         sslCaValue},
                 {options_,       QVariantHash({{SSL_CERT, sslCertValue},
                                                {SSL_KEY,  sslKeyValue},
                                                {SSL_CA,   sslCaValue}})},
             }));

    // Restore
    QVERIFY(m_dm->removeConnection(connectionName));
}

void tst_DatabaseManager::ssl_PostgreSQL_ConfigurationValues() const
{
    const auto connectionName =
            QStringLiteral(
                "tinyorm_pgsql_tests-tst_DatabaseMannager-"
                "ssl_PostgreSQL_ConfigurationValues");

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

    // Create database connection
    m_dm->addConnections({
        {connectionName, initialConfiguration},
    // Don't setup any default connection
    }, EMPTY);

    // Original configuration
    // Connection isn't created and configuration options are not parsed yet
    const auto &originalConfig = m_dm->originalConfig(connectionName);
    QCOMPARE(originalConfig, initialConfiguration);

    /* Force the creation of a connection and parse the connection configuration options.
       The SSL-related options are only parsed in the connection configuration,
       the original configuration is untouched. */
    m_dm->connection(connectionName);
    QCOMPARE(originalConfig,
             QVariantHash({
                 {driver_,        QPSQL},
                 {NAME,           connectionName},
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
    QCOMPARE(m_dm->getConfig(connectionName),
             QVariantHash({
                 {driver_,        QPSQL},
                 {NAME,           connectionName},
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
    QVERIFY(m_dm->removeConnection(connectionName));
}
// NOLINTEND(readability-convert-member-functions-to-static)

QTEST_MAIN(tst_DatabaseManager)

#include "tst_databasemanager.moc"
