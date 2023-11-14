#pragma once
#ifndef ORM_DRIVERS_SQLDRIVER_HPP
#define ORM_DRIVERS_SQLDRIVER_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QtSql/qtsqlglobal.h>
#include <QVariant>

#include <orm/macros/commonnamespace.hpp>
#include <orm/macros/export.hpp>

class QSqlDatabase;

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers
{

    class SqlDriverPrivate;
    class SqlDriverError;
    class SqlField;
    class SqlRecord;
    class SqlResult;

    class SHAREDLIB_EXPORT SqlDriver
    {
        Q_DISABLE_COPY_MOVE(SqlDriver)
        Q_DECLARE_PRIVATE(SqlDriver)

        friend QSqlDatabase;
        friend class SqlResultPrivate;

    protected:
        /*! Protected constructor. */
        explicit SqlDriver(std::unique_ptr<SqlDriverPrivate> &&dd);

    public:
        enum DriverFeature {
            BatchOperations,
            BLOB,
            CancelQuery,
            EventNotifications,
            FinishQuery,
            LastInsertId,
            LowPrecisionNumbers,
            MultipleResultSets,
            NamedPlaceholders,
            PositionalPlaceholders,
            PreparedQueries,
            QuerySize,
            SimpleLocking,
            Transactions,
            Unicode,
        };

        enum IdentifierType {
            FieldName,
            TableName,
        };

        enum DbmsType {
            UnknownDbms,
            MySqlServer,
            PostgreSQL,
            SQLite,
        };

        /*! Pure virtual destructor. */
        virtual ~SqlDriver() = 0;

        virtual bool
        open(const QString &db, const QString &user, const QString &password,
             const QString &host, int port, const QString &options) = 0;
        virtual void close() = 0;

        virtual bool hasFeature(DriverFeature feature) const = 0;

        virtual bool isOpen() const;
        bool isOpenError() const;

        DbmsType dbmsType() const;
        SqlDriverError lastError() const;

        QSql::NumericalPrecisionPolicy defaultNumericalPrecisionPolicy() const;
        void setDefaultNumericalPrecisionPolicy(QSql::NumericalPrecisionPolicy precision);

        virtual std::unique_ptr<SqlResult> createResult() const = 0;

        virtual QVariant handle() const = 0;

        virtual bool beginTransaction() = 0;
        virtual bool commitTransaction() = 0;
        virtual bool rollbackTransaction() = 0;

        virtual int maximumIdentifierLength(IdentifierType type) const;
        virtual bool isIdentifierEscaped(const QString &identifier,
                                         IdentifierType type) const;
        virtual QString stripDelimiters(const QString &identifier,
                                        IdentifierType type) const;

    protected:
        virtual void setOpen(bool value);
        virtual void setOpenError(bool value);
        virtual void setLastError(const SqlDriverError &error);

        /* Data members */
        std::unique_ptr<SqlDriverPrivate> d_ptr;
    };

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_SQLDRIVER_HPP
