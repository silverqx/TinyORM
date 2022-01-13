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
    using ConnectionName = QString;

    /*! Connectors interface class. */
    class ConnectorInterface
    {
        Q_DISABLE_COPY(ConnectorInterface)

    public:
        /*! Default constructor. */
        inline ConnectorInterface() = default;
        /*! Pure virtual destructor. */
        inline virtual ~ConnectorInterface() = 0;

        /*! Establish a database connection. */
        virtual ConnectionName connect(const QVariantHash &config) const = 0;
    };

    ConnectorInterface::~ConnectorInterface() = default;

} // namespace Orm::Connectors

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_CONNCECTORS_CONNECTORINTERFACE_HPP
