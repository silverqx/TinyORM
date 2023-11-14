#pragma once
#ifndef ORM_DRIVERS_SQLRESULT_P_HPP
#define ORM_DRIVERS_SQLRESULT_P_HPP

#include <QtSql/private/qtsqlglobal_p.h>
#include <QList>

#include <orm/utils/notnull.hpp>

#include "orm/drivers/sqldrivererror.hpp"

/* Convenience method Q*ResultPrivate::drv_d_func() returns pointer to private driver.
   Compare to Q_DECLARE_PRIVATE in qglobal.h. */
#define Q_DECLARE_SQLDRIVER_PRIVATE(Class)                                               \
    inline const Class##Private *drv_d_func() const                                      \
    {                                                                                    \
        if (sqldriver == nullptr)                                                        \
            return nullptr;                                                              \
                                                                                         \
        return reinterpret_cast<const Class *>(                                          \
                    static_cast<const SqlDriver *>(sqldriver))->d_func();                \
    }                                                                                    \
    inline Class##Private *drv_d_func()                                                  \
    {                                                                                    \
        if (sqldriver == nullptr)                                                        \
            return nullptr;                                                              \
                                                                                         \
        return reinterpret_cast<Class *>(static_cast<SqlDriver *>(sqldriver))->d_func(); \
    }

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers
{

    class SqlDriver;

    class SqlResultPrivate
    {
        Q_DISABLE_COPY_MOVE(SqlResultPrivate)

        /*! Alias for the NotNull. */
        template<typename T>
        using NotNull = Orm::Utils::NotNull<T>;

    public:
        /*! Constructor. */
        SqlResultPrivate(const SqlDriver *driver);
        /*! Default destructor. */
        inline ~SqlResultPrivate() = default;

        /* Prepared queries */
        static bool isVariantNull(const QVariant &value);

        /* Data members */
        NotNull<SqlDriver *> sqldriver;

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
