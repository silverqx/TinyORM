#pragma once
#ifndef ORM_DRIVERS_CONCERNS_SELECTSALLCOLUMNSWITHLIMIT0_HPP
#define ORM_DRIVERS_CONCERNS_SELECTSALLCOLUMNSWITHLIMIT0_HPP

#include <orm/macros/systemheader.hpp>
TINY_SYSTEM_HEADER

#include <QString>

#include <orm/macros/commonnamespace.hpp>

#include "orm/drivers/macros/export.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Drivers
{

    class SqlDriver;
    class SqlQuery;

namespace Concerns
{

    /*! Select all columns in the given table with LIMIT 0 (used by record()). */
    class TINYDRIVERS_EXPORT SelectsAllColumnsWithLimit0
    {
        Q_DISABLE_COPY_MOVE(SelectsAllColumnsWithLimit0)

    public:
        /*! Pure virtual destructor. */
        inline virtual ~SelectsAllColumnsWithLimit0() = 0;

    protected:
        /*! Default constructor. */
        SelectsAllColumnsWithLimit0() = default;

        /* Others */
        /*! Select all columns in the given table with LIMIT 0 (used by record()). */
        SqlQuery
        selectAllColumnsWithLimit0(const QString &table,
                                   const std::weak_ptr<SqlDriver> &driver) const;

    private:
        /* Others */
        /*! Dynamic cast *this to the SqlDriver & derived type. */
        const SqlDriver &sqlDriver() const;
    };

    /* public */

    SelectsAllColumnsWithLimit0::~SelectsAllColumnsWithLimit0() = default;

} // namespace Concerns
} // namespace Orm::Drivers

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_DRIVERS_CONCERNS_SELECTSALLCOLUMNSWITHLIMIT0_HPP
