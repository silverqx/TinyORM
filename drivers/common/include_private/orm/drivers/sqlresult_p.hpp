#pragma once
#ifndef ORM_DRIVERS_SQLRESULT_P_HPP
#define ORM_DRIVERS_SQLRESULT_P_HPP

#include <QList>

#include "orm/drivers/driverstypes.hpp"
#include "orm/drivers/sqlerror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers
{

    class SqlDriver;

#ifdef TINYDRIVERS_MYSQL_LOADABLE_LIBRARY
    /*! SqlResult private implementation. */
    class TINYDRIVERS_EXPORT SqlResultPrivate
#else
    /*! SqlResult private implementation. */
    class SqlResultPrivate
#endif
    {
        Q_DISABLE_COPY_MOVE(SqlResultPrivate)

        /*! Expose the NumericalPrecisionPolicy enum. */
        using enum Orm::Drivers::NumericalPrecisionPolicy;

    public:
        /*! Alias for the cursor type. */
        using size_type = int;

        /*! Constructor. */
        explicit SqlResultPrivate(const std::weak_ptr<SqlDriver> &driver);
        /*! Default destructor. */
        inline ~SqlResultPrivate() = default;

        /* Prepared queries */
        /*! Determine whether a value stored in the given QVariant isNull(). */
        static bool isVariantNull(const QVariant &value);

        /* Data members */
        /*! Database driver used to access the database connection. */
        std::weak_ptr<SqlDriver> sqldriver;

        /*! The current SQL query text. */
        QString query;

        /*! Bound values for the prepared statement. */
        QList<QVariant> boundValues;

        /*! The last query error. */
        SqlError lastError;

        /*! Connection numerical precision policy. */
        NumericalPrecisionPolicy precisionPolicy = LowPrecisionDouble;

        /*! The current cursor position. */
        size_type cursor = BeforeFirstRow;

        /*! Is this result active? */
        bool isActive = false;
        /*! Is this result from the SELECT statement? */
        bool isSelect = false;
    };

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_SQLFIELD_HPP
