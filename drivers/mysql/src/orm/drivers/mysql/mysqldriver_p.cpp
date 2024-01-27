#include "orm/drivers/mysql/mysqldriver_p.hpp"

#include "orm/drivers/exceptions/invalidargumenterror.hpp"
#include "orm/drivers/mysql/mysqlconstants_p.hpp"
#include "orm/drivers/mysql/mysqldriver.hpp"
#include "orm/drivers/mysql/mysqlutils_p.hpp"
#include "orm/drivers/utils/type_p.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using namespace Qt::StringLiterals; // NOLINT(google-build-using-namespace)

using Orm::Drivers::MySql::Constants::EQ_C;
using Orm::Drivers::MySql::Constants::SEMICOLON;

using MySqlUtils = Orm::Drivers::MySql::MySqlUtilsPrivate;

namespace Orm::Drivers::MySql
{

/* public */

/* open() */

bool MySqlDriverPrivate::mysqlInit()
{
    Q_Q(MySqlDriver);

    if (mysql = mysql_init(nullptr); mysql != nullptr)
        return true;

    return q->setLastOpenError(
                MySqlUtils::createError(
                    u"Unable to allocate and initialize the MYSQL object"_s,
                    SqlError::ConnectionError, mysql));
}

MySqlDriverPrivate::SetConnectionOptionsResult
MySqlDriverPrivate::mysqlSetConnectionOptions(const QString &options)
{
    /* Enable the MySQL's stored procedures support.
       Needed to enable CLIEN_MULTI_STATEMENTS (which also enables CLIENT_MULTI_RESULTS)
       because stored procedures can return multiple result sets, otherwise, any
       stored procedure call will fail. */
    uint optionFlags = CLIENT_MULTI_STATEMENTS;
    QString unixSocket;

    // Split a raw connection options string using the ; character
    for (auto &&optionRaw : QStringView(options).split(SEMICOLON, Qt::SkipEmptyParts)) {
        // Parse the given MySQL connection option to name and value
        auto &&[option, value] = parseMySqlOption(optionRaw);

        // Set the given MySQL connection option
        if (mysqlSetConnectionOption(option, *value))
            continue;

        /* If the connection option has no value or ends with the TRUE-like boolean
           keywords (true, on, 1), then treat it as a flag. */
        if (!value || isTrueBoolOption(*value))
            setOptionFlag(optionFlags, option);

        else if (option == "UNIX_SOCKET"_L1) // _L1 is correct here
            unixSocket = value->toString();

        else
            throw Exceptions::InvalidArgumentError(
                    u"Failed to set MySQL connection option value '%1' in %2()."_s
                    .arg(optionRaw.trimmed(), __tiny_func__));
    }

    return {optionFlags, std::move(unixSocket)};
}

bool MySqlDriverPrivate::mysqlSetCharacterSet(const QString &host, const bool before)
{
    Q_Q(MySqlDriver);

    for (const auto *const csname : DefaultCharacterSets)
        if (mysql_set_character_set(mysql, csname) == 0)
            return true;

    const auto message =
            before ? u"before establishing a database connection to the '%1' host"_s
                   : u"for SQL statements for the '%1' host"_s;

    q->setLastOpenError(
                MySqlUtils::createError(
                    u"Unable to set the default character set %1"_s
                    .arg(message.arg(host)),
                    SqlError::ConnectionError, mysql));

    mysql_close(mysql);
    mysql = nullptr;

    return false;
}

bool MySqlDriverPrivate::mysqlRealConnect(
        const QString &host, const QByteArray &username, const QByteArray &password,
        const QByteArray &database, const int port, const QByteArray &unixSocket,
        const uint optionFlags)
{
    Q_Q(MySqlDriver);

    const auto hostArray = host.toUtf8();

    const auto *const mysqlToVerify =
            mysql_real_connect(
                mysql,
                toCharArray(hostArray),  toCharArray(username),
                toCharArray(password),   toCharArray(database), port > -1 ? port : 0,
                toCharArray(unixSocket), optionFlags);

    if (mysqlToVerify != nullptr && mysqlToVerify == mysql)
        return true;

    q->setLastOpenError(MySqlUtils::createError(
                            u"Unable to establish a connection to the MySQL server "
                             "running on the '%1' host."_s.arg(host),
                            SqlError::ConnectionError, mysql));

    mysql_close(mysql);
    mysql = nullptr;

    return false;
}

bool MySqlDriverPrivate::mysqlSelectDb(const QString &database)
{
    Q_Q(MySqlDriver);

    if (database.isEmpty() ||
        mysql_select_db(mysql, database.toUtf8().constData()) == 0
    ) {
        databaseName = database;
        return true;
    }

    q->setLastOpenError(MySqlUtils::createError(
                            u"Unable to select/open database '%1'"_s.arg(database),
                            SqlError::ConnectionError, mysql));
    mysql_close(mysql);
    mysql = nullptr;

    return false;
}

/* private */

/* open() */

MySqlDriverPrivate::MySqlOptionParsed
MySqlDriverPrivate::parseMySqlOption(const QStringView optionRaw)
{
    // Can contain 0 or 1 = character; 0 for flags and 1 for options with a value
    Q_ASSERT(!(optionRaw.count(EQ_C) > 1));

    const auto option = optionRaw.split(EQ_C);

    return {option.constFirst().trimmed(), option[1].trimmed()};
}

bool MySqlDriverPrivate::mysqlSetConnectionOption(const QStringView option,
                                                  const QStringView value)
{
    const auto &optionsHash = getMySqlOptionsHash();

    // Nothing to do
    if (!optionsHash.contains(option))
        return false;

    // Set the given option using predefined setOptionXyz() static method
    if (const auto &[mysqlOption, setMySqlOption] = optionsHash.at(option);
        std::invoke(setMySqlOption, mysql, mysqlOption, value)
    )
        return true;

    throw Exceptions::InvalidArgumentError(
                u"Failed to set MySQL '%1' connection option to '%2' value in %3()."_s
                .arg(option, value, __tiny_func__));
}

const MySqlDriverPrivate::MySqlOptionsHash &
MySqlDriverPrivate::getMySqlOptionsHash()
{
    /* The u""_s is correct here, don't use latin1 because we need to use
       the hash.contains(QStringView) because our option names are QStrinView-s
       after the split() method call. So this is only one solution. */
    static const MySqlOptionsHash cachedOptions = {
        {u"SSL_KEY"_s,                   {MYSQL_OPT_SSL_KEY,         setOptionString}},
        {u"SSL_CERT"_s,                  {MYSQL_OPT_SSL_CERT,        setOptionString}},
        {u"SSL_CA"_s,                    {MYSQL_OPT_SSL_CA,          setOptionString}},
        {u"SSL_CAPATH"_s,                {MYSQL_OPT_SSL_CAPATH,      setOptionString}},
        {u"SSL_CIPHER"_s,                {MYSQL_OPT_SSL_CIPHER,      setOptionString}},
        {u"MYSQL_OPT_SSL_KEY"_s,         {MYSQL_OPT_SSL_KEY,         setOptionString}},
        {u"MYSQL_OPT_SSL_CERT"_s,        {MYSQL_OPT_SSL_CERT,        setOptionString}},
        {u"MYSQL_OPT_SSL_CA"_s,          {MYSQL_OPT_SSL_CA,          setOptionString}},
        {u"MYSQL_OPT_SSL_CAPATH"_s,      {MYSQL_OPT_SSL_CAPATH,      setOptionString}},
        {u"MYSQL_OPT_SSL_CIPHER"_s,      {MYSQL_OPT_SSL_CIPHER,      setOptionString}},
        {u"MYSQL_OPT_SSL_CRL"_s,         {MYSQL_OPT_SSL_CRL,         setOptionString}},
        {u"MYSQL_OPT_SSL_CRLPATH"_s,     {MYSQL_OPT_SSL_CRLPATH,     setOptionString}},
#if defined(MYSQL_VERSION_ID) && MYSQL_VERSION_ID >= 50710
        {u"MYSQL_OPT_TLS_VERSION"_s,     {MYSQL_OPT_TLS_VERSION,     setOptionString}},
#endif
#if defined(MYSQL_VERSION_ID) && MYSQL_VERSION_ID >= 50711 && !defined(MARIADB_VERSION_ID)
        {u"MYSQL_OPT_SSL_MODE"_s,        {MYSQL_OPT_SSL_MODE,        setOptionSslMode}},
#endif
        {u"MYSQL_OPT_CONNECT_TIMEOUT"_s, {MYSQL_OPT_CONNECT_TIMEOUT, setOptionInt}},
        {u"MYSQL_OPT_READ_TIMEOUT"_s,    {MYSQL_OPT_READ_TIMEOUT,    setOptionInt}},
        {u"MYSQL_OPT_WRITE_TIMEOUT"_s,   {MYSQL_OPT_WRITE_TIMEOUT,   setOptionInt}},
        {u"MYSQL_OPT_RECONNECT"_s,       {MYSQL_OPT_RECONNECT,       setOptionBool}},
        {u"MYSQL_OPT_LOCAL_INFILE"_s,    {MYSQL_OPT_LOCAL_INFILE,    setOptionInt}},
        {u"MYSQL_OPT_PROTOCOL"_s,        {MYSQL_OPT_PROTOCOL,        setOptionProtocol}},
        {u"MYSQL_SHARED_MEMORY_BASE_NAME"_s,
                                         {MYSQL_SHARED_MEMORY_BASE_NAME,
                                                                     setOptionString}},
    };

    return cachedOptions;
}

void MySqlDriverPrivate::setOptionFlag(uint &optionFlags, const QStringView option)
{
    if (option == "CLIENT_COMPRESS"_L1)
        optionFlags |= CLIENT_COMPRESS;
    else if (option == "CLIENT_FOUND_ROWS"_L1)
        optionFlags |= CLIENT_FOUND_ROWS;
    else if (option == "CLIENT_IGNORE_SPACE"_L1)
        optionFlags |= CLIENT_IGNORE_SPACE;
    else if (option == "CLIENT_INTERACTIVE"_L1)
        optionFlags |= CLIENT_INTERACTIVE;
    else if (option == "CLIENT_NO_SCHEMA"_L1)
        optionFlags |= CLIENT_NO_SCHEMA;
    else if (option == "CLIENT_ODBC"_L1)
        optionFlags |= CLIENT_ODBC;
    else if (option == "CLIENT_SSL"_L1)
        throw Exceptions::InvalidArgumentError(
                u"The MYSQL_OPT_SSL_KEY, MYSQL_OPT_SSL_CERT, and MYSQL_OPT_SSL_CA "
                 "should be used instead of the CLIENT_SSL option in %1()."_s
                .arg(__tiny_func__));
    else
        throw Exceptions::InvalidArgumentError(
                u"Unknown MySQL connect option '%1' in %2()."_s
                .arg(option, __tiny_func__));
}

bool MySqlDriverPrivate::setOptionString(MYSQL *const mysql, const mysql_option option,
                                         const QStringView value)
{
    return mysql_options(mysql, option, value.toUtf8().constData()) == 0;
}

bool MySqlDriverPrivate::setOptionInt(MYSQL *const mysql, const mysql_option option,
                                      const QStringView value)
{
    auto ok = false;
    const auto intValue = value.toInt(&ok);

    return ok && mysql_options(mysql, option, &intValue) == 0;
}

bool MySqlDriverPrivate::setOptionBool(MYSQL *const mysql, const mysql_option option,
                                       const QStringView value)
{
    // CUR revisit silverqx
    const auto boolValue = value.isEmpty() || isTrueBoolOption(value);

    return mysql_options(mysql, option, &boolValue) == 0;
}

bool MySqlDriverPrivate::setOptionProtocol(MYSQL *const mysql, const mysql_option option,
                                           const QStringView value)
{
    mysql_protocol_type protocol = MYSQL_PROTOCOL_DEFAULT;

    if (value == "TCP"_L1 || value == "MYSQL_PROTOCOL_TCP"_L1)
        protocol = MYSQL_PROTOCOL_TCP;
    else if (value == "SOCKET"_L1 || value == "MYSQL_PROTOCOL_SOCKET"_L1)
        protocol = MYSQL_PROTOCOL_SOCKET;
    else if (value == "PIPE"_L1 || value == "MYSQL_PROTOCOL_PIPE"_L1)
        protocol = MYSQL_PROTOCOL_PIPE;
    else if (value == "MEMORY"_L1 || value == "MYSQL_PROTOCOL_MEMORY"_L1)
        protocol = MYSQL_PROTOCOL_MEMORY;
    else if (value == "DEFAULT"_L1 || value == "MYSQL_PROTOCOL_DEFAULT"_L1)
        protocol = MYSQL_PROTOCOL_DEFAULT;
    else
        throw Exceptions::InvalidArgumentError(
                u"Unknown MySQL connection transport protocol '%1' in %2()."_s
                .arg(value, __tiny_func__));

    return mysql_options(mysql, option, &protocol) == 0;
}

// The MYSQL_OPT_SSL_MODE was added in MySQL 5.7.11
#if defined(MYSQL_VERSION_ID) && MYSQL_VERSION_ID >= 50711 && !defined(MARIADB_VERSION_ID)
bool MySqlDriverPrivate::setOptionSslMode(MYSQL *const mysql, const mysql_option option,
                                          const QStringView value)
{
    mysql_ssl_mode sslMode = SSL_MODE_DISABLED;

    if (value == "DISABLED"_L1 || value == "SSL_MODE_DISABLED"_L1)
        sslMode = SSL_MODE_DISABLED;
    else if (value == "PREFERRED"_L1 || value == "SSL_MODE_PREFERRED"_L1)
        sslMode = SSL_MODE_PREFERRED;
    else if (value == "REQUIRED"_L1 || value == "SSL_MODE_REQUIRED"_L1)
        sslMode = SSL_MODE_REQUIRED;
    else if (value == "VERIFY_CA"_L1 || value == "SSL_MODE_VERIFY_CA"_L1)
        sslMode = SSL_MODE_VERIFY_CA;
    else if (value == "VERIFY_IDENTITY"_L1 || value == "SSL_MODE_VERIFY_IDENTITY"_L1)
        sslMode = SSL_MODE_VERIFY_IDENTITY;
    else
        throw Exceptions::InvalidArgumentError(
                u"Unknown MySQL SSL mode '%1' in %2()."_s
                .arg(value, __tiny_func__));

    return mysql_options(mysql, option, &sslMode) == 0;
}
#endif

} // namespace Orm::Drivers::MySql

TINYORM_END_COMMON_NAMESPACE
