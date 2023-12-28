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

struct QT_MYSQL_TIME;

// CUR drivers revisit all this types, fix comments silverqx
/* MySQL above version 8 removed my_bool typedef while MariaDB kept it,
   by redefining it we can regain source compatibility. */
using my_bool = decltype (mysql_stmt_bind_result(nullptr, nullptr));

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
            char *fieldValue = nullptr;
            /*! Is the field NULL? */
            my_bool isNull = false;
            /*! Field value buffer length w/o terminating null character. */
            ulong fieldValueSize = 0ul; // For strings varies on the character set (latin1 1 byte or eg. utf8mb4 4 bytes so 3 characters string size will be 12)
        };

        /* Normal queries */
        /*! Populate the fields vector. */
        bool populateFields(MYSQL *mysql);

        /* Prepared queries */
        /*! Bind result set values into the resultBinds data member. */
        bool bindResultValues();

        /*! Determine whether the correct number of prepared bindings was bound. */
        bool shouldPrepareBindings(ulong placeholdersCount) const;
        /*! Check the correct prepared bindings count and show warnings. */
        static void checkPreparedBindingsCount(ulong placeholdersCount, ulong valuesSize);

        /*! Bind prepared bindings for placeholders into the preparedBinds data member. */
        void bindPreparedBindings(
                QList<my_bool> &nullVector, QList<QByteArray> &stringVector,
                QList<QT_MYSQL_TIME> &timeVector);
        /*! Bind BLOB-value type prepared bindings for placeholders. */
//        void bindBlobs();

        /*! Factory method to create the SqlDriverError for prepared statements
            (from MYSQL_STMT). */
        static SqlDriverError
        createStmtError(const QString &error, SqlDriverError::ErrorType type,
                        MYSQL_STMT *stmt);

        /* Result sets */
        /*! Get the error message based on the error code from mysql_stmt_fetch(). */
        static std::optional<QString> fetchErrorMessage(int status) noexcept;

        /*! Obtain the QVariant value for normal queries. */
        QVariant getValueForNormal(int index) const;
        /*! Obtain the QVariant value for prepared queries. */
        QVariant getValueForPrepared(int index) const;

        /* Data members */
        /* Common for both */
        /*! Fields for the currently obtained record/row. */
        QList<MyField> resultFields;

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
        MYSQL_BIND *resultBinds = nullptr;
        /*! Structure for prepared bindings (data values sent to the server). */
        MYSQL_BIND *preparedBinds = nullptr;

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
        static void allocateMemoryForBindings(MYSQL_BIND **binds,
                                              std::size_t count) noexcept;
        /*! Reserve all vectors for prepared bindings buffer data. */
        void reserveVectorsForBindings(
                QList<my_bool> &nullVector, QList<QByteArray> &stringVector,
                QList<QT_MYSQL_TIME> &timeVector) const;

        /*! Determine whether the given MySQL field type is a BLOB. */
        static bool isBlobType(enum_field_types fieldType);
        /*! Convert Qt date/time type to the MYSQL_TIME. */
        static QT_MYSQL_TIME toMySqlDateTime(QDate date, QTime time, int typeId,
                                             MYSQL_BIND *bind);

        /* Result sets */
        /*! Determine whether the given MySQL field type is a Bit-value type. */
        inline static bool isBitType(enum_field_types type) noexcept;
        /*! Convert the Bit-value field to the quint64. */
        static quint64 toBitField(const MyField &field, const char *fieldValue);

        /*! Convert the DATE value to the QDate. */
        static QVariant toQDateFromString(const QString &value);
        /*! Convert the TIME value to the QTime. */
        static QVariant toQTimeFromString(const QString &value);
        /*! Convert the DATETIME value to the QDateTime. */
        static QVariant toQDateTimeFromString(QString &&value);

        /*! Convert the Fixed/Floating-Point value types based on the set numerical
            precision policy. */
        QVariant toDoubleFromString(QString &&value) const;
        /*! Convert the BLOB value type to the QByteArray. */
        QVariant toQByteArray(int index) const;

        /*! Create the QVariant by the given metatype ID and value. */
        QVariant createQVariant(int typeId, QString &&value, int index) const;
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