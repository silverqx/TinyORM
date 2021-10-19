#pragma once
#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QVariantHash>

#include "orm/macros/commonnamespace.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

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

TINYORM_END_COMMON_NAMESPACE

#endif // CONFIGURATION_HPP
