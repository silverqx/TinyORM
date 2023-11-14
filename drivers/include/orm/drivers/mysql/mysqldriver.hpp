#pragma once
#ifndef ORM_DRIVERS_MYSQL_MYSQLDRIVER_HPP
#define ORM_DRIVERS_MYSQL_MYSQLDRIVER_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#ifdef _WIN32
#  include <QtCore/qt_windows.h>
#endif

#include "orm/drivers/sqldriver.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers::MySql
{

    class MySqlDriverPrivate;

    class SHAREDLIB_EXPORT MySqlDriver final : public SqlDriver
    {
        Q_DISABLE_COPY_MOVE(MySqlDriver)
        Q_DECLARE_PRIVATE(MySqlDriver)

        friend class MySqlResultPrivate;

    public:
        /*! Default constructor. */
        MySqlDriver();
        /*! Virtual destructor. */
        inline ~MySqlDriver() final = default;

        bool open(const QString &db, const QString &user, const QString &password,
                  const QString &host, int port, const QString &options) final;
        void close() final;

        bool hasFeature(DriverFeature feature) const final;

        std::unique_ptr<SqlResult> createResult() const final;

        QVariant handle() const final;

        bool isIdentifierEscaped(const QString &identifier,
                                 IdentifierType type) const final;

    protected:
        bool beginTransaction() final;
        bool commitTransaction() final;
        bool rollbackTransaction() final;
    };

} // namespace Orm::Drivers::MySql

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_MYSQL_MYSQLDRIVER_HPP
