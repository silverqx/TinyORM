#pragma once
#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QVariantHash>

#ifdef TINYORM_COMMON_NAMESPACE
namespace TINYORM_COMMON_NAMESPACE
{
#endif
namespace Orm
{

    /*! Database configuration class. */
    struct Configuration
    {
        /*! Inherit constructors. */
        using ConfigurationsType = QHash<QString, QVariantHash>;

        /*! Default Database Connection Name. */
        QString defaultConnection = {};

        /*! Database Connections. */
        ConfigurationsType connections = {};
    };

} // namespace Orm
#ifdef TINYORM_COMMON_NAMESPACE
} // namespace TINYORM_COMMON_NAMESPACE
#endif

#endif // CONFIGURATION_HPP
