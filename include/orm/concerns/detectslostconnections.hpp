#pragma once
#ifndef ORM_CONCERNS_DETECTSLOSTCONNECTIONS_HPP
#define ORM_CONCERNS_DETECTSLOSTCONNECTIONS_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QtGlobal>

#include "orm/macros/commonnamespace.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

class QSqlError;

namespace Orm
{

namespace Exceptions
{
    class SqlError;
}

namespace Concerns
{

    /*! Detect lost connection by passed exception message. */
    class DetectsLostConnections
    {
        Q_DISABLE_COPY(DetectsLostConnections)

    public:
        /*! Default constructor. */
        inline DetectsLostConnections() = default;
        /*! Virtual destructor, to pass -Weffc++. */
        inline virtual ~DetectsLostConnections() = default;

        /*! Determine if the given exception was caused by a lost connection. */
        bool causedByLostConnection(const Exceptions::SqlError &e) const;
        /*! Determine if the given exception was caused by a lost connection. */
        bool causedByLostConnection(const QSqlError &e) const;
    };

} // namespace Concerns
} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_CONCERNS_DETECTSLOSTCONNECTIONS_HPP
