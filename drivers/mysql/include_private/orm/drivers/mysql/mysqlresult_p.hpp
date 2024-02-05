#pragma once
#ifndef ORM_DRIVERS_MYSQL_MYSQLRESULT_P_HPP
#define ORM_DRIVERS_MYSQL_MYSQLRESULT_P_HPP

#include "orm/drivers/mysql/macros/includemysqlh_p.hpp"

#include "orm/drivers/macros/declaresqldriverprivate_p.hpp"
#include "orm/drivers/mysql/mysqldriver.hpp"
#include "orm/drivers/sqlresult_p.hpp"

/* All the in vs out bindings/data-related comments and identifier names related
   to the results or prepared bindings are described from the MySQL server perspective and
   they follow the MySQL documentation conventions.
   Input bindings are prepared statements and output bindings are results from the MySQL
   server. Is a very bad idea to switch these naming conventions. 🤔
   Instead of using the input/in vs output/out words I'm using prepared vs result words
   where possible to avoid confusion. */

/* MySQL >=8.0.1 removed my_bool typedef while MariaDB kept it, so it's still needed to
   define it for MariaDB because my_bool == char and compilation fails with the bool type.
   See https://bugs.mysql.com/bug.php?id=85131 */
#if defined(MARIADB_VERSION_ID) || MYSQL_VERSION_ID < 80001
using my_bool = decltype (mysql_stmt_bind_result(nullptr, nullptr));
#else
using my_bool = bool;
#endif

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers::MySql
{

    // CUR drivers finish and revisit whole MySqlResult/Private class silverqx
    /*! MySqlResult private implementation. */
    class MySqlResultPrivate : public SqlResultPrivate
    {
        Q_DISABLE_COPY_MOVE(MySqlResultPrivate)

    public:
        TINY_DECLARE_SQLDRIVER_PRIVATE(MySqlDriver)

        /*! Inherit constructors. */
        using SqlResultPrivate::SqlResultPrivate;

        /*! MySQL field type (used for result sets' fields). */
        struct MyField
        {
            /* Common for both */
            /*! Field metatype. */
            QMetaType metaType {};
            // CUR drivers remove? silverqx
            /*! Field metadata. */
            const MYSQL_FIELD *myField = nullptr; // Returned by mysql_fetch_field()
            /* Prepared queries */
            /*! Field value buffer in the result set. */
            std::unique_ptr<char[]> fieldValue = nullptr; // NOLINT(modernize-avoid-c-arrays)
            /*! Is the field NULL? */
            my_bool isNull = false;
            /*! Field value buffer length w/o terminating null character. */
            ulong fieldValueSize = 0UL; // For strings varies on the character set (latin1 1 byte or eg. utf8mb4 4 bytes so 3 characters string size will be 12)
        };

        /* Normal queries */
        /*! Populate the fields vector. */
        bool populateFields(MYSQL *mysql);

        /* Prepared queries */
        /*! Bind result set values into the resultBinds data member. */
        bool bindResultValues();

        /*! Determine whether the correct number of prepared bindings was bound. */
        bool shouldPrepareBindings(uint placeholdersCount) const;
        /*! Check the correct prepared bindings count and show warnings. */
        static void checkPreparedBindingsCount(uint placeholdersCount, ulong valuesSize);

        /*! Bind prepared bindings for placeholders into the preparedBinds data member. */
        void bindPreparedBindings(
                QList<my_bool> &nullVector, QList<QByteArray> &stringVector,
                QList<MYSQL_TIME> &timeVector);

        /*! Bind result set BLOB values. */
        void bindResultBlobs();

        /*! Factory method to create the SqlError for prepared statements
            (from MYSQL_STMT). */
        static SqlError createStmtError(const QString &error, SqlError::ErrorType type,
                                        MYSQL_STMT *stmt);

        /* Result sets */
        /*! Alias for the result fields vector type. */
        using ResultFieldsType = std::vector<MyField>;
        /*! Alias for the result fields vector size type. */
        using ResultFieldsSizeType = ResultFieldsType::size_type;

        /*! Get the error message based on the error code from mysql_stmt_fetch(). */
        static std::optional<QString> fetchErrorMessage(int status) noexcept;

        /*! Obtain the QVariant value for normal queries. */
        QVariant getValueForNormal(ResultFieldsSizeType index) const;
        /*! Obtain the QVariant value for prepared queries. */
        QVariant getValueForPrepared(ResultFieldsSizeType index) const;

        /*! Throw an exception if an index for result fields vector is out of bounds. */
        void throwIfBadResultFieldsIndex(ResultFieldsSizeType index) const;

        /* Data members */
        /* Common for both */
        /*! Fields for the currently obtained record/row. */
        ResultFieldsType resultFields {};

        /* Normal queries */
        /*! Result set handle (from the mysql_store_result()). */
        MYSQL_RES *result = nullptr;
        /*! Pointer to the row in the result set (from the mysql_fetch_row()). */
        MYSQL_ROW row = nullptr;

        /* Prepared queries */
        /*! Prepared statement handler. */
        MYSQL_STMT *stmt = nullptr;
        /*! Result set metadata for a prepared statement. */
        MYSQL_RES *meta = nullptr;

        /*! Structure to bind buffers to result set columns (result values returned from
            the database server). */
        std::unique_ptr<MYSQL_BIND[]> resultBinds = nullptr; // NOLINT(modernize-avoid-c-arrays)
        /*! Structure for prepared bindings (data values sent to the server). */
        std::unique_ptr<MYSQL_BIND[]> preparedBinds = nullptr; // NOLINT(modernize-avoid-c-arrays)

        /* Common for both */
        /*! Has the current result set any BLOB type field/s? */
        bool hasBlobs = false;
        /*! Is the current result set for the prepared statement? */
        bool preparedQuery = false;

    private:
        /* Prepared queries */
        /*! Verify whether all fields metadata were fetched. */
        static bool wasAllFieldsFetched(uint fieldsCount, uint lastIndex,
                                        QLatin1StringView method);

        /*! Allocate memory for prepared bindings that will be sent to the database. */
        static void allocateMemoryForBindings(std::unique_ptr<MYSQL_BIND[]> &binds, // NOLINT(modernize-avoid-c-arrays)
                                              std::size_t count) noexcept;
        /*! Reserve all vectors for prepared bindings buffer data. */
        void reserveVectorsForBindings(
                QList<my_bool> &nullVector, QList<QByteArray> &stringVector,
                QList<MYSQL_TIME> &timeVector) const;

        /*! Determine whether the given MySQL field type is a BLOB. */
        static bool isBlobType(enum_field_types fieldType) noexcept;
        /*! Convert Qt date/time type to the MYSQL_TIME. */
        static MYSQL_TIME toMySqlDateTime(QDate date, QTime time, int typeId,
                                          MYSQL_BIND &bind);

        /* Result sets */
        /*! Determine whether the given MySQL field type is a Bit-value type. */
        inline static bool isBitType(enum_field_types type) noexcept;
        /*! Convert the Bit-value field to the quint64. */
        static quint64 toBitField(const MyField &field, const char *fieldValue) noexcept;

        /*! Convert the DATE value to the QDate. */
        static QVariant toQDateFromString(const QString &value);
        /*! Convert the TIME value to the QTime. */
        static QVariant toQTimeFromString(const QString &value);
        /*! Convert the DATETIME value to the QDateTime. */
        static QVariant toQDateTimeFromString(QString value);
        /*! Convert the DATE/TIME value to the QDateTime (prepared statements only). */
        static QVariant toQDateTimeFromMySQLTime(int typeId,
                                                 const MYSQL_TIME *mysqlTime);

        /*! Convert the Fixed/Floating-Point value types based on the set numerical
            precision policy. */
        QVariant toDoubleFromString(const QString &value) const;
        /*! Convert the BLOB value type to the QByteArray. */
        QVariant toQByteArray(ResultFieldsSizeType index) const;

        /*! Create the QVariant by the given metatype ID and value. */
        QVariant createQVariant(int typeId, QString &&value,
                                ResultFieldsSizeType index) const;
    };

    /* private */

    /* Result sets */

    bool MySqlResultPrivate::isBitType(const enum_field_types type) noexcept
    {
        return type == MYSQL_TYPE_BIT;
    }

} // namespace Orm::Drivers::MySql

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_MYSQL_MYSQLRESULT_P_HPP
