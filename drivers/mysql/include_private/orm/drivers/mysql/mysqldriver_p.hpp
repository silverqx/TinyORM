#pragma once
#ifndef ORM_DRIVERS_MYSQL_MYSQLDRIVER_P_HPP
#define ORM_DRIVERS_MYSQL_MYSQLDRIVER_P_HPP

#include "orm/drivers/mysql/macros/includemysqlh_p.hpp"

#include "orm/drivers/sqldriver_p.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers::MySql
{

    /*! MySqlDriver private implementation. */
    class MySqlDriverPrivate final : public SqlDriverPrivate
    {
        Q_DISABLE_COPY_MOVE(MySqlDriverPrivate)

    public:
        /*! Default constructor. */
        inline MySqlDriverPrivate() noexcept;
        /*! Virtual destructor. */
        inline ~MySqlDriverPrivate() final = default;

        /* open() */
        struct SetConnectionOptionsResult;

        /*! Allocate and initialize the MYSQL connection handler object. */
        void mysqlInit();
        /*! Set extra MySQL connection options. */
        SetConnectionOptionsResult mysqlSetConnectionOptions(const QString &options);
        /*! Set the default character set for the mysql_real_connect() function. */
        void mysqlSetCharacterSet(const QString &host, bool before) const;
        /*! Establish a connection to the MySQL server running on the host. */
        void mysqlRealConnect(
                const QString &host, const QByteArray &username,
                const QByteArray &password, const QByteArray &database, int port,
                const QByteArray &unixSocket, uint optionFlags) const;
        /*! Select the default database. */
        void mysqlSelectDb(const QString &database);
        /*! Deallocate the MYSQL connection handler object. */
        void mysqlClose() noexcept;

        /*! The return value type for the mysqlSetConnectionOptions() method. */
        struct SetConnectionOptionsResult
        {
            /*! Connection option flags. */
            uint optionFlags;
            /*! Unix socket path. */
            QString unixSocket;
        };

        /* hasFeature() */
        /*! Determine whether the MySQL connection handler supports transactions. */
        inline bool supportsTransactions() const noexcept;

        /* Data members */
        /*! MYSQL handler. */
        MYSQL *mysql = nullptr;
        /*! The currently selected default database name. */
        QString databaseName {};

    private:
        /* open() */
        /*! Parsed MySQL connection option name and value. */
        struct MySqlOptionParsed
        {
            /*! MySQL connection option name. */
            QStringView option;
            /*! MySQL connection option value. */
            std::optional<QStringView> value;
        };
        /*! Parse the given MySQL connection option to name and value. */
        static MySqlOptionParsed parseMySqlOption(QStringView optionRaw);

        /*! Set the given MySQL connection option. */
        bool mysqlSetConnectionOption(QStringView option, QStringView value);

        /*! Alias type for the setOptionXx() methods. */
        using SetOptionMemFn = std::function<bool(MYSQL *, mysql_option, QStringView)>;
        /*! MySQL connection option and the set method. */
        struct MySqlOptionValue {
            /*! MySQL connection option. */
            mysql_option option;
            /*! The set method for MySQL connection option (calls mysql_option()). */
            SetOptionMemFn setMySqlOption;
        };

        /*! Type for all MySQL options hash. */
        using MySqlOptionsHash = std::unordered_map<QStringView, MySqlOptionValue>;
        /*! Map an option name string to the real MySQL option and the set method. */
        static const MySqlOptionsHash &getMySqlOptionsHash();

        /*! Update the openFlags argument value based on the given flag value. */
        static void setOptionFlag(uint &optionFlags, QStringView option);
        /*! Set the MySQL option to the given string value. */
        static bool setOptionString(MYSQL *mysql, mysql_option option, QStringView value);
        /*! Set the MySQL option to the given unsigned int value. */
        static bool setOptionUInt(MYSQL *mysql, mysql_option option, QStringView value);
        /*! Set the MySQL option to the given boolean value. */
        static bool setOptionBool(MYSQL *mysql, mysql_option option,
                                  QStringView value) noexcept;
        /*! Set the MySQL protocol type option to the given value. */
        static bool setOptionProtocol(MYSQL *mysql, mysql_option option,
                                      QStringView value);
        /*! Get the MySQL protocol type option by the given QString value. */
        static mysql_protocol_type getOptionProtocol(QStringView value);
// The MYSQL_OPT_SSL_MODE was added in MySQL 5.7.11
#if !defined(MARIADB_VERSION_ID) && defined(MYSQL_VERSION_ID) && MYSQL_VERSION_ID >= 50711
        /*! Set the MySQL SSL-mode option to the given value. */
        static bool setOptionSslMode(MYSQL *mysql, mysql_option option,
                                     QStringView value);
        /*! Get the MySQL SSL-mode option by the given QString value. */
        static mysql_ssl_mode getOptionSslMode(QStringView value);
#endif
        /*! Log warnings to the console for some boolean connection options. */
        static void logBoolOptionWarnings(mysql_option option);
        /*! Determine if the given value is the bool true value (true, on, yes, 1). */
        inline static bool isTrueBoolOption(QStringView value) noexcept;

        /*! Convert the given QString to the char array (return nullptr if isNull()). */
        inline static const char *toCharArray(const QByteArray &value) noexcept;

        /*! The default character sets used for connection and SQL statements. */
        constexpr static auto DefaultCharacterSets = std::to_array({"utf8mb4", "utf8"});
    };

    /* public */

    MySqlDriverPrivate::MySqlDriverPrivate() noexcept
        : SqlDriverPrivate(SqlDriver::MySqlServer)
    {}

    /* hasFeature() */

    bool MySqlDriverPrivate::supportsTransactions() const noexcept
    {
        return mysql != nullptr &&
              (mysql->server_capabilities & CLIENT_TRANSACTIONS) == CLIENT_TRANSACTIONS;
    }

    /* private */

    /* open() */

    const char *MySqlDriverPrivate::toCharArray(const QByteArray &value) noexcept
    {
        return value.isNull() ? nullptr : value.constData();
    }

    bool MySqlDriverPrivate::isTrueBoolOption(const QStringView value) noexcept
    {
        using namespace Qt::StringLiterals; // NOLINT(google-build-using-namespace)

        // _L1 is correct here
        return value.compare("true"_L1, Qt::CaseInsensitive) == 0 ||
               value.compare("on"_L1,   Qt::CaseInsensitive) == 0 ||
               value.compare("yes"_L1,  Qt::CaseInsensitive) == 0 ||
               value == '1'_L1;
    }

} // namespace Orm::Drivers::MySql

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_MYSQL_MYSQLDRIVER_P_HPP
