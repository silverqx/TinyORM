#pragma once
#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QVariantHash>

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{

    struct Configuration
    {
        /*! Inherit constructors. */
        using ConfigurationsType = QHash<QString, QVariantHash>;

        /*! Default Database Connection Name. */
        QString defaultConnection;

        /*! Database Connections. */
        ConfigurationsType connections;
    };

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // CONFIGURATION_H
