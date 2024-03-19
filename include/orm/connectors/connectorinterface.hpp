#pragma once
#ifndef ORM_CONNCECTORS_CONNECTORINTERFACE_HPP
#define ORM_CONNCECTORS_CONNECTORINTERFACE_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QVariantHash>

#include "orm/macros/commonnamespace.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Connectors
{
    /*! Type for the Connection name (QString). */
    using ConnectionName = QString;

    /*! Connectors interface class. */
    class ConnectorInterface
    {
        Q_DISABLE_COPY_MOVE(ConnectorInterface)

    public:
        /*! Default constructor. */
        ConnectorInterface() = default;
        /*! Pure virtual destructor. */
        inline virtual ~ConnectorInterface() = 0;

        /*! Establish a database connection. */
        virtual ConnectionName connect(const QVariantHash &config) const = 0;
    };

    /* public */

    ConnectorInterface::~ConnectorInterface() = default;

} // namespace Orm::Connectors

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_CONNCECTORS_CONNECTORINTERFACE_HPP
