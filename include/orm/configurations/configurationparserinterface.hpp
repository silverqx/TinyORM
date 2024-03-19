#pragma once
#ifndef ORM_CONFIGURATIONS_CONFIGURATIONPARSERINTERFACE_HPP
#define ORM_CONFIGURATIONS_CONFIGURATIONPARSERINTERFACE_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <QVariantHash>

#include "orm/macros/commonnamespace.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Configurations
{

    /*! Alias for the Connection name (QString). */
    using ConnectionName = QString;

    /*! Configuration parser interface class. */
    class ConfigurationParserInterface
    {
        Q_DISABLE_COPY_MOVE(ConfigurationParserInterface)

    public:
        /*! Default constructor. */
        ConfigurationParserInterface() = default;
        /*! Pure virtual destructor. */
        inline virtual ~ConfigurationParserInterface() = 0;

        /*! Parse and prepare the database configuration. */
        virtual QVariantHash
        parseConfiguration(QVariantHash &config,
                           const ConnectionName &connection) const = 0;
    };

    /* public */

    ConfigurationParserInterface::~ConfigurationParserInterface() = default;

} // namespace Orm::Configurations

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_CONFIGURATIONS_CONFIGURATIONPARSERINTERFACE_HPP
