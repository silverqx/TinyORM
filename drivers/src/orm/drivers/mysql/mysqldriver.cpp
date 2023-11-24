#include "orm/drivers/mysql/mysqldriver.hpp"

#include "orm/drivers/mysql/mysqldriver_p.hpp"
#include "orm/drivers/mysql/mysqlresult.hpp"
#include "orm/drivers/mysql/mysqlutils_p.hpp"

#define Q_NO_MYSQL_EMBEDDED

Q_DECLARE_METATYPE(MYSQL_RES *)
Q_DECLARE_METATYPE(MYSQL *)
Q_DECLARE_METATYPE(MYSQL_STMT *)

// MYSQL_TYPE_JSON was added in MySQL 5.7.9
#if defined(MYSQL_VERSION_ID) && MYSQL_VERSION_ID < 50709
#  define MYSQL_TYPE_JSON 245
#endif

TINYORM_BEGIN_COMMON_NAMESPACE

using MySqlUtils = Orm::Drivers::MySql::MySqlUtilsPrivate;

using namespace Qt::StringLiterals;

namespace Orm::Drivers::MySql
{

/* public */

MySqlDriver::MySqlDriver()
    : SqlDriver(std::make_unique<MySqlDriverPrivate>())
{}

namespace
{
    // Defined pointers to static methods to have shorter names
    constexpr auto *const setOptionString   = MySqlDriverPrivate::setOptionString;
    constexpr auto *const setOptionInt      = MySqlDriverPrivate::setOptionInt;
    constexpr auto *const setOptionBool     = MySqlDriverPrivate::setOptionBool;
    constexpr auto *const setOptionProtocol = MySqlDriverPrivate::setOptionProtocol;
#if defined(MYSQL_VERSION_ID) && MYSQL_VERSION_ID >= 50711 && !defined(MARIADB_VERSION_ID)
    constexpr auto *const setOptionSslMode  = MySqlDriverPrivate::setOptionSslMode;
#endif
} // namespace

bool MySqlDriver::open(
        const QString &db, const QString &user, const QString &password,
        const QString &host, const int port, const QString &options)
{
    Q_D(MySqlDriver);

    if (isOpen())
        close();

    if (!(d->mysql = mysql_init(nullptr))) {
        setLastError(MySqlUtils::makeError(
                         QStringLiteral("Unable to allocate a MYSQL object"),
                         SqlDriverError::ConnectionError, d->mysql));
        setOpenError(true);
        return false;
    }

    typedef bool (*SetOptionFunc)(MYSQL*, mysql_option, QStringView);
    struct mysqloptions {
        QLatin1StringView key;
        mysql_option option;
        SetOptionFunc func;
    };

    const mysqloptions allOptions[] = {
        {"SSL_KEY"_L1,                   MYSQL_OPT_SSL_KEY,         setOptionString},
        {"SSL_CERT"_L1,                  MYSQL_OPT_SSL_CERT,        setOptionString},
        {"SSL_CA"_L1,                    MYSQL_OPT_SSL_CA,          setOptionString},
        {"SSL_CAPATH"_L1,                MYSQL_OPT_SSL_CAPATH,      setOptionString},
        {"SSL_CIPHER"_L1,                MYSQL_OPT_SSL_CIPHER,      setOptionString},
        {"MYSQL_OPT_SSL_KEY"_L1,         MYSQL_OPT_SSL_KEY,         setOptionString},
        {"MYSQL_OPT_SSL_CERT"_L1,        MYSQL_OPT_SSL_CERT,        setOptionString},
        {"MYSQL_OPT_SSL_CA"_L1,          MYSQL_OPT_SSL_CA,          setOptionString},
        {"MYSQL_OPT_SSL_CAPATH"_L1,      MYSQL_OPT_SSL_CAPATH,      setOptionString},
        {"MYSQL_OPT_SSL_CIPHER"_L1,      MYSQL_OPT_SSL_CIPHER,      setOptionString},
        {"MYSQL_OPT_SSL_CRL"_L1,         MYSQL_OPT_SSL_CRL,         setOptionString},
        {"MYSQL_OPT_SSL_CRLPATH"_L1,     MYSQL_OPT_SSL_CRLPATH,     setOptionString},
#if defined(MYSQL_VERSION_ID) && MYSQL_VERSION_ID >= 50710
        {"MYSQL_OPT_TLS_VERSION"_L1,     MYSQL_OPT_TLS_VERSION,     setOptionString},
#endif
#if defined(MYSQL_VERSION_ID) && MYSQL_VERSION_ID >= 50711 && !defined(MARIADB_VERSION_ID)
        {"MYSQL_OPT_SSL_MODE"_L1,        MYSQL_OPT_SSL_MODE,        setOptionSslMode},
#endif
        {"MYSQL_OPT_CONNECT_TIMEOUT"_L1, MYSQL_OPT_CONNECT_TIMEOUT, setOptionInt},
        {"MYSQL_OPT_READ_TIMEOUT"_L1,    MYSQL_OPT_READ_TIMEOUT,    setOptionInt},
        {"MYSQL_OPT_WRITE_TIMEOUT"_L1,   MYSQL_OPT_WRITE_TIMEOUT,   setOptionInt},
        {"MYSQL_OPT_RECONNECT"_L1,       MYSQL_OPT_RECONNECT,       setOptionBool},
        {"MYSQL_OPT_LOCAL_INFILE"_L1,    MYSQL_OPT_LOCAL_INFILE,    setOptionInt},
        {"MYSQL_OPT_PROTOCOL"_L1,        MYSQL_OPT_PROTOCOL,        setOptionProtocol},
        {"MYSQL_SHARED_MEMORY_BASE_NAME"_L1,
                                         MYSQL_SHARED_MEMORY_BASE_NAME, setOptionString},
    };

    auto trySetOption = [&](const QStringView &key, const QStringView &value) -> bool {
      for (const mysqloptions &opt : allOptions) {
          if (key == opt.key) {
              if (!opt.func(d->mysql, opt.option, value)) {
                  qWarning("MySqlDriver::open: Could not set connect option value '%s' to '%s'",
                           key.toLocal8Bit().constData(), value.toLocal8Bit().constData());
              }
              return true;
          }
      }
      return false;
    };

    /* This is a hack to get MySQL's stored procedure support working.
       Since a stored procedure _may_ return multiple result sets,
       we have to enable CLIEN_MULTI_STATEMENTS here, otherwise _any_
       stored procedure call will fail.
    */
    uint optionFlags = CLIENT_MULTI_STATEMENTS;
    const QList<QStringView> opts(QStringView(options).split(u';', Qt::SkipEmptyParts));
    QString unixSocket;

    // extract the real options from the string
    for (const auto &option : opts) {
        const QStringView sv = QStringView(option).trimmed();
        qsizetype idx;
        if ((idx = sv.indexOf(u'=')) != -1) {
            const QStringView key = sv.left(idx).trimmed();
            const QStringView val = sv.mid(idx + 1).trimmed();
            if (trySetOption(key, val))
                continue;
            else if (key == "UNIX_SOCKET"_L1)
                unixSocket = val.toString();
            else if (val == "TRUE"_L1 || val == "1"_L1)
                MySqlDriverPrivate::setOptionFlag(optionFlags, key);
            else
                qWarning("MySqlDriver::open: Illegal connect option value '%s'",
                         sv.toLocal8Bit().constData());
        } else {
            MySqlDriverPrivate::setOptionFlag(optionFlags, sv);
        }
    }

    // try utf8 with non BMP first, utf8 (BMP only) if that fails
    static const char wanted_charsets[][8] = { "utf8mb4", "utf8" };
#ifdef MARIADB_VERSION_ID
    MARIADB_CHARSET_INFO *cs = nullptr;
    for (const char *p : wanted_charsets) {
        cs = mariadb_get_charset_by_name(p);
        if (cs) {
            d->mysql->charset = cs;
            break;
        }
    }
#else
    // dummy
    struct {
        const char *csname;
    } *cs = nullptr;
#endif

    MYSQL *mysql = mysql_real_connect(d->mysql,
                                      host.isNull() ? nullptr : host.toUtf8().constData(),
                                      user.isNull() ? nullptr : user.toUtf8().constData(),
                                      password.isNull() ? nullptr : password.toUtf8().constData(),
                                      db.isNull() ? nullptr : db.toUtf8().constData(),
                                      (port > -1) ? port : 0,
                                      unixSocket.isNull() ? nullptr : unixSocket.toUtf8().constData(),
                                      optionFlags);

    if (mysql != d->mysql) {
        setLastError(MySqlUtils::makeError(
                         QStringLiteral("Unable to connect"),
                         // CUR drivers check d->mysql vs mysql, original was d->mysql, also check below and use mysql if possible instead of d->mysql silverqx
                         SqlDriverError::ConnectionError, d->mysql));
        mysql_close(d->mysql);
        d->mysql = nullptr;
        setOpenError(true);
        return false;
    }

    // now ask the server to match the charset we selected
    if (!cs || mysql_set_character_set(d->mysql, cs->csname) != 0) {
        bool ok = false;
        for (const char *p : wanted_charsets) {
            if (mysql_set_character_set(d->mysql, p) == 0) {
                ok = true;
                break;
            }
        }
        if (!ok)
            qWarning("MySQL: Unable to set the client character set to utf8 (\"%s\"). Using '%s' instead.",
                     mysql_error(d->mysql),
                     mysql_character_set_name(d->mysql));
    }

    if (!db.isEmpty() && mysql_select_db(d->mysql, db.toUtf8().constData())) {
        setLastError(MySqlUtils::makeError(
                         QStringLiteral("Unable to open database '%1'").arg(db),
                         SqlDriverError::ConnectionError, d->mysql));
        mysql_close(d->mysql);
        setOpenError(true);
        return false;
    }

    d->dbName = db;

    // CUR drivers called to late? silverqx
#if QT_CONFIG(thread)
    mysql_thread_init();
#endif

    setOpen(true);
    setOpenError(false);
    return true;
}

void MySqlDriver::close()
{
    Q_D(MySqlDriver);
    // Nothing to do
    if (!isOpen())
        return;

    setOpenError(false);
    setOpen(false);

    d->dbName.clear();
    d->mysql = nullptr;

#if QT_CONFIG(thread)
    mysql_thread_end();
#endif
    mysql_close(d->mysql);
}

bool MySqlDriver::hasFeature(const DriverFeature feature) const
{
    Q_D(const MySqlDriver);

    switch (feature) {
    case Transactions:
        return d->supportsTransactions();

    case BatchOperations:
    case CancelQuery:
    case EventNotifications:
    case FinishQuery:
    case MultipleResultSets:
    case NamedPlaceholders:
    case SimpleLocking:
        return false;

    case BLOB:
    case LastInsertId:
    case LowPrecisionNumbers:
    case QuerySize:
    case Unicode:
        return true;

    case PositionalPlaceholders:
    case PreparedQueries:
        return true;

    default:
        Q_UNREACHABLE();
    }

    return false;
}

std::unique_ptr<SqlResult> MySqlDriver::createResult() const
{
    return std::make_unique<MySqlResult>(this);
}

QVariant MySqlDriver::handle() const
{
    Q_D(const MySqlDriver);
    return QVariant::fromValue(d->mysql);
}

bool MySqlDriver::isIdentifierEscaped(const QString &identifier,
                                      const IdentifierType /*unused*/) const
{
    return identifier.size() > 2 &&
           identifier.startsWith(QChar('`')) &&
           identifier.endsWith(QChar('`'));
}

/* protected */

bool MySqlDriver::beginTransaction()
{
    Q_D(MySqlDriver);

    // Nothing to do
    if (!isOpen()) {
        qWarning("MySqlDriver::beginTransaction: Database not open");
        return false;
    }

    if (mysql_query(d->mysql, "START TRANSACTION") == 0)
        return true;

    setLastError(MySqlUtils::makeError(
                     QStringLiteral("Unable to start transaction"),
                     SqlDriverError::StatementError, d->mysql));
    return false;
}

bool MySqlDriver::commitTransaction()
{
    Q_D(MySqlDriver);

    // Nothing to do
    if (!isOpen()) {
        qWarning("MySqlDriver::commitTransaction: Database not open");
        return false;
    }

    if (mysql_query(d->mysql, "COMMIT") == 0)
        return true;

    setLastError(MySqlUtils::makeError(
                     QStringLiteral("Unable to commit transaction"),
                     SqlDriverError::StatementError, d->mysql));
    return false;
}

bool MySqlDriver::rollbackTransaction()
{
    Q_D(MySqlDriver);

    // Nothing to do
    if (!isOpen()) {
        qWarning("MySqlDriver::rollbackTransaction: Database not open");
        return false;
    }

    if (mysql_query(d->mysql, "ROLLBACK") == 0)
        return true;

    setLastError(MySqlUtils::makeError(
                     QStringLiteral("Unable to rollback transaction"),
                     SqlDriverError::StatementError, d->mysql));
    return false;
}

} // namespace Orm::Drivers::MySql

TINYORM_END_COMMON_NAMESPACE
