#pragma once
#ifndef ORM_DRIVERS_SQLDRIVER_P_HPP
#define ORM_DRIVERS_SQLDRIVER_P_HPP

#include <QtSql/private/qtsqlglobal_p.h>

#include "orm/drivers/sqldriver.hpp"
#include "orm/drivers/sqldrivererror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers
{

    class SqlDriverPrivate
    {
        Q_DISABLE_COPY_MOVE(SqlDriverPrivate)

    public:
        /*! Constructor. */
        inline explicit SqlDriverPrivate(SqlDriver::DbmsType type);
        /*! Default destructor. */
        inline ~SqlDriverPrivate() = default;

        /* Data members */
        SqlDriverError lastError {};
        SqlDriver::DbmsType dbmsType;
        QSql::NumericalPrecisionPolicy defaultPrecisionPolicy = QSql::LowPrecisionDouble;

        bool isOpen = false;
        bool isOpenError = false;
    };

    /* public */

    SqlDriverPrivate::SqlDriverPrivate(const SqlDriver::DbmsType type)
        : dbmsType(type)
    {}

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_SQLDRIVER_P_HPP
