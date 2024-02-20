#pragma once
#ifndef ORM_DRIVERS_SQLRESULT_P_HPP
#define ORM_DRIVERS_SQLRESULT_P_HPP

#include <QVariant>

#include "orm/drivers/driverstypes.hpp"
#ifdef TINYDRIVERS_MYSQL_LOADABLE_LIBRARY
#  include "orm/drivers/macros/export.hpp"
#endif

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
        explicit SqlResultPrivate(const std::weak_ptr<SqlDriver> &driver) noexcept;
        /*! Default destructor. */
        inline ~SqlResultPrivate() = default;

        /* Prepared queries */
        /*! Determine whether a value stored in the given QVariant isNull(). */
        static bool isVariantNull(const QVariant &value);

        /* Data members */
        /*! Database driver used to access the database connection. */
        std::weak_ptr<SqlDriver> sqldriver;

        /*! The current database connection name (used in exceptions and logs). */
        QString connectionName {};

        /*! The current SQL query text. */
        QString query {};
        /*! Bound values for a prepared statement. */
        QList<QVariant> boundValues {};

        /*! Connection numerical precision policy. */
        NumericalPrecisionPolicy precisionPolicy = LowPrecisionDouble;

        /*! The current cursor position. */
        size_type cursor = BeforeFirstRow;

        /*! Is this result active? (has records to be retrieved) */
        bool isActive = false;
        /*! Is this result from the SELECT statement? */
        bool isSelect = false;
    };

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_SQLFIELD_HPP
