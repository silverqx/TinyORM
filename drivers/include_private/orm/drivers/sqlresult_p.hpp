#pragma once
#ifndef ORM_DRIVERS_SQLRESULT_P_HPP
#define ORM_DRIVERS_SQLRESULT_P_HPP

#include <QtSql/private/qtsqlglobal_p.h>
#include <QList>

#include "orm/drivers/sqldrivererror.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers
{

    class SqlDriver;

    class SqlResultPrivate
    {
        Q_DISABLE_COPY_MOVE(SqlResultPrivate)

    public:
        /*! Constructor. */
        explicit SqlResultPrivate(const std::weak_ptr<SqlDriver> &driver);
        /*! Default destructor. */
        inline ~SqlResultPrivate() = default;

        /* Prepared queries */
        static bool isVariantNull(const QVariant &value);

        /* Data members */
        std::weak_ptr<SqlDriver> sqldriver;

        QString query;

        QList<QVariant> values;

        SqlDriverError lastError;

        QSql::NumericalPrecisionPolicy precisionPolicy = QSql::LowPrecisionDouble;

        int cursor = QSql::BeforeFirstRow;

        bool active = false;
        bool select = false;
    };

} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_SQLFIELD_HPP
