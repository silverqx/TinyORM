#pragma once
#ifndef ORM_CONFIGURATION_HPP
#define ORM_CONFIGURATION_HPP

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
        /*! Type used for Database Connections. */
        using ConfigurationsType = QHash<QString, QVariantHash>;

        // CUR better naming silverqx
        /*! Default Database Connection Name. */
        QString defaultConnection;

        /*! Database Connections. */
        ConfigurationsType connections;
    };

} // namespace Orm

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_CONFIGURATION_HPP
