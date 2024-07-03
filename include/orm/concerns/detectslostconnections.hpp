#pragma once
#ifndef ORM_CONCERNS_DETECTSLOSTCONNECTIONS_HPP
#define ORM_CONCERNS_DETECTSLOSTCONNECTIONS_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QtGlobal>

#include "orm/macros/commonnamespace.hpp"
#include "orm/macros/export.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Concerns
{

    /*! Detect lost connection by passed exception message. */
    class TINYORM_EXPORT DetectsLostConnections
    {
        Q_DISABLE_COPY_MOVE(DetectsLostConnections)

    public:
        /*! Default constructor. */
        DetectsLostConnections() = default;
        /*! Pure virtual destructor, to pass -Weffc++. */
        inline virtual ~DetectsLostConnections() = 0;

        /*! Determine if the given exception was caused by a lost connection. */
        static bool causedByLostConnection(const QString &errorMessage);
    };

    /* public */

    DetectsLostConnections::~DetectsLostConnections() = default;

} // namespace Orm::Concerns

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_CONCERNS_DETECTSLOSTCONNECTIONS_HPP
