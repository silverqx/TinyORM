#pragma once
#ifndef ORM_DRIVERS_MYSQL_MYSQLRESULT_P_HPP
#define ORM_DRIVERS_MYSQL_MYSQLRESULT_P_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include "orm/drivers/macros/includemysqlh_p.hpp" // IWYU pragma: keep

#include "orm/drivers/mysql/mysqldriver.hpp"
#include "orm/drivers/sqlresult_p.hpp"

struct QT_MYSQL_TIME;

/* MySQL above version 8 removed my_bool typedef while MariaDB kept it,
   by redefining it we can regain source compatibility. */
using my_bool = decltype (mysql_stmt_bind_result(nullptr, nullptr));

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers::MySql
{

    class MySqlResultPrivate : public SqlResultPrivate
    {
        Q_DISABLE_COPY_MOVE(MySqlResultPrivate)

    public:
        Q_DECLARE_SQLDRIVER_PRIVATE(MySqlDriver)

        /*! Inherit constructors. */
        using SqlResultPrivate::SqlResultPrivate;

        struct MyField
        {
            // CUR drivers rename to inField? silverqx
            char *outField = nullptr;
            // CUR drivers remove? silverqx
            // CUR drivers rename to mysqlField silverqx
            const MYSQL_FIELD *myField = nullptr; // Returned by mysql_fetch_field()
            // CUR drivers rename to typeId silverqx
            QMetaType type {};
            my_bool nullIndicator = false;
            ulong bufferLength = 0ul; // Varies on the character set (latin1 1 byte or utf8mb4 4 bytes), w/o terminating null character
        };

        /* Normal queries */
        bool populateFields(MYSQL *mysql);

        /* Prepared queries */
        bool bindInValues();
        /*! Determine whether the correct number of prepared bindings was bound. */
        bool hasPreparedBindings(ulong placeholdersCount) const;
        void bindPreparedBindings(
                QList<my_bool> &nullVector, QList<QByteArray> &stringVector,
                QList<QT_MYSQL_TIME> &timeVector);
//        void bindBlobs();

        /* Result sets */
        QVariant getValueForNormal(int index) const;
        QVariant getValueForPrepared(int index) const;

        /* Others */
        static SqlDriverError
        makeStmtError(const QString &error, SqlDriverError::ErrorType type,
                      MYSQL_STMT *stmt);

        /* Data members */
        MYSQL_RES *result = nullptr;
        MYSQL_ROW row = nullptr; // MYSQL_ROW is pointer

        QList<MyField> fields;

        MYSQL_STMT *stmt = nullptr;
        MYSQL_RES *meta = nullptr;

        MYSQL_BIND *inBinds = nullptr; // For results returned from the database
        MYSQL_BIND *outBinds = nullptr; // Prepared bindings that will be sent to the database

        bool hasBlobs = false;
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

        /*! Determine whether the given MySQL column type is a BLOB. */
        static bool isBlobType(enum_field_types fieldType);
        static QT_MYSQL_TIME toMySqlDate(QDate date, QTime time, int typeId,
                                         MYSQL_BIND *bind);

        /* Result sets */
        inline static bool isBitType(enum_field_types type) noexcept;
        static uint64_t toBitField(const MyField &field, const char *outField);

        static QVariant toQDateFromString(const QString &value);
        static QVariant toQTimeFromString(const QString &value);
        static QVariant toQDateTimeFromString(QString &&value);

        QVariant toQVariantDouble(QString &&value) const;
        QVariant toQVariantByteArray(int index) const;

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
