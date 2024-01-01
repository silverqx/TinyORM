#pragma once
#ifndef ORM_CONCERNS_DETECTSLOSTCONNECTIONS_HPP
#define ORM_CONCERNS_DETECTSLOSTCONNECTIONS_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QtGlobal>

#include "orm/macros/commonnamespace.hpp"
#include "orm/macros/export.hpp"
#include "orm/macros/sqldrivermappings.hpp"

TINY_FORWARD_DECLARE_TSqlError

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm
{

namespace Exceptions
{
    class SqlError;
}

namespace Concerns
{

    /*! Detect lost connection by passed exception message. */
    class SHAREDLIB_EXPORT DetectsLostConnections
    {
        Q_DISABLE_COPY_MOVE(DetectsLostConnections)

    public:
        /*! Default constructor. */
        DetectsLostConnections() = default;
        /*! Pure virtual destructor, to pass -Weffc++. */
        inline virtual ~DetectsLostConnections() = 0;

        /*! Determine if the given exception was caused by a lost connection. */
        static bool causedByLostConnection(const Exceptions::SqlError &e);
        /*! Determine if the given exception was caused by a lost connection. */
        static bool causedByLostConnection(const TSqlError &e);
    };

    /* public */

    DetectsLostConnections::~DetectsLostConnections() = default;

} // namespace Concerns
} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_CONCERNS_DETECTSLOSTCONNECTIONS_HPP
