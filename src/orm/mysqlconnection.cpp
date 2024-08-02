#include "orm/mysqlconnection.hpp"

#ifdef TINYORM_MYSQL_PING
#  include <QDebug>
#endif
#include <QVersionNumber>

#ifdef TINYORM_MYSQL_PING
#  include TINY_INCLUDE_TSqlDriver

#  ifdef __MINGW32__
#    include <mysql/errmsg.h>
#  elif __has_include(<errmsg.h>)
#    include <errmsg.h>
#  endif
#  if __has_include(<mysql/mysql.h>)
#    include <mysql/mysql.h>
#  elif __has_include(<mysql.h>)
#    include <mysql.h>
#  else
#    error Can not find the <mysql.h> header file, install the MySQL C client library or \
disable the TINYORM_MYSQL_PING preprocessor directive.
#  endif
#endif

#include "orm/query/grammars/mysqlgrammar.hpp"
#include "orm/query/processors/mysqlprocessor.hpp"
#include "orm/schema/grammars/mysqlschemagrammar.hpp"
#include "orm/schema/mysqlschemabuilder.hpp"
#include "orm/utils/configuration.hpp"

// Revisited, correct (comes from the TINY_INCLUDE_TQueryError)
#ifdef TINYORM_USING_TINYDRIVERS
#  include "orm/exceptions/runtimeerror.hpp"
#endif

TINYORM_BEGIN_COMMON_NAMESPACE

using ConfigUtils = Orm::Utils::Configuration;

