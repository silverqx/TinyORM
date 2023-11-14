#include "orm/drivers/mysql/mysqldriver_p.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

using namespace Qt::StringLiterals;

namespace Orm::Drivers::MySql
{

/* public */

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
        // CUR drivers throw exception here silverqx
        throw std::exception(
                "MySqlDriver: The MYSQL_OPT_SSL_KEY, MYSQL_OPT_SSL_CERT, and "
                "MYSQL_OPT_SSL_CA should be used instead of the CLIENT_SSL option.");
    else
        // CUR drivers throw exception here, check all others qWarning()-s; I think would be a good idea to throw instead of the qWarning() everywhere silverqx
        qWarning().noquote()
                << QStringLiteral("MySqlDriver::open: Unknown connect option '%1'")
                   .arg(option);
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
    const auto boolValue = value.isEmpty() || value == "TRUE"_L1 || value == "1"_L1;

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
        qWarning().noquote().nospace()
                << "Unknown protocol '" << value << "' - using MYSQL_PROTOCOL_DEFAULT";

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
        qWarning().noquote().nospace()
                << "Unknown ssl mode '" << value << "' - using SSL_MODE_DISABLED";

    return mysql_options(mysql, option, &sslMode) == 0;
}
#endif

} // namespace Orm::Drivers::MySql

TINYORM_END_COMMON_NAMESPACE
