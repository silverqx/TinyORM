#include "orm/drivers/mysql/mysqldriver_p.hpp"

#include "orm/drivers/exceptions/invalidargumenterror.hpp"
#include "orm/drivers/exceptions/sqlerror.hpp"
#include "orm/drivers/mysql/mysqlconstants_p.hpp"
#include "orm/drivers/mysql/mysqlutils_p.hpp"
#include "orm/drivers/utils/type_p.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using namespace Qt::StringLiterals; // NOLINT(google-build-using-namespace)

using Orm::Drivers::MySql::Constants::COMMA;
using Orm::Drivers::MySql::Constants::EQ_C;
using Orm::Drivers::MySql::Constants::SEMICOLON;

using MySqlUtils = Orm::Drivers::MySql::MySqlUtilsPrivate;

namespace Orm::Drivers::MySql
{

/* public */

/* open() */

void MySqlDriverPrivate::mysqlInit()
{
    if (mysql = mysql_init(nullptr); mysql != nullptr)
        return;

    throw Exceptions::SqlError(
                u"Unable to allocate and initialize the MYSQL handler object in %1()."_s
                .arg(__tiny_func__),
                MySqlUtils::prepareMySqlError(mysql));
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

namespace
{
    /*! Join all default character sets used for connection and SQL statements. */
    QString joinDefaultCharacterSets(const auto defaultCharacterSets)
    {
        QStringList result;
        result.reserve(defaultCharacterSets.size());

        for (const auto *const charset : defaultCharacterSets)
            // All character sets are always latin1 (SHOW CHARACTER SET)
            result << QString::fromLatin1(charset);

        return result.join(COMMA);
    }
} // namespace

void
MySqlDriverPrivate::mysqlSetCharacterSet(const QString &host, const bool before) const
{
#ifndef MARIADB_VERSION_ID
    static constexpr auto isMaria = false;
#else
    static constexpr auto isMaria = true;
#endif

    for (const auto *const characterSetName : DefaultCharacterSets)
        /* MySQL's mysql_set_character_set() is more advanced and is optimized to call it
           before the mysql_real_connect(), it can detect whether connection
           to the database exists and prepare charset based on it.
           On the other hand, MariaDB isn't able to do it this way and reports
           "Server has gone away" because it tries to execute "SET NAMES csname" query. */
        if (before && isMaria) {
            if (mysql_options(mysql, MYSQL_SET_CHARSET_NAME, characterSetName) == 0) // Must be as the separate if
                return;
        }
        else {
            if (mysql_set_character_set(mysql, characterSetName) == 0)
                return;
        }

    const auto messageTmpl =
            before ? u"before establishing a database connection to the '%1' host"_s
                   : u"for SQL statements for the '%1' host"_s;

    throw Exceptions::SqlError(
                u"Unable to set the default character sets [%1] %2 in %3()."_s
                .arg(joinDefaultCharacterSets(DefaultCharacterSets),
                     messageTmpl.arg(host), __tiny_func__),
                MySqlUtils::prepareMySqlError(mysql));
}

void MySqlDriverPrivate::mysqlRealConnect(
        const QString &host, const QByteArray &username, const QByteArray &password,
        const QByteArray &database, const int port, const QByteArray &unixSocket,
        const uint optionFlags) const
{
    const auto hostArray = host.toUtf8();

    const auto *const
    mysqlAferConnect = mysql_real_connect(
                           mysql,
                           toCharArray(hostArray),  toCharArray(username),
                           toCharArray(password),   toCharArray(database),
                           port > -1 ? static_cast<uint>(port) : 0,
                           toCharArray(unixSocket), optionFlags);

    if (mysqlAferConnect != nullptr && mysqlAferConnect == mysql)
        return;

    throw Exceptions::SqlError(
                u"Unable to establish a connection to the MySQL server "
                 "running on the '%1' host in %2()."_s
                .arg(host, __tiny_func__),
                MySqlUtils::prepareMySqlError(mysql));
}

void MySqlDriverPrivate::mysqlSelectDb(const QString &database)
{
    // Nothing to do
    if (database.isEmpty())
        return databaseName.clear(); // clazy:exclude=returning-void-expression

    if (mysql_select_db(mysql, database.toUtf8().constData()) == 0) {
        databaseName = database;
        return;
    }

    throw Exceptions::SqlError(
                u"Unable to select/open database '%1' in %2()."_s
                .arg(database, __tiny_func__),
                MySqlUtils::prepareMySqlError(mysql));
}

void MySqlDriverPrivate::mysqlClose() noexcept
{
    // Deallocate the connection handler
    mysql_close(mysql);
    mysql = nullptr;

    /* Unlike mysql_thread_init(), mysql_thread_end() isn't invoked automatically if
       the thread ends. It must be called explicitly to avoid memory leaks.
       It can or should be called after the mysql_close() because it's supposed to be
       called before the thread ends and not before or after the connection closes.
       Even calling it here isn't at 100% right, it should be called before the current
       thread ends. */
    mysql_thread_end();
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

    // Nothing to do, also don't throw exception here (an exception will be thrown later)
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
       the hash.contains(QStringView) as our option names are QStrinView-s
       after the split() method call. So this is the best solution. */
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
        {u"MYSQL_OPT_CONNECT_TIMEOUT"_s, {MYSQL_OPT_CONNECT_TIMEOUT, setOptionUInt}},
        {u"MYSQL_OPT_READ_TIMEOUT"_s,    {MYSQL_OPT_READ_TIMEOUT,    setOptionUInt}},
        {u"MYSQL_OPT_WRITE_TIMEOUT"_s,   {MYSQL_OPT_WRITE_TIMEOUT,   setOptionUInt}},
        {u"MYSQL_OPT_RECONNECT"_s,       {MYSQL_OPT_RECONNECT,       setOptionBool}},
        {u"MYSQL_OPT_LOCAL_INFILE"_s,    {MYSQL_OPT_LOCAL_INFILE,    setOptionUInt}},
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
                 "must be used instead of the CLIENT_SSL option (the CLIENT_SSL is set "
                 "internally in the client library), in %1()."_s
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

bool MySqlDriverPrivate::setOptionUInt(MYSQL *const mysql, const mysql_option option,
                                       const QStringView value)
{
    auto ok = false;
    const auto intValue = value.toUInt(&ok);

    return ok && mysql_options(mysql, option, &intValue) == 0;
}

bool MySqlDriverPrivate::setOptionBool(MYSQL *const mysql, const mysql_option option,
                                       const QStringView value) noexcept
{
    // Log warnings to the console for some boolean connection options
    logBoolOptionWarnings(option);

    // Revisited, an empty value is considered as true so it's a kind of flag
    const auto boolValue = value.isEmpty() || isTrueBoolOption(value);

    return mysql_options(mysql, option, &boolValue) == 0;
}

bool MySqlDriverPrivate::setOptionProtocol(MYSQL *const mysql, const mysql_option option,
                                           const QStringView value)
{
    const auto protocol = getOptionProtocol(value);

    return mysql_options(mysql, option, &protocol) == 0;
}

mysql_protocol_type MySqlDriverPrivate::getOptionProtocol(const QStringView value)
{
    if (value == "TCP"_L1 || value == "MYSQL_PROTOCOL_TCP"_L1)
        return MYSQL_PROTOCOL_TCP;
    if (value == "SOCKET"_L1 || value == "MYSQL_PROTOCOL_SOCKET"_L1)
        return MYSQL_PROTOCOL_SOCKET;
    if (value == "PIPE"_L1 || value == "MYSQL_PROTOCOL_PIPE"_L1)
        return MYSQL_PROTOCOL_PIPE;
    if (value == "MEMORY"_L1 || value == "MYSQL_PROTOCOL_MEMORY"_L1)
        return MYSQL_PROTOCOL_MEMORY;
    if (value == "DEFAULT"_L1 || value == "MYSQL_PROTOCOL_DEFAULT"_L1)
        return MYSQL_PROTOCOL_DEFAULT;

    throw Exceptions::InvalidArgumentError(
                u"Unknown MySQL connection transport protocol '%1' in %2()."_s
                .arg(value, __tiny_func__));
}

// The MYSQL_OPT_SSL_MODE was added in MySQL 5.7.11
#if defined(MYSQL_VERSION_ID) && MYSQL_VERSION_ID >= 50711 && !defined(MARIADB_VERSION_ID)
bool MySqlDriverPrivate::setOptionSslMode(MYSQL *const mysql, const mysql_option option,
                                          const QStringView value)
{
    const auto sslMode = getOptionSslMode(value);

    return mysql_options(mysql, option, &sslMode) == 0;
}

mysql_ssl_mode MySqlDriverPrivate::getOptionSslMode(const QStringView value)
{
    if (value == "DISABLED"_L1 || value == "SSL_MODE_DISABLED"_L1)
        return SSL_MODE_DISABLED;
    if (value == "PREFERRED"_L1 || value == "SSL_MODE_PREFERRED"_L1)
        return SSL_MODE_PREFERRED;
    if (value == "REQUIRED"_L1 || value == "SSL_MODE_REQUIRED"_L1)
        return SSL_MODE_REQUIRED;
    if (value == "VERIFY_CA"_L1 || value == "SSL_MODE_VERIFY_CA"_L1)
        return SSL_MODE_VERIFY_CA;
    if (value == "VERIFY_IDENTITY"_L1 || value == "SSL_MODE_VERIFY_IDENTITY"_L1)
        return SSL_MODE_VERIFY_IDENTITY;

    throw Exceptions::InvalidArgumentError(
                u"Unknown MySQL SSL mode '%1' in %2()."_s
                .arg(value, __tiny_func__));
}
#endif

void MySqlDriverPrivate::logBoolOptionWarnings(const mysql_option option)
{
    if (option == MYSQL_OPT_RECONNECT)
        qWarning().noquote()
                << u"The MYSQL_OPT_RECONNECT option is still available but is "
                    "deprecated; expect it to be removed in a future version of MySQL, "
                    "in %1()."_s.arg(__tiny_func__);
}

} // namespace Orm::Drivers::MySql

TINYORM_END_COMMON_NAMESPACE