namespace Orm
{

/* private */

MySqlConnection::MySqlConnection(
        std::function<Connectors::ConnectionName()> &&connection,
        QString &&database, QString &&tablePrefix, QtTimeZoneConfig &&qtTimeZone,
        QVariantHash &&config
)
    : DatabaseConnection(
          std::move(connection), std::move(database), std::move(tablePrefix),
          std::move(qtTimeZone), std::move(config))
    , m_isMaria(std::nullopt)
{
    /* We need to initialize a query grammar that is a very important part
       of the database abstraction, so we initialize it to the default value
       while starting. */
    useDefaultQueryGrammar();

    useDefaultPostProcessor();
}

/* public */

/* Getters / Setters */

std::optional<QString> MySqlConnection::version()
{
    auto versionConfig = ConfigUtils::getValidConfigVersion(m_config);
    const auto isVersionConfigEmpty = versionConfig.isEmpty();

    /* The default value is the std::nullopt if pretending and the database configuration
       doesn't contain a valid version value. */
    if (m_pretending && !m_version && isVersionConfigEmpty)
        return std::nullopt;

    // Return the cached value
    if (m_version)
        return m_version;

    // A user can provide the version through the configuration to save one DB query
    if (!isVersionConfigEmpty)
        return m_version = std::move(versionConfig);

    // Obtain and cache the database version value
    return m_version = selectOne(QStringLiteral("select version()")).value(0)
                       .value<QString>();
}

bool MySqlConnection::isMaria()
{
    // Default values is false if pretending and the config. version was not set manually
    if (m_pretending && !m_isMaria && !m_version &&
        !ConfigUtils::hasValidConfigVersion(m_config)
    )
        return false;

    // Return the cached value
    if (m_isMaria)
        return *m_isMaria;

    // Obtain a version from the database if needed
    version();

    // This should never happen 🤔 because of the condition at beginning
    if (!m_version)
        return false;

    // Cache the value
    m_isMaria = m_version->contains(QStringLiteral("MariaDB"));

    return *m_isMaria;
}

bool MySqlConnection::useUpsertAlias()
{
    // Default values is true if pretending and the config. version was not set manually
    if (m_pretending && !m_useUpsertAlias && !m_version &&
        !ConfigUtils::hasValidConfigVersion(m_config)
    )
        // FUTURE useUpsertAlias() default value to true after MySQL 8.0 will be end-of-life silverqx
        return false;

    // Return the cached value
    if (m_useUpsertAlias)
        return *m_useUpsertAlias;

    // Obtain a version from the database if needed
    version();

    // This should never happen 🤔 because of the condition at beginning
    if (!m_version)
        return false;

    /* The MySQL >=8.0.19 supports aliases in the VALUES and SET clauses
       of INSERT INTO ... ON DUPLICATE KEY UPDATE statement for the row to be
       inserted and its columns. It also generates warning if old style used.
       So set it to true to avoid this warning.

       Currently, the MariaDB 11.0.1 doesn't support aliases in the VALUES and
       SET clauses, so the upsert alias is always disabled (false). */

    // Cache the value
    // MariaDB doesn't support the upsert alias
    if (isMaria())
        m_useUpsertAlias = false;

    // MySQL >=8.0.19 does support the upsert alias, so force to use it
    else
        m_useUpsertAlias = QVersionNumber::fromString(*m_version) >=
                           QVersionNumber(8, 0, 19);

    return *m_useUpsertAlias;
}

#ifdef TINYORM_TESTS_CODE
void MySqlConnection::setConfigVersion(const QString &value)
{
    // Override it through the config., it ensures that more code branches will be tested
    m_config.insert(Version, value);

    // We need to reset these to recomputed them again
    m_version = std::nullopt;
    m_isMaria = std::nullopt;
    m_useUpsertAlias = std::nullopt;
}
#endif

/* Others */

bool MySqlConnection::pingDatabase()
{
#ifdef TINYORM_MYSQL_PING
    auto qtConnection = getSqlConnection();

    const auto getDriverHandle = [&qtConnection = std::as_const(qtConnection)]()
                                 -> QVariant
    {
#ifdef TINYORM_USING_QTSQLDRIVERS
        return qtConnection.driver()->handle();
#elif defined(TINYORM_USING_TINYDRIVERS)
        return qtConnection.driverWeak().lock()->handle();
#else
#  error Missing include "orm/macros/sqldrivermappings.hpp".
#endif
    };

    const auto getMysqlHandle = [&getDriverHandle]() -> MYSQL *
    {
        auto driverHandle = std::invoke(getDriverHandle);
        const auto *typeName = driverHandle.typeName();

        // MYSQL* for MySQL client and st_mysql* for MariaDB client
        if (qstrcmp(typeName, "MYSQL*") == 0 || qstrcmp(typeName, "st_mysql*") == 0)
            return *static_cast<MYSQL **>(driverHandle.data());

        return nullptr;
    };

    const auto mysqlPing = [&getMysqlHandle]() -> bool
    {
        auto *mysqlHandle = std::invoke(getMysqlHandle);
        if (mysqlHandle == nullptr)
            return false;

        const auto ping = mysql_ping(mysqlHandle);
        const auto errNo = mysql_errno(mysqlHandle);

        /* So strange logic, because I want interpret CR_COMMANDS_OUT_OF_SYNC errno as
           successful ping. */
        if (ping != 0 && errNo == CR_COMMANDS_OUT_OF_SYNC) {
            // TODO log to file, how often this happen silverqx
            qWarning("mysql_ping() returned : CR_COMMANDS_OUT_OF_SYNC(%ud)", errNo); // NOLINT(cppcoreguidelines-pro-type-vararg)
            return true;
        }

        // Connection is alive
        if (ping == 0)
            return true;

        qWarning("Unknown behavior during mysql_ping(), this should never happen :/"); // NOLINT(cppcoreguidelines-pro-type-vararg)
        return false;
    };

    if (qtConnection.isOpen() && std::invoke(mysqlPing)) {
        logConnected();
        return true;
    }

    // The database connection was lost
    logDisconnected();

    // Database connection have to be closed manually
    // isOpen() check is called in MySQL driver
    qtConnection.close();

    // Reset in transaction state and the savepoints counter
    resetTransactions();

    return false;
#else
    throw Exceptions::RuntimeError(
                QStringLiteral(
                    "pingDatabase() method was disabled for the '%1' database driver, "
                    "if you want to use MySqlConnection::pingDatabase(), then "
                    "reconfigure the TinyORM project with the MYSQL_PING preprocessor "
                    "macro ( -DMYSQL_PING ) for cmake or with the 'mysql_ping' "
                    "configuration option ( \"CONFIG+=mysql_ping\" ) for qmake.")
                .arg(driverName()));
#endif
}

/* protected */

std::unique_ptr<QueryGrammar> MySqlConnection::getDefaultQueryGrammar() const
{
    // Ownership of a unique_ptr()
    auto grammar = std::make_unique<Query::Grammars::MySqlGrammar>();

    withTablePrefix(*grammar);

    return grammar;
}

std::unique_ptr<SchemaGrammar> MySqlConnection::getDefaultSchemaGrammar()
{
    // Ownership of a unique_ptr()
    auto grammar = std::make_unique<SchemaNs::Grammars::MySqlSchemaGrammar>(isMaria());

    withTablePrefix(*grammar);

    return grammar;
}

std::unique_ptr<SchemaBuilder> MySqlConnection::getDefaultSchemaBuilder()
{
    return std::make_unique<SchemaNs::MySqlSchemaBuilder>(shared_from_this());
}

std::unique_ptr<QueryProcessor> MySqlConnection::getDefaultPostProcessor() const
{
    return std::make_unique<Query::Processors::MySqlProcessor>();
}

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE
