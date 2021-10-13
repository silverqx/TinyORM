#pragma once
#ifndef CONNECTORINTERFACE_HPP
#define CONNECTORINTERFACE_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QVariantHash>

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
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
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // CONNECTORINTERFACE_HPP
