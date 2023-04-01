#pragma once
#ifndef ORM_CONFIGURATIONS_CONFIGURATIONPARSERFACTORY_HPP
#define ORM_CONFIGURATIONS_CONFIGURATIONPARSERFACTORY_HPP

#include "orm/macros/systemheader.hpp"
TINY_SYSTEM_HEADER

#include <memory>

#include "orm/configurations/configurationparserinterface.hpp"

TINYORM_BEGIN_COMMON_NAMESPACE

namespace Orm::Configurations
{

    /*! Configuration parser factory. */
    class ConfigurationParserFactory
    {
        Q_DISABLE_COPY_MOVE(ConfigurationParserFactory)

    public:
        /*! Deleted default constructor, this is a pure library class. */
        ConfigurationParserFactory() = delete;
        /*! Deleted destructor. */
        ~ConfigurationParserFactory() = delete;

        /*! Get a cached configuration parser instance based on the configuration. */
        static const ConfigurationParserInterface &
        cachedParser(const QVariantHash &config);

        /*! Create a configuration parser instance based on the given driver name. */
        static std::unique_ptr<ConfigurationParserInterface>
        make(const QString &driver);

    private:
        /*! Get a normalized driver name (using the toUpper()). */
        static QString getDriverName(const QVariantHash &config);
    };

} // namespace Orm::Configurations

TINYORM_END_COMMON_NAMESPACE

#endif // ORM_CONFIGURATIONS_CONFIGURATIONPARSERFACTORY_HPP
