#pragma once
#ifndef ORM_CONNECTORINTERFACE_HPP
#define ORM_CONNECTORINTERFACE_HPP

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
    public:
        /*! Pure virtual destructor. */
        virtual ~ConnectorInterface() = 0;

        /*! Establish a database connection. */
        virtual ConnectionName connect(const QVariantHash &config) const = 0;
    };

    inline ConnectorInterface::~ConnectorInterface() = default;

} // namespace Orm::Connectors

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_CONNECTORINTERFACE_HPP
